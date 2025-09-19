#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "G29.h"
#include "Port.h"
#include "steering.h"

const char* can_interface = "can0" ;


//-------- Machine State------- //
enum State { initialise, turnLeft, turnRight, set_middle_position, end_process} ;
enum State calibrate = initialise ;
bool find_left_max = true ;

int16_t extract_signed_16bit(uint8_t msb, uint8_t lsb) ;

int main() {
    // Initialize the SocketCAN interface
    if (socketcan_init(can_interface) < 0) {
        fprintf(stderr, "ERROR: Cannot initialize CAN socket on %s\n", can_interface);
        return 1;
    }
    struct can_frame frame;

    enableWheel() ;     // Enable the steering wheel.
    usleep(1000) ;
    switch (calibrate) {
       case initialise : 
       speedMode(100) ;
       printf("GET IN TO LOOP") ;
       while (find_left_max) {
             printf("IN LOOP") ;
             if (CAN_Read(&frame) > 0) {
                printf("Received CAN frame: ID=0x%X, DLC=%d, Data=", frame.can_id, frame.can_dlc);
		if (frame.can_id == 0x07000001) {
                   int16_t extracted_val = extract_signed_16bit(frame.data[4], frame.data[5]) ;
                   if (extracted_val >= 17 || extracted_val <= -17){
                      int left_max_position = extract_signed_16bit(frame.data[0], frame.data[1]) ;
                      printf("LEFT MAX POSITION : %d ", left_max_position) ;
                      find_left_max = false ;
                   }
		}
          }
       }
    }
}

int16_t extract_signed_16bit(uint8_t msb, uint8_t lsb) {
    return (int16_t)((msb << 8) | lsb);
}
