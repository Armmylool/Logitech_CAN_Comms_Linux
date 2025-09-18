#include <stdio.h>
#include <stdbool.h>
#include "G29.h"
#include "Port.h"
#include "steering.h"

const char* can_interface = "can0" ;

int main() {
    // Initialize the SocketCAN interface
    if (socketcan_init(can_interface) < 0) {
        fprintf(stderr, "ERROR: Cannot initialize CAN socket on %s\n", can_interface);
        return 1;
    }

    enableWheel() ;     // Enable the steering wheel.
    
}
