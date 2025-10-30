#include "Brake.h"
#include "Port.h"

#define Brake_CAN_ID 0x06000002 | CAN_EFF_FLAG

struct stepperMotorCommand brake_comms = {
	{0x04, 0x01, 0x10, 0x15} , /* Fix the Slave ID is 1 ,, with Pattern : msg Length | ID | Command | CheckSum */
	{0x08, 0x01, 0x13, 0x00, 0x00, 0x00 ,0x00, 0x00}
} ;

unsigned char byteArray[2];


void home_Setting(void) {
    struct can_frame frame ;
    frame.can_id = Brake_CAN_ID ;
    frame.can_dlc = 4 ;
    memcpy(frame.data, brake_comms.HOME, 4) ;
    CAN_Write(&frame);
}

uint8_t calculate_checksum(uint8_t *data, size_t length) {
    uint16_t checksum = 0;
    for (size_t i = 0; i < length ; i++) {
        checksum += data[i];
    }
    return (uint8_t)(checksum & 0xFF);
}




void brake_Control(int value) {
    struct can_frame frame ;
    frame.can_id = Brake_CAN_ID ;
    frame.can_dlc = 8 ;
    memcpy(frame.data, brake_comms.MOVE, 8) ;
    uint16_t value_cali_brake = 800 - (((int32_t)value + 32768) * 800 / 65535);
    //printf("Value : %d\n", value_cali_brake) ;
    unsigned char lowerByte = value_cali_brake & 0xFF ;
    unsigned char higherByte = (value_cali_brake >> 8) & 0xFF ;

    frame.data[3] = lowerByte ;
    frame.data[4] = higherByte ;
    frame.data[7] = calculate_checksum(frame.data,7) ;
    /*printf("Sending frame: ");
    for (int i = 0; i < 8; i++) printf("%02X ", frame.data[i]);
    printf("\n"); */
    CAN_Write(&frame) ;
}

