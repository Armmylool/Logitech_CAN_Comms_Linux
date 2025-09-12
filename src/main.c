#include <stdio.h>
#include <stdbool.h>
#include "G29.h"

bool joystickReady = 0;
typedef struct {
    Sint16 steering;
    Sint16 accelerator;
    Sint16 brake;
} val ;

val G29_val = {0, 0, 0};
SDL_mutex* state_mutex = NULL;
SDL_bool app_is_running = SDL_TRUE ;
// Declare Functions.
int getData_G29(void* data)  ;

int main(int argc, char *argv[]) {
    SDL_Begin() ;
    joyStick_Begin() ;
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
}

int getData_G29(void* data) {
    SDL_Event event; 
    while (1) {
        while (SDL_PollEvent(&event)) {                                         // Initalize Joystick to control with button 23.
            if (event.type == SDL_JOYBUTTONDOWN){
                SDL_Log("Button %d pressed.", event.jbutton.button);
                if (event.jbutton.button == 23) {
                    joystickReady = 1 ;                                                                                                                                             // Enter pin on logitech to start a program
                }   
            }
        }
	if (joystickReady == 1) {
	    SDL_JoystickUpdate() ;
	    SDL_LockMutex(state_mutex);
	    G29_val.steering = SDL_JoystickGetAxis(joystick, 0); 
	    G29_val.accelerator = SDL_JoystickGetAxis(joystick, 2); 
	    G29_val.brake = SDL_JoystickGetAxis(joystick, 3);
            SDL_Log("Steering: %d, Accel: %d, Brake: %d",G29_val.steering, G29_val.accelerator, G29_val.brake);
	    SDL_UnlockMutex(state_mutex);
	    SDL_Delay(1) ;
	}
    }
}





