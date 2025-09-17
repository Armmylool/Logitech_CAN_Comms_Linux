#include <stdio.h>
#include <stdbool.h>
#include "G29.h"
#include "Port.h"
#include "steering.h"

bool joystickReady = 0;
typedef struct {
    Sint16 steering;
    Sint16 accelerator;
    Sint16 brake;
} val;

val G29_val = {0, 0, 0};
SDL_mutex* state_mutex = NULL;
SDL_bool app_is_running = SDL_TRUE;

// Define the CAN interface to use
const char* can_interface = "can0";

int getData_G29(void* data);

int main(int argc, char *argv[]) {
    SDL_Begin();
    joyStick_Begin();

    // Initialize the SocketCAN interface
    if (socketcan_init(can_interface) < 0) {
        fprintf(stderr, "ERROR: Cannot initialize CAN socket on %s\n", can_interface);
        return 1;
    }

    enableWheel();

    state_mutex = SDL_CreateMutex();
    SDL_Log("Main: Starting worker threads...");
    SDL_Thread* producer_thread = SDL_CreateThread(getData_G29, "G29_Publish", NULL);

    while (app_is_running) {
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app_is_running = SDL_FALSE;
            }
        }
    }

    // Clean up
    SDL_WaitThread(producer_thread, NULL);
    SDL_DestroyMutex(state_mutex);
    disableWheel();
    CAN_Close();
    SDL_Quit();
    printf("Application shut down cleanly.\n");
    return 0;
}

int getData_G29(void* data) {
    while (app_is_running) { // Use the main loop's flag
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_JOYBUTTONDOWN) {
                SDL_Log("Button %d pressed.", event.jbutton.button);
                if (event.jbutton.button == 23) {
                    joystickReady = 1;
                }
            }
        }

        if (joystickReady) {
            SDL_JoystickUpdate();
            SDL_LockMutex(state_mutex);
            G29_val.steering = SDL_JoystickGetAxis(joystick, 0);
            G29_val.accelerator = SDL_JoystickGetAxis(joystick, 2);
            G29_val.brake = SDL_JoystickGetAxis(joystick, 3);
            SDL_Log("Steering Data : %d", G29_val.steering) ;
            // Send steering data over CAN
            receiveDataFromG29(G29_val.steering);

            SDL_UnlockMutex(state_mutex);
            SDL_Delay(1); // 10ms delay gives a 100Hz update rate
        } 
    }
    return 0;
}
