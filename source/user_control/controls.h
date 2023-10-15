#ifndef VOXELIZE__controls
#define VOXELIZE__controls

/* Include */
// basic
#include "../base/essentials.h"

// SDL2
#include <SDL2/SDL.h>

/* Define */
typedef const Uint8 CONTROLS__sdl2_keyboard_state;
typedef CONTROLS__sdl2_keyboard_state* CONTROLS__sdl2_keyboard_state_address;
typedef s32 CONTROLS__mouse_axis;
typedef CONTROLS__mouse_axis CONTROLS__mouse_change_x;
typedef CONTROLS__mouse_axis CONTROLS__mouse_change_y;

/* Controls */
// key pressed type
typedef enum CONTROLS__kpt {
    CONTROLS__kpt__not_pressed,
    CONTROLS__kpt__pressed,
    CONTROLS__kpt__COUNT,
} CONTROLS__kpt;

// mouse position change
typedef struct CONTROLS__mouse_position_change {
    CONTROLS__mouse_change_x p_mouse_change_x;
    CONTROLS__mouse_change_y p_mouse_change_y;
} CONTROLS__mouse_position_change;

// change mouse position
CONTROLS__mouse_position_change CONTROLS__create__mouse_position_change(CONTROLS__mouse_change_x x, CONTROLS__mouse_change_y y) {
    CONTROLS__mouse_position_change output;

    // setup output
    output.p_mouse_change_x = x;
    output.p_mouse_change_y = y;

    return output;
}

// controls
typedef struct CONTROLS__controls {
    CONTROLS__sdl2_keyboard_state_address p_keyboard_state_address;
    CONTROLS__mouse_position_change p_mouse_position_change;
} CONTROLS__controls;

// create custom controls
CONTROLS__controls CONTROLS__create__controls(CONTROLS__sdl2_keyboard_state_address sdl2_keyboard_state_address) {
    CONTROLS__controls output;

    // setup output
    output.p_keyboard_state_address = sdl2_keyboard_state_address;
    output.p_mouse_position_change = CONTROLS__create__mouse_position_change(0.0f, 0.0f);

    return output;
}

// create null controls
CONTROLS__controls CONTROLS__create_null__controls() {
    return CONTROLS__create__controls(0);
}

// setup controls
CONTROLS__controls CONTROLS__open__controls() {
    return CONTROLS__create__controls(SDL_GetKeyboardState((void*)0));
}

// change whether or not the mouse is fixed to the middle of the game window
void CONTROLS__update__mouse_lock(BASIC__bt mouse_locked) {
    if (mouse_locked == BASIC__bt__true) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    return;
}

// close controls
void CONTROLS__close__controls(CONTROLS__controls controls) {
    // quiet compiler message
    controls = controls;

    // make sure mouse is released
    CONTROLS__update__mouse_lock(BASIC__bt__false);

    return;
}

// get key pressed
CONTROLS__kpt CONTROLS__check__key_pressed(CONTROLS__controls controls, CONTROLS__sdl2_keyboard_state key) {
    return (CONTROLS__kpt)(BASIC__bt)(controls.p_keyboard_state_address[key]);
}

// get mouse position change
void CONTROLS__update__mouse_position_change(CONTROLS__controls* controls) {
    CONTROLS__mouse_position_change mouse_position_change;

    // setup output
    SDL_GetRelativeMouseState(&(mouse_position_change.p_mouse_change_x), &(mouse_position_change.p_mouse_change_y));

    // change position
    (*controls).p_mouse_position_change.p_mouse_change_x = mouse_position_change.p_mouse_change_x;
    (*controls).p_mouse_position_change.p_mouse_change_y = mouse_position_change.p_mouse_change_y;

    return;
}

#endif
