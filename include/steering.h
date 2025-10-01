#ifndef STEERING_H_
#define STEERING_H_

#include "G29.h"

struct Keya_Command {
    const uint8_t ENABLE[8];
    const uint8_t DISABLE[8];
    const uint8_t POSITION[8];
    const uint8_t SPEED[8] ;
    const uint8_t SPEED_MODE[8] ;
    const uint8_t ABSOLUTE_MODE[8] ;
};

extern struct Keya_Command wheel_comms;

void enableWheel(void);
void disableWheel(void);
void changeSpeedMode(void) ;
void changeAbsolutePositionMode(void) ;
void speedMode(int16_t speed) ;
void receiveDataFromG29(Sint16 data);
void absolutePosition(int16_t position) ;

#endif
