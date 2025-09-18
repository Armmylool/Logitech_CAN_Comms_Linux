#ifndef THROTTLE_H_
#define THROTTLE_H_

#include "G29.h" // For Sint16 type

/**
 * @struct throttle_Command
 * @brief Defines the CAN command structures for the throttle.
 */
struct throttle_Command {
    const uint8_t SET_VALUE[8]; // Command header for sending throttle value
};

// Declare the global instance of the command struct
extern struct throttle_Command throttle_comms;

/**
 * @brief Prepares and sends the throttle data over the CAN bus.
 *
 * This function takes the raw joystick data for the accelerator,
 * scales it, calculates a checksum, and sends it as a CAN frame.
 *
 * @param data The raw Sint16 value from the joystick's accelerator axis.
 */
void throttle_prepare(Sint16 data);

#endif // THROTTLE_H_
