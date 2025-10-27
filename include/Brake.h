#ifndef BRAKE_H_
#define BRAKE_H_
#include "G29.h"
#include "Port.h"

struct stepperMotorCommand {
	const uint8_t HOME[4] ;
	const uint8_t MOVE[8] ;
} ;

extern struct stepperMotorCommand brake_comms ;


void home_Setting(void) ;

/** @brief checksum for Brake cmd. */
uint8_t calculate_checksum(uint8_t *data, size_t length) ;

/** @brief process_data*/
void brake_Control(int raw_data) ;

#endif
