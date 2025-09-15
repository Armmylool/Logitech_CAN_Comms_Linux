#include "brake.h"

 uint8_t Move_Pre_command[8] = {0x08, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00};
uint16_t checksum;
// unsigned char low_byte;
// unsigned char high_byte;
unsigned char byteArray[2];

uint8_t calculate_checksum(uint8_t *data, size_t length) {
    checksum = 0;
    for (size_t i = 0; i < length - 1; i++) {
        checksum += data[i];
    }
    return (uint8_t)(checksum & 0xFF);
}

void process_data(int value) {
     uint16_t value_cali_brake = 800 - (((int32_t)value + 32768) * 800 / 65535);
    unsigned char lowerByte = value_cali_brake & 0xFF ;          
    unsigned char higherByte = (value_cali_brake >> 8) & 0xFF ;    

    Move_Pre_command[3] = lowerByte ;
    Move_Pre_command[4] = higherByte ;
    Move_Pre_command[7] = calculate_checksum(Move_Pre_command,sizeof(Move_Pre_command)) ;
    //Serial_Write(hSerial, Move_Pre_command, sizeof(Move_Pre_command)) ;
}
