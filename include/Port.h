#ifndef PORT_H_
#define PORT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/can.h> // Required for can_frame

// The global file descriptor for the socket
extern int fd;

/**
 * @brief Initializes the CAN socket.
 * @param interface_name The name of the interface, e.g., "can0".
 * @return The socket descriptor if successful, -1 if it fails.
 */
int socketcan_init(const char* interface_name);

/**
 * @brief Writes a CAN frame to the bus.
 * @param frame Pointer to the can_frame to be sent.
 * @return true if successful, false otherwise.
 */
bool CAN_Write(struct can_frame *frame);

/**
 * @brief Closes the CAN socket.
 */
void CAN_Close(void);

#endif // PORT_H_
