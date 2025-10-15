#include <stdio.h>
#include <stdbool.h>
#include "G29.h"
#include "Port.h"
#include "steering.h"
#include "Throttle.h"

typedef struct {
    Sint16 steering;
    Sint16 accelerator;
    Sint16 brake;
} val;

SDL_bool joystickReady = SDL_FALSE;
val G29_val = {0, 0, 0};
SDL_mutex* state_mutex = NULL;
SDL_cond* data_cond = NULL;
SDL_bool app_is_running = SDL_TRUE;
SDL_bool data_ready = SDL_FALSE;

// Define the CAN interface to use
const char* can_interface = "can0";

int getData_G29(void* data);
int send_can_frames(void* data);

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
    data_cond = SDL_CreateCond();

    SDL_Thread* producer_thread = NULL;
    SDL_Thread* can_thread = NULL;

    if (!state_mutex || !data_cond) {
        SDL_Log("Failed to create synchronization primitives: %s", SDL_GetError());
        app_is_running = SDL_FALSE;
    } else {
        SDL_Log("Main: Starting worker threads...\n");
        producer_thread = SDL_CreateThread(getData_G29, "G29_Publish", NULL);
        can_thread = SDL_CreateThread(send_can_frames, "CAN_Send", NULL);

        while (app_is_running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_LockMutex(state_mutex);
                    app_is_running = SDL_FALSE;
                    SDL_CondBroadcast(data_cond);
                    SDL_UnlockMutex(state_mutex);
                } else if (event.type == SDL_JOYBUTTONDOWN) {
                    if (event.jbutton.button == 23) {
                        SDL_LockMutex(state_mutex);
                        joystickReady = SDL_TRUE;
                        SDL_UnlockMutex(state_mutex);
                        SDL_Log("Joystick ready for data capture.\n");
                    }
                }
            }
            SDL_Delay(10);
        }
    }

    if (state_mutex) {
        SDL_LockMutex(state_mutex);
        app_is_running = SDL_FALSE;
        if (data_cond) {
            SDL_CondBroadcast(data_cond);
        }
        SDL_UnlockMutex(state_mutex);
    }

    if (producer_thread) {
        SDL_WaitThread(producer_thread, NULL);
    }
    if (can_thread) {
        SDL_WaitThread(can_thread, NULL);
    }
    if (data_cond) {
        SDL_DestroyCond(data_cond);
    }
    if (state_mutex) {
        SDL_DestroyMutex(state_mutex);
    }

    disableWheel();
    CAN_Close();
    SDL_Quit();
    printf("Application shut down cleanly.\n");
    return 0;
}

int getData_G29(void* data) {
    while (1) {
        SDL_LockMutex(state_mutex);
        SDL_bool running = app_is_running;
        SDL_bool ready = joystickReady;
        SDL_UnlockMutex(state_mutex);

        if (!running) {
            break;
        }

        if (!ready) {
            SDL_Delay(10);
            continue;
        }

        SDL_JoystickUpdate();

        val latest = {0};
        latest.steering = SDL_JoystickGetAxis(joystick, 0);
        latest.accelerator = SDL_JoystickGetAxis(joystick, 2);
        latest.brake = SDL_JoystickGetAxis(joystick, 3);

        SDL_LockMutex(state_mutex);
        G29_val = latest;
        data_ready = SDL_TRUE;
        SDL_CondSignal(data_cond);
        SDL_UnlockMutex(state_mutex);

        SDL_Delay(1);
    }
    return 0;
}

int send_can_frames(void* data) {
    while (1) {
        SDL_LockMutex(state_mutex);
        while (app_is_running && !data_ready) {
            SDL_CondWait(data_cond, state_mutex);
        }

        if (!app_is_running) {
            SDL_UnlockMutex(state_mutex);
            break;
        }

        val pending = G29_val;
        data_ready = SDL_FALSE;
        SDL_UnlockMutex(state_mutex);

        SDL_Log("Accelerator Data : %d", pending.accelerator);
        throttle_prepare(pending.accelerator);
        // receiveDataFromG29(pending.steering);
    }
    return 0;
}
