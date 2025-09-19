#include "steering.h"
#include "Port.h"
#include <string.h> // For memset

// Define the CAN ID directly as a number.
// "06000001" is a hex string, which converts to a decimal CAN ID.
// We also need to set the EFF (Extended Frame Format) flag.
#define WHEEL_CAN_ID 0x06000001 | CAN_EFF_FLAG

struct Keya_Command wheel_comms = {
    {0x23, 0x0D, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00},  // Enable Command
    {0x23, 0x0D, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00},  // Disable Command
    {0x23, 0x02, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00},  // Header of Position Mode
    {0x23, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00}
};

// This function is no longer needed as we build the data directly.
// void create_extended_packet(...) {}

void enableWheel(void) {
    struct can_frame frame;
    frame.can_id = WHEEL_CAN_ID;
    frame.can_dlc = 8;
    memcpy(frame.data, wheel_comms.ENABLE, 8);
    CAN_Write(&frame);
}

void disableWheel(void) {
    struct can_frame frame;
    frame.can_id = WHEEL_CAN_ID;
    frame.can_dlc = 8;
    memcpy(frame.data, wheel_comms.DISABLE, 8);
    CAN_Write(&frame);
}

void receiveDataFromG29(Sint16 data) {
    struct can_frame frame;
    frame.can_id = WHEEL_CAN_ID;
    frame.can_dlc = 8;

    // Scale the joystick data
    int16_t scaleData = -1 * (data / 1.31068);
    SDL_Log("Steering wheel scale data : %d", scaleData);

    // Prepare the data payload directly in the frame
    // This combines the logic from create_extended_packet and CAN_Write_integer
    memcpy(frame.data, wheel_comms.POSITION, 4); // Copy the header "23022001"

    // Place the scaled data into the last 4 bytes (Big Endian)
    frame.data[4] = (scaleData >> 8) & 0xFF;
    frame.data[5] = scaleData & 0xFF;
    
    if (scaleData < 0) {
        frame.data[6] = 0xFF;
        frame.data[7] = 0xFF;
    } else {
        frame.data[6] = 0x00;
        frame.data[7] = 0x00;
    }

    CAN_Write(&frame);
}

void speedMode(int speed) {
     struct can_frame frame ;
     frame.can_id = WHEEL_CAN_ID ;
     frame.can_dlc = 8 ;
     memcpy(frame.data, wheel_comms.SPEED, 4) ;
     frame.data[4] = (speed >> 8) & 0xFF ;
     frame.data[5] = speed & 0xFF ;

     if (speed < 0) {
        frame.data[6] = 0xFF ;
        frame.data[7] = 0xFF ;
     } else {
        frame.data[6] = 0x00 ;
        frame.data[7] = 0x00 ;
     }
     CAN_Write(&frame) ;
}
