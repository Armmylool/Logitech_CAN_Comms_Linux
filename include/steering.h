#ifndef STEERING_H_
#define STEERING_H_

#include "G29.h"

struct Keya_Command {
    const uint8_t ENABLE[8];
    const uint8_t DISABLE[8];
    const uint8_t POSITION[8];
};

extern struct Keya_Command wheel_comms;

void enableWheel(void);
void disableWheel(void);
void receiveDataFromG29(Sint16 data);

#endif
