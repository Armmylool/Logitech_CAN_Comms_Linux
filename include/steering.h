#ifndef STEERING_H_      // 1. เพิ่ม Include Guard (ถ้ายังไม่มี STEERING_H_)
#define STEERING_H_      // ให้ประกาศ STEERING_H_ ขึ้นมา

#include "G29.h"

struct Keya_Command {
    const char* ENABLE;
    const char* DISABLE;
    const char* POSITION;
};

extern struct Keya_Command wheel_comms;

void create_extended_packet(int16_t data, uint8_t* packet_buffer) ;
void enableWheel(int fd);
void disableWheel(int fd) ;
void receiveDataFromG29(int fd, Sint16 data);

#endif
