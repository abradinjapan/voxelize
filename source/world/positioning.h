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
    BASIC__buffer p_allocation;
    BASIC__buffer p_chunk_body_positions;
    BASIC__buffer p_chunk_XY_surface_positions;
    BASIC__buffer p_chunk_YZ_surface_positions;
    BASIC__buffer p_chunk_XZ_surface_positions;

    // the counts of the positions
    POS__position_count p_allocation_count;
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

// create a sub allocation
BASIC__buffer POS__calculate__sub_allocation(BASIC__buffer original_allocation, u64 vertex_offset, u64 vertex_count) {
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
POS__positioning POS__open__positioning(ESS__world_vertex camera_position, ESS__world_vertex chunks_position, ESS__dimensions chunks_dimensions) {
    POS__positioning output;
    BASIC__buffer allocation;

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

    // allocate positions
    allocation = BASIC__open__buffer(sizeof(ESS__world_vertex) * (output.p_chunk_body_count + output.p_chunk_XY_surface_count + output.p_chunk_YZ_surface_count + output.p_chunk_XZ_surface_count));

    // create sub allocations
    output.p_chunk_body_positions = POS__calculate__sub_allocation(allocation, 0, output.p_chunk_body_count);
    output.p_chunk_XY_surface_positions = POS__calculate__sub_allocation(allocation, output.p_chunk_body_count, output.p_chunk_XY_surface_count);
    output.p_chunk_YZ_surface_positions = POS__calculate__sub_allocation(allocation, output.p_chunk_body_count + output.p_chunk_XY_surface_count, output.p_chunk_YZ_surface_count);
    output.p_chunk_XZ_surface_positions = POS__calculate__sub_allocation(allocation, output.p_chunk_body_count + output.p_chunk_XY_surface_count + output.p_chunk_YZ_surface_count, output.p_chunk_XZ_surface_count);

    // initialize all chunk body positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < output.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < output.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < output.p_chunk_body_dimensions.p_depth; z++) {
                // initialize chunk position
                ((ESS__world_vertex*)output.p_chunk_body_positions.p_address)[ESS__calculate__dimensions_index(output.p_chunk_body_dimensions, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y, z);
            }
        }
    }

    // initialize all chunk XY surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < output.p_chunk_XY_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < output.p_chunk_XY_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < output.p_chunk_XY_surface_dimensions.p_depth; z++) {
                // initialize chunk position
                ((ESS__world_vertex*)output.p_chunk_XY_surface_positions.p_address)[ESS__calculate__dimensions_index(output.p_chunk_XY_surface_dimensions, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y, z + 1);
            }
        }
    }

    // initialize all chunk YZ surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < output.p_chunk_YZ_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < output.p_chunk_YZ_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < output.p_chunk_YZ_surface_dimensions.p_depth; z++) {
                // initialize chunk position
                ((ESS__world_vertex*)output.p_chunk_YZ_surface_positions.p_address)[ESS__calculate__dimensions_index(output.p_chunk_YZ_surface_dimensions, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x + 1, y, z);
            }
        }
    }

    // initialize all chunk XZ surface positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < output.p_chunk_XZ_surface_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < output.p_chunk_XZ_surface_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < output.p_chunk_XZ_surface_dimensions.p_depth; z++) {
                // initialize chunk position
                ((ESS__world_vertex*)output.p_chunk_XZ_surface_positions.p_address)[ESS__calculate__dimensions_index(output.p_chunk_XZ_surface_dimensions, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y + 1, z);
            }
        }
    }

    return output;
}

// close positions
void POS__close__positioning(POS__positioning positioning) {
    // close positions
    BASIC__close__buffer(positioning.p_allocation);

    return;
}

// calculate the block position inside of a chunk from world coordinates
CHUNK__block_index POS__calculate__block_index_from_world_position(ESS__world_vertex chunk_position, ESS__world_vertex subject_position) {
    CHUNK__block_index output;
    CHUNK__block_position block_position;

    // calculate block position
    block_position.p_x = (chunk_position.p_x - subject_position.p_x) / (ESS__define__bits_per_block__total_count);
    block_position.p_y = (chunk_position.p_y - subject_position.p_y) / (ESS__define__bits_per_block__total_count);
    block_position.p_z = (chunk_position.p_z - subject_position.p_z) / (ESS__define__bits_per_block__total_count);

    // setup output
    output = CHUNK__calculate__block_index(block_position);

    return output;
}

#endif
