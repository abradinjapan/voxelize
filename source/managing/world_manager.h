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
typedef u64 MANAGER__slot_index;
typedef MANAGER__slot_index MANAGER__chunk_body_slot_index;
typedef MANAGER__slot_index MANAGER__chunk_XY_surface_slot_index;
typedef MANAGER__slot_index MANAGER__chunk_YZ_surface_slot_index;
typedef MANAGER__slot_index MANAGER__chunk_XZ_surface_slot_index;

// slot availability type
typedef enum MANAGER__sat {
    // statuses
    MANAGER__sat__unavailable = BASIC__bt__false,
    MANAGER__sat__available = BASIC__bt__true,

    // COUNT
    MANAGER__sat__COUNT = 2,
} MANAGER__sat;

// invalid index
#define MANAGER__define__invalid_index -1

/* Chunk Radius */
/*// chunk radius
typedef CHUNK__chunk_count MANAGER__chunk_radius;

// calculate the chunk radius
ESS__dimensions MANAGER__calculate__chunks_dimensions_from_chunks_radius(MANAGER__chunk_radius chunk_radius) {
    return ESS__create__dimensions((chunk_radius * 2) + 1, (chunk_radius * 2) + 1, (chunk_radius * 2) + 1);
}*/

/* Slot */
typedef struct MANAGER__slot {
    MANAGER__sat p_availability;
} MANAGER__slot;

// create custom slot
MANAGER__slot MANAGER__create__slot(MANAGER__sat slot_availability) {
    MANAGER__slot output;

    // setup output
    output.p_availability = slot_availability;

    return output;
}

// create null slot
MANAGER__slot MANAGER__create_null__slot() {
    // return empty
    return MANAGER__create__slot(MANAGER__sat__available);
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
    BASIC__buffer p_slots_allocation;

    // slots
    BASIC__buffer p_chunk_body_slots;
    BASIC__buffer p_chunk_XY_surface_slots;
    BASIC__buffer p_chunk_YZ_surface_slots;
    BASIC__buffer p_chunk_XZ_surface_slots;
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

    // open slots
    output.p_slots_allocation = BASIC__open__buffer(sizeof(MANAGER__slot) * (output.p_positioning.p_chunk_body_count + output.p_positioning.p_chunk_XY_surface_count + output.p_positioning.p_chunk_YZ_surface_count + output.p_positioning.p_chunk_XZ_surface_count));

    // setup sub-allocations
    output.p_chunk_body_slots = BASIC__create__buffer(output.p_slots_allocation.p_address, sizeof(MANAGER__slot) * output.p_positioning.p_chunk_body_count);
    output.p_chunk_XY_surface_slots = BASIC__create__buffer(output.p_slots_allocation.p_address + output.p_chunk_body_slots.p_length, sizeof(MANAGER__slot) * output.p_positioning.p_chunk_XY_surface_count);
    output.p_chunk_YZ_surface_slots = BASIC__create__buffer(output.p_slots_allocation.p_address + output.p_chunk_body_slots.p_length + output.p_chunk_XY_surface_slots.p_length, sizeof(MANAGER__slot) * output.p_positioning.p_chunk_YZ_surface_count);
    output.p_chunk_XZ_surface_slots = BASIC__create__buffer(output.p_slots_allocation.p_address + output.p_chunk_body_slots.p_length + output.p_chunk_XY_surface_slots.p_length + output.p_chunk_YZ_surface_slots.p_length, sizeof(MANAGER__slot) * output.p_positioning.p_chunk_XZ_surface_count);

    // setup all handles as available
    // setup chunk body slots
    for (MANAGER__chunk_body_slot_index index = 0; index < output.p_positioning.p_chunk_body_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_body_slots.p_address)[index] = MANAGER__create__slot(MANAGER__sat__available);
    }
    // setup chunk XY surface slots
    for (MANAGER__chunk_XY_surface_slot_index index = 0; index < output.p_positioning.p_chunk_XY_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_XY_surface_slots.p_address)[index] = MANAGER__create__slot(MANAGER__sat__available);
    }
    // setup chunk YZ surface slots
    for (MANAGER__chunk_YZ_surface_slot_index index = 0; index < output.p_positioning.p_chunk_YZ_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_YZ_surface_slots.p_address)[index] = MANAGER__create__slot(MANAGER__sat__available);
    }
    // setup chunk XZ surface slots
    for (MANAGER__chunk_XZ_surface_slot_index index = 0; index < output.p_positioning.p_chunk_XZ_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_XZ_surface_slots.p_address)[index] = MANAGER__create__slot(MANAGER__sat__available);
    }

    return output;
}

// create a null world manager
MANAGER__world_manager MANAGER__create_null__world_manager() {
    MANAGER__world_manager output;

    // setup output
    output.p_generation_algorithm = (GENERATION__function_address__generate_chunk)0;
    output.p_chunks = CHUNK__create_null__chunks();
    output.p_slots_allocation = BASIC__create_null__buffer();

    return output;
}

// close a world manager
void MANAGER__close__world_manager(MANAGER__world_manager world_manager) {
    // close tracked objects
    POS__close__positioning(world_manager.p_positioning);
    CHUNK__close__chunks(world_manager.p_chunks);
    RENDER__close__world(world_manager.p_rendered_world);

    // close buffers
    BASIC__close__buffer(world_manager.p_slots_allocation);

    return;
}

// initialize the world
void MANAGER__initialize__world(MANAGER__world_manager world_manager, ESS__world_vertex chunks_position, SKIN__skins skins, RENDER__temporaries temps) {
    CHUNK__chunk new_chunk;
    CHUNK__chunks_index center_chunks_index;
    CHUNK__chunks_index outside_chunks_index;
    CHUNK__chunks_index surface_index;

    // create chunks
    for (CHUNK__chunk_x x = 0; x < world_manager.p_positioning.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunk_y y = 0; y < world_manager.p_positioning.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunk_z z = 0; z < world_manager.p_positioning.p_chunk_body_dimensions.p_depth; z++) {
                // generate chunk data
                new_chunk = (*world_manager.p_generation_algorithm)(ESS__calculate__chunk_relative_world_position(chunks_position, ESS__create__dimensions(x, y, z)));
                center_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z);

                // set chunk
                CHUNK__set__chunk_in_chunks(world_manager.p_chunks, center_chunks_index, &new_chunk);

                // set chunk position
                ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_body_positions.p_address)[ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y, z);

                // render chunk
                RENDER__render__chunk_body(skins, CHUNK__get__chunk_pointer_in_chunks(world_manager.p_chunks, center_chunks_index), center_chunks_index, world_manager.p_rendered_world, temps);
            }
        }
    }

    // create chunk surfaces
    // initialize all chunk XY surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < world_manager.p_positioning.p_chunk_XY_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < world_manager.p_positioning.p_chunk_XY_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < world_manager.p_positioning.p_chunk_XY_surface_dimensions.p_depth; z++) {
                // calculate indices
                center_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z);
                outside_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z + 1);
                surface_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_XY_surface_dimensions, x, y, z);
                
                // initialize chunk position
                ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_XY_surface_positions.p_address)[surface_index] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y, z + 1);

                // render surface
                RENDER__render__chunk_XY_surface(skins, world_manager.p_chunks, surface_index, center_chunks_index, outside_chunks_index, world_manager.p_rendered_world, temps);
            }
        }
    }

    // initialize all chunk YZ surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < world_manager.p_positioning.p_chunk_YZ_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < world_manager.p_positioning.p_chunk_YZ_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < world_manager.p_positioning.p_chunk_YZ_surface_dimensions.p_depth; z++) {
                center_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z);
                outside_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x + 1, y, z);
                surface_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_YZ_surface_dimensions, x, y, z);

                // initialize chunk position
                ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_YZ_surface_positions.p_address)[surface_index] = POS__calculate__chunk_position_in_chunks(chunks_position, x + 1, y, z);

                // render surface
                RENDER__render__chunk_YZ_surface(skins, world_manager.p_chunks, surface_index, center_chunks_index, outside_chunks_index, world_manager.p_rendered_world, temps);
            }
        }
    }

    // initialize all chunk XZ surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < world_manager.p_positioning.p_chunk_XZ_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < world_manager.p_positioning.p_chunk_XZ_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < world_manager.p_positioning.p_chunk_XZ_surface_dimensions.p_depth; z++) {
                center_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z);
                outside_chunks_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y + 1, z);
                surface_index = ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_XZ_surface_dimensions, x, y, z);

                // initialize chunk position
                ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_XZ_surface_positions.p_address)[surface_index] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y + 1, z);

                // render surface
                RENDER__render__chunk_XZ_surface(skins, world_manager.p_chunks, surface_index, center_chunks_index, outside_chunks_index, world_manager.p_rendered_world, temps);
            }
        }
    }

    return;
}

// calculate chunks world box
ESS__world_box MANAGER__calculate__chunks_world_box(MANAGER__world_manager world_manager) {
    ESS__world_box output;
    ESS__world_vertex half_range;

    // calculate half range
    half_range = ESS__create__world_vertex((world_manager.p_positioning.p_chunk_body_dimensions.p_width * ESS__calculate__chunk_side_size_in_world_coordinates()) / 2, (world_manager.p_positioning.p_chunk_body_dimensions.p_height * ESS__calculate__chunk_side_size_in_world_coordinates()) / 2, (world_manager.p_positioning.p_chunk_body_dimensions.p_depth * ESS__calculate__chunk_side_size_in_world_coordinates()) / 2);

    // setup output
    output.p_right_up_front = ESS__calculate__add_world_vertices(world_manager.p_positioning.p_camera_position, half_range);
    output.p_left_down_back = ESS__calculate__subtract_world_vertices(world_manager.p_positioning.p_camera_position, half_range);

    return output;
}

// mark slots as free space that are no longer in camera range
void MANAGER__crop__slots(MANAGER__world_manager world_manager) {
    ESS__world_box valid_chunk_range;

    // create valid chunk range
    valid_chunk_range = MANAGER__calculate__chunks_world_box(world_manager);

    // crop the chunk body slots
    for (MANAGER__chunk_body_slot_index index = 0; index < world_manager.p_positioning.p_chunk_body_count; index++) {
        // check to see if the current slot is out of range
        if (ESS__calculate__box_is_in_box__inclusive(valid_chunk_range, ESS__calculate__chunk_box_from_vertex(((ESS__world_vertex*)world_manager.p_positioning.p_chunk_body_positions.p_address)[index])) == BASIC__bt__false) {
            // mark as out of range
            ((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[index].p_availability = MANAGER__sat__available;
        }
    }

    // crop the chunk XY surface handles
    // crop the chunk YZ surface handles
    // crop the chunk XZ surface handles

    return;
}

// update world
void MANAGER__update__world(MANAGER__world_manager world_manager, ESS__world_vertex camera_position) {
    ESS__world_vertex current_chunk;

    // calculate current chunk
    current_chunk = ESS__calculate__current_chunk_by_coordinates(camera_position);

    // DEBUG
    //printf("Current Chunk: [ %lu %lu %lu ]\n", current_chunk.p_x, current_chunk.p_y, current_chunk.p_z);

    // crop out chunks that are out of camera range
    // TODO

    // check to see if chunks are loaded
    

    return;
}

#endif
