#include "Throttle.h"
#include "Port.h"
#include <string.h>

#define THROTTLE_CAN_ID 0x06000002 | CAN_EFF_FLAG

struct throttle_Command throttle_comms = {
    .SET_VALUE = {0x23, 0x03, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00}
};


void throttle_prepare(Sint16 data) {
    struct can_frame frame;
    frame.can_id = THROTTLE_CAN_ID;
    frame.can_dlc = 8;

    int32_t raw_data = data;
    int16_t scaled_value = (32767 - raw_data) / 16;
    if (scaled_value < 0) scaled_value = 0;
    if (scaled_value > 4095) scaled_value = 4095;

    memcpy(frame.data, throttle_comms.SET_VALUE, 4);

    frame.data[4] = (scaled_value >> 8) & 0xFF; // High byte
    frame.data[5] = scaled_value & 0xFF;       // Low byte

    uint16_t checksum = 0;
    for (int i = 0; i < 6; i++) {
        checksum += frame.data[i];
    }

    frame.data[6] = (checksum >> 8) & 0xFF; // High byte ของ checksum
    frame.data[7] = checksum & 0xFF;       // Low byte ของ checksum

    CAN_Write(&frame);
}
