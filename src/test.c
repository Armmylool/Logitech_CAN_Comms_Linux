#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include "G29.h"
#include <stdbool.h>

// === Serial Port Configuration ===
int open_serial_port(const char* port, int baud_rate) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open_serial_port: Unable to open serial port");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    // Set baud rate
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit
    tty.c_iflag &= ~IGNBRK;                         // disable break processing
    tty.c_lflag = 0;                                // no signaling chars, no echo
    tty.c_oflag = 0;                                // no output processing
    tty.c_cc[VMIN]  = 0;                            // non-blocking read
    tty.c_cc[VTIME] = 5;                            // 0.5s timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // no xon/xoff
    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);              // clear parity
    tty.c_cflag |= PARENB;                          // enable EVEN parity
    tty.c_cflag &= ~CSTOPB;                         // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // no HW flow control

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

// === Checksum: sum of bytes (mod 256) ===
uint8_t calculate_checksum_2(uint8_t* data, size_t len) {
    uint16_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return (uint8_t)(sum & 0xFF);
}

// === Send Absolute Position Command (0x13) ===
void send_sanmotion_position(int serial_fd, uint8_t address, uint32_t position) {
    // Packet: [Len][Addr][Cmd][Data(4)][Checksum]
    uint8_t packet[8];
    packet[0] = 8;                      // Total length
    packet[1] = address;                // e.g., 0x00
    packet[2] = 0x13;                   // Absolute Position Command

    // Position: 32-bit, LSB first
    packet[3] = (position >> 0)  & 0xFF;
    packet[4] = (position >> 8)  & 0xFF;
    packet[5] = 0x00;
    packet[6] = 0x00;

    // Checksum over first 7 bytes
    packet[7] = calculate_checksum_2(packet, 7);

    // Send packet
    ssize_t written = write(serial_fd, packet, 8);
    if (written != 8) {
        fprintf(stderr, "Failed to send full packet\n");
    }

    // Optional: log sent packet
    printf("Sent: ");
    for (int i = 0; i < 8; i++) printf("%02X ", packet[i]);
    printf("\n");
}

// === Main ===
int main(int argc, char *argv[]) {
    // === 1. Initialize SDL and G29 ===
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_NumJoysticks() < 1) {
        fprintf(stderr, "No joystick found.\n");
        SDL_Quit();
        return -1;
    }

    SDL_Joystick* joystick = SDL_JoystickOpen(0);
    if (!joystick) {
        fprintf(stderr, "Failed to open G29.\n");
        SDL_Quit();
        return -1;
    }

    printf("G29 opened. Press any button to start sending commands.\n");

    // === 2. Open USB-to-RS485 Port ===
    const char* serial_port = "/dev/ttyUSB0";  // Change if needed (e.g., /dev/ttyACM0)
    int baud = B115200;                        // Default SANMOTION baud rate

    int serial_fd = open_serial_port(serial_port, baud);
    if (serial_fd < 0) {
        SDL_JoystickClose(joystick);
        SDL_Quit();
        return -1;
    }

    // === 3. Main Loop ===
    bool running = true;
    bool enabled = false;
    const uint8_t motor_address = 0x01;  // Must match DIP switch on motor

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_JOYBUTTONDOWN) {
                printf("Button %d pressed. Sending commands enabled.\n", event.jbutton.button);
                enabled = true;
            }
        }

        if (enabled) {
            SDL_JoystickUpdate();
            Sint16 brake = SDL_JoystickGetAxis(joystick, 3); // Axis 2 = accelerator

            // Map [-32768, 32767] → [0, 800]
            uint16_t calibrated = 800 - (((int32_t)brake + 32768) * 800 / 65535);
            uint32_t position = calibrated;

            // Send to SANMOTION via USB-RS485
            send_sanmotion_position(serial_fd, motor_address, position);
        }

        SDL_Delay(10); // ~100 Hz
    }

    // === 4. Cleanup ===
    close(serial_fd);
    SDL_JoystickClose(joystick);
    SDL_Quit();
    printf("Application shut down cleanly.\n");
    return 0;
}
