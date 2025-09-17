#include <G29.h>


SDL_Joystick *joystick = NULL;

void SDL_Begin() {
    printf("SDL2 Initialize!\n") ;
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0){
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    printf("Can not init Joystick\n") ;
    }
}

void  joyStick_Begin() {
    int joystickCount = SDL_NumJoysticks();
    if (joystickCount <= 0) {
        printf("No joysticks connected.\n");
        SDL_Quit() ;
        exit(EXIT_FAILURE) ;
    }  
    printf("Found %d joystick(s):\n", joystickCount);

    for (int i = 0; i < joystickCount; i++) {
        joystick = SDL_JoystickOpen(i);

        if (joystick != NULL) {
            printf("Successfully opened joystick at index %d: %s\n", i, SDL_JoystickName(joystick));
            break; 
        } else {
            fprintf(stderr, "Failed to open joystick at index %d: %s\n", i, SDL_GetError());
        }
    }
}
