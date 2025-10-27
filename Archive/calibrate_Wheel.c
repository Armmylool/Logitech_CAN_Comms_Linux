#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include "G29.h"
#include "Port.h"
#include "steering.h"

// --- Function Prototypes ---
int16_t extract_signed_16bit(uint8_t msb, uint8_t lsb);
int16_t middle_position ;
float scale_left_max_position = 0 ;
float scale_right_max_position = 0 ;

// --- State Machine States ---
typedef enum {
    INITIALISE,
    TURN_LEFT,
    TURN_RIGHT,
    SET_MIDDLE,
    END_PROCESS
} CalibrationState;

int main() {
    const char* can_interface = "can0";

    // Initialize the SocketCAN interface
    if (socketcan_init(can_interface) < 0) {
        fprintf(stderr, "ERROR: Cannot initialize CAN socket on %s\n", can_interface);
        return 1;
    }

    struct can_frame frame;
    CalibrationState currentState = INITIALISE; // Start at the initial state
    bool isRunning = true;

    int left_max_position = 0;
    int right_max_position = 0; // You will likely need a variable for this state

    printf("Starting calibration process...\n");
    changeSpeedMode() ; // Change to speed mode
    usleep(10000) ;
    enableWheel(); // Enable the steering wheel
    usleep(10000); // Give the wheel a moment to enable

    while (isRunning) {
        // The main state machine logic is handled by this switch statement
        switch (currentState) {
            case INITIALISE:
                printf("State: INITIALISE - Setting wheel to turn left...\n");
                speedMode(500); // Assuming this tells the wheel to start turning
                currentState = TURN_LEFT; // Move to the next state
                break;

            case TURN_LEFT:
                // In this state, we wait for the wheel to hit the left maximum
                if (CAN_Read(&frame) > 0) {
                    if ((frame.can_id & CAN_EFF_MASK) == 0x7000001) {
                        int16_t extracted_val = extract_signed_16bit(frame.data[4], frame.data[5]);

                        // Check if the wheel has hit the lock (value is high enough)
                        if (extracted_val >= 17 || extracted_val <= -17) {
                            left_max_position = extract_signed_16bit(frame.data[0], frame.data[1]);
                            scale_left_max_position = (left_max_position / 360.0) * 10000 ;
                            printf("State: TURN_LEFT - Left max position found: %d\n", left_max_position);
                            printf("State: TURN_LEFT - Left max position found: %f\n",scale_left_max_position) ;
                            // Now, tell the wheel to turn the other way
                            speedMode(-500); // Assuming negative speed is right
                            sleep(5) ;
                            currentState = TURN_RIGHT; // Move to the next state
                        }
                    }
                }
                break;

            case TURN_RIGHT:
                 if (CAN_Read(&frame) > 0) {
                    if ((frame.can_id & CAN_EFF_MASK) == 0x7000001) {
                        int16_t extracted_val = extract_signed_16bit(frame.data[4], frame.data[5]);

                        // Check if the wheel has hit the lock (value is high enough)
                        if (extracted_val >= 17 || extracted_val <= -16) {
                            right_max_position = extract_signed_16bit(frame.data[0], frame.data[1]);
                            scale_right_max_position = (right_max_position / 360.0) * 10000 ;
                            printf("State: TURN_RIGHT - Right max position found: %d\n", right_max_position);
                            printf("State: TURN_RIGHT - Right max position found (scale) : %f\n", scale_right_max_position) ;
                            currentState = SET_MIDDLE; // Move to the next state
                        }
                    }
                }
                break;

            case SET_MIDDLE:
                printf("State: SET_MIDDLE - Centering the wheel...\n");
                // TODO: Add logic to command the wheel to go to the calculated center.
                middle_position = (scale_left_max_position + scale_right_max_position) / 2 ;
                printf("MIDDLE_POSITION : %d\n", middle_position) ;
                currentState = END_PROCESS;
                break;
            
            case END_PROCESS:
                printf("State: END_PROCESS - Calibration finished.\n");
                disableWheel();
                usleep(10000) ;
                changeAbsolutePositionMode() ; // Changing back to Absolute position mode
                usleep(10000) ;
                enableWheel() ;
                usleep(10000) ;
                absolutePosition(middle_position) ;
                isRunning = false; // Set to false to exit the while loop
                break;
        }
    }
    sleep(1) ;
    disableWheel() ;
    CAN_Close();
    printf("Program finished.\n");
    return 0;
}

/**
 * @brief Combines an MSB and LSB into a signed 16-bit integer.
 */
int16_t extract_signed_16bit(uint8_t msb, uint8_t lsb) {
    return (int16_t)((msb << 8) | lsb);
}
