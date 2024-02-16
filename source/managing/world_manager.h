#ifndef VOXELIZE__world_manager
#define VOXELIZE__world_manager

/* Notes */
/*
    The world must always be rendered with the check of "is this chunk drawn?"
*/

/* Include */
// voxelize
#include "../rendering/render.h"
#include "../world/chunks.h"
#include "../world/chunk_surfaces.h"
#include "../world/positioning.h"
#include "../world/generation.h"

/* Define */
// indices
typedef CHUNK__chunks_index MANAGER__chunks_index;
typedef RENDER__object_index MANAGER__rendering_object_index;

// invalid index
#define MANAGER__define__invalid_index -1

/* Slots */
// one slot of a chunk (body & array)
typedef struct MANAGER__chunk_body_slot {
    // open spot flag
    BASIC__bt p_slot_open;

    // chunk array & chunk body indices
    MANAGER__chunks_index p_chunks_index;
    MANAGER__rendering_object_index p_rendering_index;
} MANAGER__chunk_body_slot;

// create custom body slot
MANAGER__chunk_body_slot MANAGER__create__chunk_body_slot(BASIC__bt slot_open, MANAGER__chunks_index chunks_index, MANAGER__rendering_object_index rendering_index) {
    MANAGER__chunk_body_slot output;

    // setup output
    output.p_slot_open = slot_open;
    output.p_chunks_index = chunks_index;
    output.p_rendering_index = rendering_index;

    return output;
}

// create bull body slot
MANAGER__chunk_body_slot MANAGER__create_null__chunk_body_slot() {
    // return empty
    return MANAGER__create__chunk_body_slot(BASIC__bt__false, MANAGER__define__invalid_index, MANAGER__define__invalid_index);
}

// one slot of a chunk (surface)
typedef struct MANAGER__chunk_surface_slot {
    // open spot flag
    BASIC__bt p_slot_open;

    // chunk array & chunk body indices
    MANAGER__rendering_object_index p_rendering_index;
} MANAGER__chunk_surface_slot;

// create custom surface slot
MANAGER__chunk_surface_slot MANAGER__create__chunk_surface_slot(BASIC__bt slot_open, MANAGER__rendering_object_index rendering_index) {
    MANAGER__chunk_surface_slot output;

    // setup output
    output.p_slot_open = slot_open;
    output.p_rendering_index = rendering_index;

    return output;
}

// create null surface slot
MANAGER__chunk_surface_slot MANAGER__create_null__chunk_surface_slot() {
    // return empty
    return MANAGER__create__chunk_surface_slot(BASIC__bt__false, MANAGER__define__invalid_index);
}

/* Manager */
// manager type
typedef struct MANAGER__world_manager {
    // generation algorithm
    GENERATION__function_address__generate_chunk p_generation_algorithm;

    // tracked objects
    RENDER__world p_rendered_world;
    CHUNK__chunks p_chunks;
    POS__positioning p_positioning;

    // slots allocation
    BASIC__buffer p_allocation;

    // slots
    BASIC__buffer p_chunk_body_slots;
    BASIC__buffer p_chunk_surface_slots;
} MANAGER__world_manager;

// open a world manager
MANAGER__world_manager MANAGER__open__world_manager(GENERATION__function_address__generate_chunk generation_algorithm, ESS__dimensions chunks_dimensions, ESS__world_vertex chunks_initial_position, ESS__world_vertex camera_position) {
    MANAGER__world_manager output;
    CHUNK__chunk blank_chunk = CHUNK__create_null__chunk();

    // setup generation algorithm
    output.p_generation_algorithm = generation_algorithm;

    // open positions
    output.p_positioning = POS__open__positioning(camera_position, chunks_initial_position, chunks_dimensions);

    // open renderer
    output.p_rendered_world = RENDER__open__world(output.p_positioning);

    // open chunks
    output.p_chunks = CHUNK__open__chunks(output.p_positioning.p_chunk_body_count, &blank_chunk);

    return output;
}

// create a null world manager
MANAGER__world_manager MANAGER__create_null__world_manager() {
    MANAGER__world_manager output;

    // setup output
    output.p_generation_algorithm = (GENERATION__function_address__generate_chunk)0;
    output.p_chunks = CHUNK__create_null__chunks();
    output.p_allocation = BASIC__create_null__buffer();

    return output;
}

// close a world manager
void MANAGER__close__world_manager(MANAGER__world_manager world_manager) {
    // close tracked objects
    POS__close__positioning(world_manager.p_positioning);
    CHUNK__close__chunks(world_manager.p_chunks);
    RENDER__close__world(world_manager.p_rendered_world);

    // close buffers
    BASIC__close__buffer(world_manager.p_allocation);

    return;
}

// generate all chunks
void MANAGER__build__world(MANAGER__world_manager world_manager, ESS__world_vertex chunks_position) {
    // create chunks
    for (CHUNK__chunk_x x = 0; x < world_manager.p_positioning.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunk_y y = 0; y < world_manager.p_positioning.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunk_z z = 0; z < world_manager.p_positioning.p_chunk_body_dimensions.p_depth; z++) {
                // generate chunk data
                CHUNK__chunk new_chunk = (*world_manager.p_generation_algorithm)(ESS__calculate__chunk_relative_world_position(chunks_position, ESS__create__dimensions(x, y, z)));

                // set chunk
                CHUNK__set__chunk_in_chunks(world_manager.p_chunks, ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z), &new_chunk);
            }
        }
    }

    return;
}

#endif
