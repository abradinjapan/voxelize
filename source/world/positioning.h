#ifndef VOXELIZE__positioning
#define VOXELIZE__positioning

/* Include */
#include "../base/essentials.h"
#include "chunks.h"
#include "chunk_surfaces.h"

/* Define */
// position indexing
typedef u64 POS__position_index;
typedef POS__position_index POS__position_count;

/* Positioning Manager */
// all game positions & related data
typedef struct POS__positioning {
    // the position of the camera
    ESS__world_vertex p_camera_position;

    // the positions of all the chunks in world space
    BASIC__buffer p_position_allocation;
    BASIC__buffer p_chunk_body_positions;
    BASIC__buffer p_chunk_XY_surface_positions;
    BASIC__buffer p_chunk_YZ_surface_positions;
    BASIC__buffer p_chunk_XZ_surface_positions;

    // the counts of the positions
    POS__position_count p_total_count;
    POS__position_count p_chunk_body_count;
    POS__position_count p_chunk_XY_surface_count;
    POS__position_count p_chunk_YZ_surface_count;
    POS__position_count p_chunk_XZ_surface_count;

    // the dimensions of the chunks & surfaces
    ESS__dimensions p_chunk_body_dimensions;
    ESS__dimensions p_chunk_XY_surface_dimensions;
    ESS__dimensions p_chunk_YZ_surface_dimensions;
    ESS__dimensions p_chunk_XZ_surface_dimensions;
} POS__positioning;

// create a position sub allocation
BASIC__buffer POS__calculate__position_sub_allocation(BASIC__buffer original_allocation, u64 vertex_offset, u64 vertex_count) {
    // return calculation
    return BASIC__create__buffer(original_allocation.p_address + (sizeof(ESS__world_vertex) * vertex_offset), (sizeof(ESS__world_vertex) * vertex_count));
}

// calculate the world position of a chunk inside a chunks object
ESS__world_vertex POS__calculate__chunk_position_in_chunks(ESS__world_vertex chunks_position, CHUNK__chunks_x x, CHUNK__chunks_y y, CHUNK__chunks_z z) {
    ESS__world_vertex output;

    // calculate position
    output.p_x = chunks_position.p_x - (ESS__calculate__chunk_side_size_in_world_coordinates() * x);
    output.p_y = chunks_position.p_y - (ESS__calculate__chunk_side_size_in_world_coordinates() * y);
    output.p_z = chunks_position.p_z - (ESS__calculate__chunk_side_size_in_world_coordinates() * z);

    return output;
}

// open positions
POS__positioning POS__open__positioning(ESS__world_vertex camera_position, ESS__dimensions chunks_dimensions) {
    POS__positioning output;

    // setup camera position
    output.p_camera_position = camera_position;

    // calculate dimensions
    output.p_chunk_body_dimensions = chunks_dimensions;
    output.p_chunk_XY_surface_dimensions = CHUNKSURFACE__calculate__XY_surface_dimensions_from_chunks_dimensions(chunks_dimensions);
    output.p_chunk_YZ_surface_dimensions = CHUNKSURFACE__calculate__YZ_surface_dimensions_from_chunks_dimensions(chunks_dimensions);
    output.p_chunk_XZ_surface_dimensions = CHUNKSURFACE__calculate__XZ_surface_dimensions_from_chunks_dimensions(chunks_dimensions);

    // calculate counts
    output.p_chunk_body_count = CHUNK__calculate__chunks_count(chunks_dimensions);
    output.p_chunk_XY_surface_count = CHUNKSURFACE__calculate__XY_surfaces_volume(chunks_dimensions);
    output.p_chunk_YZ_surface_count = CHUNKSURFACE__calculate__YZ_surfaces_volume(chunks_dimensions);
    output.p_chunk_XZ_surface_count = CHUNKSURFACE__calculate__XZ_surfaces_volume(chunks_dimensions);
    output.p_total_count = output.p_chunk_body_count + output.p_chunk_XY_surface_count + output.p_chunk_YZ_surface_count + output.p_chunk_XZ_surface_count;

    // allocate positions
    output.p_position_allocation = BASIC__open__buffer(sizeof(ESS__world_vertex) * output.p_total_count);

    // create sub allocations
    output.p_chunk_body_positions = POS__calculate__position_sub_allocation(output.p_position_allocation, 0, output.p_chunk_body_count);
    output.p_chunk_XY_surface_positions = POS__calculate__position_sub_allocation(output.p_position_allocation, output.p_chunk_body_count, output.p_chunk_XY_surface_count);
    output.p_chunk_YZ_surface_positions = POS__calculate__position_sub_allocation(output.p_position_allocation, output.p_chunk_body_count + output.p_chunk_XY_surface_count, output.p_chunk_YZ_surface_count);
    output.p_chunk_XZ_surface_positions = POS__calculate__position_sub_allocation(output.p_position_allocation, output.p_chunk_body_count + output.p_chunk_XY_surface_count + output.p_chunk_YZ_surface_count, output.p_chunk_XZ_surface_count);

    // blank initialize all positions
    for (POS__position_index index = 0; index < output.p_total_count; index++) {
        // setup blank position
        ((ESS__world_vertex*)output.p_position_allocation.p_address)[index] = ESS__create_null__world_vertex();
    }

    return output;
}

// close positions
void POS__close__positioning(POS__positioning positioning) {
    // close positions
    BASIC__close__buffer(positioning.p_position_allocation);

    return;
}

// calculate the block position inside of a chunk (assumes the chunk_position and subject position are within one chunk!)
CHUNK__block_position POS__calculate__block_local_position_from_world_position(ESS__world_vertex chunk_position, ESS__world_vertex subject_position) {
    CHUNK__block_position output;

    // calculate block position
    output.p_x = (chunk_position.p_x - subject_position.p_x) / (ESS__define__bits_per_block__total_count);
    output.p_y = (chunk_position.p_y - subject_position.p_y) / (ESS__define__bits_per_block__total_count);
    output.p_z = (chunk_position.p_z - subject_position.p_z) / (ESS__define__bits_per_block__total_count);
    
    return output;
}

// calculate the block position inside of a chunk from world coordinates (assumes the chunk_position and subject position are within one chunk!)
CHUNK__block_index POS__calculate__block_index_from_world_position(ESS__world_vertex chunk_position, ESS__world_vertex subject_position) {
    // return calculation
    return CHUNK__calculate__block_index(POS__calculate__block_local_position_from_world_position(chunk_position, subject_position));
}

#endif
