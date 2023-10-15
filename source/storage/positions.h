#ifndef VOXELIZE__world
#define VOXELIZE__world

/* Include */
#include "../base/essentials.h"
#include "chunks.h"

/* World Simulation */
// the world (global) positions of objects
typedef struct POS__world {
    // simulation data
    ESS__world_box p_simulation_space;
    ESS__world_vertex p_main_player_position; // the position of the main player
    BASIC__buffer p_chunk_positions; // the world positions of each active chunk
} POS__world;

// create custom world
POS__world POS__create__world(ESS__world_box simulation_space, ESS__world_vertex main_player_position, BASIC__buffer chunk_world_positions) {
    POS__world output;

    // setup output
    output.p_simulation_space = simulation_space;
    output.p_main_player_position = main_player_position;
    output.p_chunk_positions = chunk_world_positions;

    return output;
}

// open world
POS__world POS__open__world(ESS__world_vertex main_player_position, ESS__world_vertex simulation_axis_lengths) {
    POS__world output;
    ESS__world_vertex anti_simulation_axis_lengths;

    // setup main player world position
    output.p_main_player_position = main_player_position;

    // setup simulation space based on main player position
    output.p_simulation_space.p_right_up_front = simulation_axis_lengths;

    // create chunks based on simulation space size

    return output;
}

#endif
