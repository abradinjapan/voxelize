#ifndef VOXELIZE__positions
#define VOXELIZE__positions

/* Include */
#include "../base/essentials.h"
#include "chunks.h"

/* Define */


/* Position Manager */
// all game positions
typedef struct POS__positions {
    ESS__world_vertex p_camera_position;
    BASIC__buffer p_allocation;
    BASIC__buffer p_chunk_body_positions;
    BASIC__buffer p_chunk_XY_surface_positions;
    BASIC__buffer p_chunk_YZ_surface_positions;
    BASIC__buffer p_chunk_XZ_surface_positions;
} POS__positions;

// create custom positions
POS__positions POS__create__positions(ESS__world_vertex camera_position, BASIC__buffer allocation, BASIC__buffer chunk_body_positions, BASIC__buffer chunk_XY_surface_positions, BASIC__buffer chunk_YZ_surface_positions, BASIC__buffer chunk_XZ_surface_positions) {
    POS__positions output;

    // setup output
    output.p_camera_position = camera_position;
    output.p_allocation = allocation;
    output.p_chunk_body_positions = chunk_body_positions;
    output.p_chunk_XY_surface_positions = chunk_XY_surface_positions;
    output.p_chunk_YZ_surface_positions = chunk_YZ_surface_positions;
    output.p_chunk_XZ_surface_positions = chunk_XZ_surface_positions;

    return output;
}

// create null positions
POS__positions POS__create_null__positions() {
    // return null
    return POS__create__positions(ESS__create_null__world_vertex(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer());
}

// open positions
POS__positions POS__open__positions(ESS__world_vertex camera_position, CHUNK__chunks_x chunks_width, CHUNK__chunks_y chunks_height, CHUNK__chunks_z chunks_depth) {
    POS__positions output;

    // calculate dimensions

    // allocate positions

}

#endif
