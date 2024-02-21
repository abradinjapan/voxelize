#ifndef VOXELIZE__world_manager
#define VOXELIZE__world_manager

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
MANAGER__world_manager MANAGER__open__world_manager(GENERATION__function_address__generate_chunk generation_algorithm, ESS__dimensions chunks_dimensions, ESS__world_vertex camera_position) {
    MANAGER__world_manager output;
    CHUNK__chunk blank_chunk = CHUNK__create_null__chunk();

    // setup generation algorithm
    output.p_generation_algorithm = generation_algorithm;

    // open positions
    output.p_positioning = POS__open__positioning(camera_position, chunks_dimensions);

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
        ((MANAGER__slot*)output.p_chunk_body_slots.p_address)[index] = MANAGER__create_null__slot();
    }
    // setup chunk XY surface slots
    for (MANAGER__chunk_XY_surface_slot_index index = 0; index < output.p_positioning.p_chunk_XY_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_XY_surface_slots.p_address)[index] = MANAGER__create_null__slot();
    }
    // setup chunk YZ surface slots
    for (MANAGER__chunk_YZ_surface_slot_index index = 0; index < output.p_positioning.p_chunk_YZ_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_YZ_surface_slots.p_address)[index] = MANAGER__create_null__slot();
    }
    // setup chunk XZ surface slots
    for (MANAGER__chunk_XZ_surface_slot_index index = 0; index < output.p_positioning.p_chunk_XZ_surface_count; index++) {
        // initialize one slot
        ((MANAGER__slot*)output.p_chunk_XZ_surface_slots.p_address)[index] = MANAGER__create_null__slot();
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

// load chunk body
void MANAGER__load__chunk_body(MANAGER__world_manager world_manager, MANAGER__chunk_body_slot_index chunk_slot_index, ESS__world_vertex chunk_position, SKIN__skins skins, RENDER__temporaries temps) {
    CHUNK__chunk new_chunk;

    // generate chunk data
    new_chunk = (*world_manager.p_generation_algorithm)(chunk_position);

    // write chunk data
    CHUNK__set__chunk_in_chunks(world_manager.p_chunks, chunk_slot_index, &new_chunk);

    // set chunk position
    ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_body_positions.p_address)[chunk_slot_index] = chunk_position;

    // render chunk
    RENDER__render__chunk_body(skins, CHUNK__get__chunk_pointer_in_chunks(world_manager.p_chunks, chunk_slot_index), chunk_slot_index, world_manager.p_rendered_world, temps);

    // mark slot as taken
    ((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[chunk_slot_index].p_availability = MANAGER__sat__unavailable;

    return;
}

// initialize the world
void MANAGER__initialize__world(MANAGER__world_manager world_manager, ESS__world_vertex chunks_position, SKIN__skins skins, RENDER__temporaries temps) {
    CHUNK__chunks_index center_chunks_index; // also just the chunk body index
    CHUNK__chunks_index outside_chunks_index;
    CHUNK__chunks_index surface_index;

    // create chunk bodies
    for (CHUNK__chunk_x x = 0; x < world_manager.p_positioning.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunk_y y = 0; y < world_manager.p_positioning.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunk_z z = 0; z < world_manager.p_positioning.p_chunk_body_dimensions.p_depth; z++) {
                // load chunk
                MANAGER__load__chunk_body(world_manager, ESS__calculate__dimensions_index(world_manager.p_positioning.p_chunk_body_dimensions, x, y, z), ESS__calculate__chunk_relative_world_position(chunks_position, ESS__create__dimensions(x, y, z)), skins, temps);
            }
        }
    }

    // create chunk surfaces
    // initialize all chunk XY surfaces
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

    // initialize all chunk YZ surfaces
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

    // initialize all chunk XZ surfaces
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

// unload the out of range chunks & surfaces
void MANAGER__crop__slots(MANAGER__world_manager world_manager, ESS__world_box valid_chunk_body_box) {
    MANAGER__chunk_body_slot_index chunk_body_slots_freed = 0;

    // mark open slots as available that are out of bounds
    for (MANAGER__chunk_body_slot_index slot_index = 0; slot_index < world_manager.p_positioning.p_chunk_body_count; slot_index++) {
        // check if slot is out of bounds
        if (ESS__calculate__box_is_in_box__inclusive(valid_chunk_body_box, ESS__calculate__chunk_box_from_vertex(((ESS__world_vertex*)world_manager.p_positioning.p_chunk_body_positions.p_address)[slot_index])) == BASIC__bt__false) {
            // free slot
            ((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[slot_index].p_availability = MANAGER__sat__available;

            // DEBUG
            chunk_body_slots_freed++;
        }
    }

    // DEBUG
    //printf("Slots freed: #%lu\n", chunk_body_slots_freed);

    return;
}

// calculate valid chunk body box
ESS__world_box MANAGER__calculate__valid_chunk_body_box(ESS__world_vertex current_chunk_coord, ESS__dimensions chunks_dimensions) {
    ESS__world_box output;
    ESS__world_vertex left_down_back_chunk_count;
    ESS__world_vertex right_up_front_chunk_count;
    ESS__world_axis chunk_size = ESS__calculate__chunk_side_size_in_world_coordinates();

    // calculate left down back
    left_down_back_chunk_count.p_x = chunks_dimensions.p_width / 2;
    left_down_back_chunk_count.p_y = chunks_dimensions.p_height / 2;
    left_down_back_chunk_count.p_z = chunks_dimensions.p_depth / 2;

    // calculate right up front (account for even chunk count)
    right_up_front_chunk_count.p_x = (chunks_dimensions.p_width / 2) + ((chunks_dimensions.p_width) % 2);
    right_up_front_chunk_count.p_y = (chunks_dimensions.p_height / 2) + ((chunks_dimensions.p_height) % 2);
    right_up_front_chunk_count.p_z = (chunks_dimensions.p_depth / 2) + ((chunks_dimensions.p_depth) % 2);

    // calculate valid chunk box
    output.p_left_down_back = ESS__calculate__subtract_world_vertices(current_chunk_coord, ESS__create__world_vertex(left_down_back_chunk_count.p_x * chunk_size, left_down_back_chunk_count.p_y * chunk_size, left_down_back_chunk_count.p_z * chunk_size));
    output.p_right_up_front = ESS__calculate__add_world_vertices(current_chunk_coord, ESS__create__world_vertex(right_up_front_chunk_count.p_x * chunk_size, right_up_front_chunk_count.p_y * chunk_size, right_up_front_chunk_count.p_z * chunk_size));

    return output;
}

// find an open chunk slot
MANAGER__chunk_body_slot_index MANAGER__find__available_chunk_body_slot_index(MANAGER__world_manager world_manager) {
    // search for slot index
    for (MANAGER__chunk_body_slot_index index = 0; index < world_manager.p_positioning.p_chunk_body_count; index++) {
        // if slot is avaliable
        if (((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[index].p_availability == MANAGER__sat__available) {
            // return index
            return index;
        }
    }

    // should not be possible, but handled anyways
    return world_manager.p_positioning.p_chunk_body_count;
}

// find if the chunk is already open
MANAGER__chunk_body_slot_index MANAGER__find__unavaliable_chunk_body_slot_index(MANAGER__world_manager world_manager, ESS__world_vertex chunk_position) {
    MANAGER__chunk_body_slot_index output = 0;

    // search for the slot
    while (output < world_manager.p_positioning.p_chunk_body_count) {
        // check for slot
        if (((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[output].p_availability == MANAGER__sat__unavailable && ESS__calculate__world_vertices_are_equal(chunk_position, ((ESS__world_vertex*)world_manager.p_positioning.p_chunk_body_positions.p_address)[output])) {
            // valid slot
            return output;
        }

        // next slot
        output++;
    }

    // slot not found
    return output;
}

ESS__world_vertex MANAGER__calculate__chunk_position_by_offset(ESS__world_box valid_chunk_body_box, CHUNK__chunks_x x, CHUNK__chunks_y y, CHUNK__chunks_z z) {
    ESS__world_vertex output;
    ESS__world_axis chunk_size = ESS__calculate__chunk_side_size_in_world_coordinates();

    // setup output
    output.p_x = valid_chunk_body_box.p_right_up_front.p_x - (x * chunk_size);
    output.p_y = valid_chunk_body_box.p_right_up_front.p_y - (y * chunk_size);
    output.p_z = valid_chunk_body_box.p_right_up_front.p_z - (z * chunk_size);

    return output;
}

// load new chunks
void MANAGER__load__new_chunks(MANAGER__world_manager world_manager, ESS__world_box valid_chunk_body_box, SKIN__skins skins, RENDER__temporaries temps) {
    MANAGER__chunk_body_slot_index chunks_loaded = 0;

    // check for loaded chunks and load unloaded chunks
    for (CHUNK__chunks_x x = 0; x < world_manager.p_positioning.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < world_manager.p_positioning.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < world_manager.p_positioning.p_chunk_body_dimensions.p_depth; z++) {
                ESS__world_vertex chunk_position = MANAGER__calculate__chunk_position_by_offset(valid_chunk_body_box, x, y, z);
                MANAGER__chunk_body_slot_index current_slot_index = MANAGER__find__unavaliable_chunk_body_slot_index(world_manager, chunk_position);

                // check for not loaded chunk
                if (current_slot_index >= world_manager.p_positioning.p_chunk_body_count) {
                    // get an available slot
                    MANAGER__chunk_body_slot_index available_slot_index = MANAGER__find__available_chunk_body_slot_index(world_manager);

                    // load chunk
                    MANAGER__load__chunk_body(world_manager, available_slot_index, chunk_position, skins, temps);

                    // mark chunk as loaded
                    ((MANAGER__slot*)world_manager.p_chunk_body_slots.p_address)[available_slot_index].p_availability = MANAGER__sat__unavailable;

                    // DEBUG
                    chunks_loaded++;
                }
            }
        }
    }

    // DEBUG
    //printf("Chunks Loaded: %lu\n", (u64)chunks_loaded);

    return;
}

// unload and load the correct chunks
void MANAGER__update__world(MANAGER__world_manager world_manager, SKIN__skins skins, RENDER__temporaries temps) {
    // calculate current player chunk
    ESS__world_vertex current_chunk_coord = ESS__calculate__current_chunk_by_coordinates(world_manager.p_positioning.p_camera_position);

    // create the chunk body box
    ESS__world_box valid_chunk_body_box = MANAGER__calculate__valid_chunk_body_box(current_chunk_coord, world_manager.p_positioning.p_chunk_body_dimensions);

    /*// DEBUG
    printf("Current Chunk: ");
    ESS__print__world_vertex(current_chunk_coord);
    printf(" Box: ");
    ESS__print__world_box(valid_chunk_body_box);
    printf("\n");*/

    // open up slots if necessary
    MANAGER__crop__slots(world_manager, valid_chunk_body_box);

    // load slots in new chunks
    MANAGER__load__new_chunks(world_manager, valid_chunk_body_box, skins, temps);
    
    return;
}

#endif
