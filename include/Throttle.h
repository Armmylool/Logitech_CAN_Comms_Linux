#ifndef THROTTLE_H_
#define THROTTLE_H_

#include "G29.h"

struct throttle_Command {
       const char* THROTTLE ;
}

extern struct throttle_Command throttle_comms ;

void throttle_prepare(int fd, Sint16 data);

#endif
