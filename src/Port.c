#define _DEFAULT_SOURCE_

#include "Port.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h> 

// This global file descriptor will now be our socket
int fd = -1;

/**
 * @brief Initializes a raw CAN socket on a given interface (e.g., "can0").
 * @param interface_name The name of the CAN interface.
 * @return The socket file descriptor on success, -1 on failure.
 */
int socketcan_init(const char* interface_name) {
    struct sockaddr_can addr;
    struct ifreq ifr;

    // 1. Create a socket
    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // 2. Get the interface index by its name
    strcpy(ifr.ifr_name, interface_name);
    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl failed to get interface index");
        close(fd);
        return -1;
    }

    // 3. Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(fd);
        return -1;
    }

    printf("CAN socket initialized successfully on %s!\n", interface_name);
    return fd;
}

/**
 * @brief Writes a single CAN frame to the socket.
 * @param frame A pointer to the can_frame struct to be sent.
 * @return true on success, false on failure.
 */
bool CAN_Write(struct can_frame *frame) {
    if (fd < 0) {
        fprintf(stderr, "CAN socket not initialized.\n");
        return false;
    }

    // The write() system call is atomic for a single frame
    if (write(fd, frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("CAN write failed");
        return false;
    }

    return true;
}

/**
 * @brief Reads a single CAN frame from the socket (blocking).
 * @param frame A pointer to a can_frame struct where the received data will be stored.
 * @return The number of bytes read on success, -1 on failure.
 */
int CAN_Read(struct can_frame *frame) {
    if (fd < 0) {
        fprintf(stderr, "CAN socket not initialized.\n");
        return -1;
    }

    // The read() system call will block until a frame is received
    ssize_t nbytes = read(fd, frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        perror("CAN read failed");
        return -1;
    }

    if (nbytes < sizeof(struct can_frame)) {
        fprintf(stderr, "Incomplete CAN frame received.\n");
        return -1;
    }

    return nbytes;
}

/**
 * @brief Closes the CAN socket.
 */
void CAN_Close(void) {
    if (fd >= 0) {
        close(fd);
        fd = -1;
        printf("CAN socket closed.\n");
    }
}
