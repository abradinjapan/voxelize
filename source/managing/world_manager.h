#ifndef VOXELIZE__world_manager
#define VOXELIZE__world_manager

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
#define POS__define__invalid_index -1

/* Manager */
// one index into the chunk manager
typedef struct MANAGER__chunk_body_slot {
    // open spot flag
    BASIC__bt p_slot_open;

    // chunk array & chunk body indices
    MANAGER__chunks_index p_chunks_index;
    MANAGER__rendering_object_index p_rendering_index;
} MANAGER__chunk_body_slot;

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

#endif
