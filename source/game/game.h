#ifndef VOXELIZE__game
#define VOXELIZE__game

/* Include */
// game internals
#include "../rendering/game_window.h"
#include "../rendering/textures.h"
#include "../rendering/shaders.h"
#include "../user_control/controls.h"
#include "../world/chunks.h"
#include "../world/positioning.h"
#include "../rendering/render.h"

/* Game Information */
// game state information
typedef enum GAME__gsi {
    GAME__gsi__game_paused,
    GAME__gsi__game_running,
    GAME__gsi__game_quitting,
    GAME__gsi__LENGTH,
} GAME__gsi;

// game container
typedef struct GAME__information {
    // running state information
    GAME__gsi p_game_state;

    // error information
    BASIC__bt p_error_occured;
    WINDOW__et p_graphics_error;
    SHADER__et p_shader_error;
    SHADER__error_log p_shader_error_log;

    // graphical information
    WINDOW__graphics p_graphics;
    SHADER__program p_chunks_shader_program;
    TEX__game_textures p_game_textures;
    SKIN__skins p_skins;
    RENDER__world p_world_rendering;

    // player input information
    CONTROLS__controls p_controls;

    // game simulation information
    POS__positioning p_positioning;
    CHUNK__chunks p_chunks;
    RENDER__vertex p_camera_rotation;

    // game wide random contexts
    RANDOM__context p_random_pixel_context;

    // game wide temporaries
    RENDER__temporaries p_temporaries;
} GAME__information;

// setup game information (does not actually start a game, information should be setup beforehand)
GAME__information GAME__create__game_information(WINDOW__graphics graphics, SHADER__program chunks_shader_program, TEX__game_textures game_textures) {
    GAME__information output;

    // setup output
    output.p_game_state = GAME__gsi__game_running;
    output.p_error_occured = BASIC__bt__false;
    output.p_graphics_error = WINDOW__et__no_error;
    output.p_shader_error = SHADER__et__no_error;
    output.p_shader_error_log = SHADER__create_null__error_log();
    output.p_graphics = graphics;
    output.p_chunks_shader_program = chunks_shader_program;
    output.p_game_textures = game_textures;
    output.p_controls = CONTROLS__create_null__controls();
    output.p_chunks = CHUNK__create_null__chunks();
    output.p_random_pixel_context = RANDOM__create_null__context();

    return output;
}

// create null game information
GAME__information GAME__create_null__game_information() {
    // return null game information
    return GAME__create__game_information(WINDOW__create_null__graphics(), SHADER__create_null__shader_program(), TEX__create_null__game_textures());
}

#endif
