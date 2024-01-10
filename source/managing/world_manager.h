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
    // tracked objects
    RENDER__world p_rendered_world;
    CHUNK__chunks p_chunks;
    POS__positioning p_positions;

    // slots allocation
    BASIC__buffer p_allocation;

    // slots
    BASIC__buffer p_chunk_body_slots;
    BASIC__buffer p_chunk_surface_slots;
} MANAGER__world_manager;

// open a world manager
MANAGER__world_manager MANAGER__open__world_manager() {
    
}

#endif
