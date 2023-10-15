#ifndef VOXELIZE__graphics
#define VOXELIZE__graphics

/* Include */
// base
#include "../base/essentials.h"

// dependencies
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

/* Define */
typedef u16 WINDOW__window_dimension;

/* Window Configuration */
typedef struct WINDOW__window_configuration {
    BASIC__buffer p_title;
    WINDOW__window_dimension p_width;
    WINDOW__window_dimension p_height;
} WINDOW__window_configuration;

// setup a possible window configuration
WINDOW__window_configuration WINDOW__create__window_configuration(BASIC__buffer title_no_null_termination, WINDOW__window_dimension width, WINDOW__window_dimension height) {
    WINDOW__window_configuration output;

    // setup output
    output.p_title = title_no_null_termination;
    output.p_width = width;
    output.p_height = height;

    return output;
}

// create a null window configuration
WINDOW__window_configuration WINDOW__create_null__window_configuration() {
    return WINDOW__create__window_configuration(BASIC__create_null__buffer(), 0, 0);
}

// destroy a configuration (currently does nothing, but might in the future) (use for consistency)
void WINDOW__destroy__window_configuration(WINDOW__window_configuration window_configuration) {
    // quiet compiler warning
    window_configuration = window_configuration;

    return;
}

/* Error */
// error types
typedef enum WINDOW__et {
    // error codes
    WINDOW__et__no_error,
    WINDOW__et__sdl2_did_not_initialize,
    WINDOW__et__sdl2_did_not_open_opengl_window,
    WINDOW__et__sdl2_opengl_context_did_not_intialize,
    WINDOW__et__glew_did_not_initialize,
    WINDOW__et__LIMIT,

    // count
    WINDOW__et__COUNT = WINDOW__et__LIMIT,
} WINDOW__et;

/* Graphics */
typedef struct WINDOW__graphics {
    SDL_Window* p_window_context;
    SDL_GLContext p_sdl2_opengl_context;
    WINDOW__window_configuration p_window_configuration;
} WINDOW__graphics;

// create graphics
WINDOW__graphics WINDOW__create__graphics(SDL_Window* window_context, SDL_GLContext sdl2_opengl_context, WINDOW__window_configuration window_configuration) {
    WINDOW__graphics output;

    // setup output
    output.p_window_context = window_context;
    output.p_sdl2_opengl_context = sdl2_opengl_context;
    output.p_window_configuration = window_configuration;

    return output;
}

// create graphics as recognizably empty
WINDOW__graphics WINDOW__create_null__graphics() {
    // return null
    return WINDOW__create__graphics(0, 0, WINDOW__create_null__window_configuration());
}

// open a window using SDL2
WINDOW__graphics WINDOW__open__window(WINDOW__et* error, WINDOW__window_configuration window_configuration) {
    WINDOW__graphics output;

    // setup error to no error
    *error = WINDOW__et__no_error;

    // setup output
    output = WINDOW__create_null__graphics();

    // set configuration
    output.p_window_configuration = window_configuration;

    // initialize SDL2
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
        // set error
        *error = WINDOW__et__sdl2_did_not_initialize;

        return output;
    }

    // initalize window
    output.p_window_context = SDL_CreateWindow((const char*)window_configuration.p_title.p_address, 0, 0, window_configuration.p_width, window_configuration.p_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (output.p_window_context == 0) {
        // set error
        *error = WINDOW__et__sdl2_did_not_open_opengl_window;

        // quit cleanly
        SDL_Quit();

        return output;
    }

    // initalize opengl context
    output.p_sdl2_opengl_context = SDL_GL_CreateContext(output.p_window_context);
    if (output.p_sdl2_opengl_context == 0) {
        // set error
        *error = WINDOW__et__sdl2_opengl_context_did_not_intialize;

        // quit cleanly
        SDL_Quit();
        SDL_DestroyWindow(output.p_window_context);

        return output;
    }

    // initalize glew
    if (glewInit() != GLEW_OK) {
        // set error
        *error = WINDOW__et__glew_did_not_initialize;

        // quit cleanly
        SDL_Quit();
        SDL_DestroyWindow(output.p_window_context);
        SDL_GL_DeleteContext(output.p_sdl2_opengl_context);

        return output;
    }

    // window initialized
    return output;
}

// get new window size
void WINDOW__update__current_window_size(WINDOW__graphics* window) {
    s32 width = 0;
    s32 height = 0;

    // get window size from SDL2
    SDL_GetWindowSize((*window).p_window_context, &width, &height);

    // update window dimensions
    (*window).p_window_configuration.p_width = width;
    (*window).p_window_configuration.p_height = height;

    // update opengl on screen size change
    glViewport(0, 0, width, height);

    return;
}

// close window from SDL2
void WINDOW__close__window(WINDOW__graphics window) {
    // close things in order
    SDL_GL_DeleteContext(window.p_sdl2_opengl_context);
    SDL_DestroyWindow(window.p_window_context);
    SDL_Quit();

    return;
}

#endif
