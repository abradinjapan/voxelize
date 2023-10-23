#ifndef VOXELIZE__positions
#define VOXELIZE__positions

/* Include */
#include "../base/essentials.h"
#include "chunks.h"
#include "chunk_surfaces.h"

/* Define */
// position indexing
typedef u64 POS__position_index;
typedef POS__position_index POS__position_count;

/* Position Manager */
// all game positions
typedef struct POS__positions {
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
} POS__positions;

// create custom positions
POS__positions POS__create__positions(ESS__world_vertex camera_position, BASIC__buffer allocation, BASIC__buffer chunk_body_positions, BASIC__buffer chunk_XY_surface_positions, BASIC__buffer chunk_YZ_surface_positions, BASIC__buffer chunk_XZ_surface_positions, ESS__dimensions chunk_body_dimensions, ESS__dimensions chunk_XY_surface_dimensions, ESS__dimensions chunk_YZ_surface_dimensions, ESS__dimensions chunk_XZ_surface_dimensions) {
    POS__positions output;

    // setup output
    output.p_camera_position = camera_position;
    output.p_allocation = allocation;
    output.p_chunk_body_positions = chunk_body_positions;
    output.p_chunk_XY_surface_positions = chunk_XY_surface_positions;
    output.p_chunk_YZ_surface_positions = chunk_YZ_surface_positions;
    output.p_chunk_XZ_surface_positions = chunk_XZ_surface_positions;

    // calculate lengths
    output.p_allocation_count = output.p_allocation.p_length / sizeof(ESS__world_vertex);
    output.p_chunk_body_count = output.p_chunk_body_positions.p_length / sizeof(ESS__world_vertex);
    output.p_chunk_XY_surface_count = output.p_chunk_XY_surface_positions.p_length / sizeof(ESS__world_vertex);
    output.p_chunk_YZ_surface_count = output.p_chunk_YZ_surface_positions.p_length / sizeof(ESS__world_vertex);
    output.p_chunk_XZ_surface_count = output.p_chunk_XZ_surface_positions.p_length / sizeof(ESS__world_vertex);

    // setup dimensions
    output.p_chunk_body_dimensions = chunk_body_dimensions;
    output.p_chunk_XY_surface_dimensions = chunk_XY_surface_dimensions;
    output.p_chunk_YZ_surface_dimensions = chunk_YZ_surface_dimensions;
    output.p_chunk_XZ_surface_dimensions = chunk_XZ_surface_dimensions;

    return output;
}

// create null positions
POS__positions POS__create_null__positions() {
    // return null
    return POS__create__positions(ESS__create_null__world_vertex(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), BASIC__create_null__buffer(), ESS__create_null__dimensions(), ESS__create_null__dimensions(), ESS__create_null__dimensions(), ESS__create_null__dimensions());
}

// create a sub allocation
BASIC__buffer POS__calculate__sub_allocation(BASIC__buffer original_allocation, u64 vertex_offset, u64 vertex_count) {
    // return calculation
    return BASIC__create__buffer(original_allocation.p_address + (sizeof(ESS__world_vertex) * vertex_offset), (sizeof(ESS__world_vertex) * vertex_count));
}

// calculate the world position of a chunk inside a chunks object
ESS__world_vertex POS__calculate__chunk_position_in_chunks(ESS__world_vertex chunks_position, CHUNK__chunks_x x, CHUNK__chunks_y y, CHUNK__chunks_z z) {
    ESS__world_vertex output;

    // calculate position
    output.p_x = chunks_position.p_x - (ESS__calculate__chunk_size_in_world_coordinates() * x);
    output.p_y = chunks_position.p_y - (ESS__calculate__chunk_size_in_world_coordinates() * y);
    output.p_z = chunks_position.p_z - (ESS__calculate__chunk_size_in_world_coordinates() * z);

    return output;
}

// open positions
POS__positions POS__open__positions(ESS__world_vertex camera_position, ESS__world_vertex chunks_position, CHUNK__chunks chunks) {
    POS__positions output;
    ESS__dimensions_volume chunk_bodies_volume;
    ESS__dimensions_volume chunk_XY_surfaces_volume;
    ESS__dimensions_volume chunk_YZ_surfaces_volume;
    ESS__dimensions_volume chunk_XZ_surfaces_volume;
    BASIC__buffer allocation;

    // setup camera position
    output.p_camera_position = camera_position;

    // calculate dimensions
    output.p_chunk_body_dimensions = chunks.p_dimensions;
    output.p_chunk_XY_surface_dimensions = CHUNKSURFACE__calculate__XY_surface_dimensions_from_chunks_dimensions(chunks.p_dimensions);
    output.p_chunk_YZ_surface_dimensions = CHUNKSURFACE__calculate__YZ_surface_dimensions_from_chunks_dimensions(chunks.p_dimensions);
    output.p_chunk_XZ_surface_dimensions = CHUNKSURFACE__calculate__XZ_surface_dimensions_from_chunks_dimensions(chunks.p_dimensions);

    // calculate volumes
    chunk_bodies_volume = CHUNK__calculate__chunks_count(chunks.p_dimensions);
    chunk_XY_surfaces_volume = CHUNKSURFACE__calculate__XY_surfaces_volume(chunks.p_dimensions);
    chunk_YZ_surfaces_volume = CHUNKSURFACE__calculate__YZ_surfaces_volume(chunks.p_dimensions);
    chunk_XZ_surfaces_volume = CHUNKSURFACE__calculate__XZ_surfaces_volume(chunks.p_dimensions);

    // allocate positions
    allocation = BASIC__open__buffer(sizeof(ESS__world_vertex) * (chunk_bodies_volume + chunk_XY_surfaces_volume + chunk_YZ_surfaces_volume + chunk_XZ_surfaces_volume));

    // create positions
    output = POS__create__positions(camera_position, allocation, POS__calculate__sub_allocation(allocation, 0, chunk_bodies_volume), POS__calculate__sub_allocation(allocation, chunk_bodies_volume, chunk_XY_surfaces_volume), POS__calculate__sub_allocation(allocation, chunk_bodies_volume + chunk_XY_surfaces_volume, chunk_YZ_surfaces_volume), POS__calculate__sub_allocation(allocation, chunk_bodies_volume + chunk_XY_surfaces_volume + chunk_YZ_surfaces_volume, chunk_XZ_surfaces_volume), output.p_chunk_body_dimensions, output.p_chunk_XY_surface_dimensions, output.p_chunk_YZ_surface_dimensions, output.p_chunk_XZ_surface_dimensions);

    // initialize all chunk body positions to be relative to their chunk offset
    for (CHUNK__chunks_x x = 0; x < output.p_chunk_body_dimensions.p_width; x++) {
        for (CHUNK__chunks_y y = 0; y < output.p_chunk_body_dimensions.p_height; y++) {
            for (CHUNK__chunks_z z = 0; z < output.p_chunk_body_dimensions.p_depth; z++) {
                // initialize chunk position
                ((ESS__world_vertex*)output.p_chunk_body_positions.p_address)[CHUNK__calculate__chunks_index(chunks, x, y, z)] = POS__calculate__chunk_position_in_chunks(chunks_position, x, y, z);
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
void POS__close__positions(POS__positions positions) {
    // close positions
    BASIC__close__buffer(positions.p_allocation);

    return;
}

#endif
