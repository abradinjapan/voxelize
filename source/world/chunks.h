#ifndef VOXELIZE__chunks
#define VOXELIZE__chunks

/* Include */
#include "block.h"

/* Define */
// coordinates
typedef ESS__world_x CHUNK__coordinate_x;
typedef ESS__world_y CHUNK__coordinate_y;
typedef ESS__world_z CHUNK__coordinate_z;

// the index of a block in the one dimensional chunk blocks array
typedef u16 CHUNK__block_index;

// the index of a chunk into a 3d chunk array
typedef u32 CHUNK__chunks_index;

// the sides of the chunks
typedef u16 CHUNK__chunks_axis_index;
typedef CHUNK__chunks_axis_index CHUNK__chunks_x;
typedef CHUNK__chunks_axis_index CHUNK__chunks_y;
typedef CHUNK__chunks_axis_index CHUNK__chunks_z;

// the total count of chunks
typedef ESS__dimensions_volume CHUNK__chunk_count;

/* Block Position */
// the type that stores one block position
typedef struct CHUNK__block_position {
    BLOCK__block_x p_x;
    BLOCK__block_y p_y;
    BLOCK__block_z p_z;
} CHUNK__block_position;

// create a block position
CHUNK__block_position CHUNK__create__block_position(BLOCK__block_x x, BLOCK__block_y y, BLOCK__block_z z) {
    CHUNK__block_position output;

    // setup output
    output.p_x = x;
    output.p_y = y;
    output.p_z = z;

    return output;
}

// create null position
CHUNK__block_position CHUNK__create_null__block_position() {
    // return empty
    return CHUNK__create__block_position(0, 0, 0);
}

/* Chunk */
typedef struct CHUNK__chunk {
    BLOCK__block p_blocks[ESS__define__chunk_total_block_count];
} CHUNK__chunk;

// chunk address
typedef CHUNK__chunk* CHUNK__chunk_address;

// calculate block index from the block's chunk internal coords
CHUNK__block_index CHUNK__calculate__block_index(CHUNK__block_position block_position) {
    // calculate (z is forward and back, y is up and down, x is left and right)
    return (block_position.p_z * ESS__define__chunk_slice_block_count) + (block_position.p_y * ESS__define__chunk_side_block_count) + block_position.p_x;
}

// set block
void CHUNK__set__block_data_from_chunk_address(CHUNK__chunk_address chunk_address, CHUNK__block_index block_index, BLOCK__block block) {
    // set data
    (*chunk_address).p_blocks[block_index] = block;

    return;
}

// get block
BLOCK__block CHUNK__get__block_from_chunk_address(CHUNK__chunk_address chunk_address, CHUNK__block_index block_index) {
    // return data
    return (*chunk_address).p_blocks[block_index];
}

// create a chunk with one specific block data for every block
CHUNK__chunk CHUNK__create__chunk(BLOCK__block block) {
    CHUNK__chunk output;

    // setup block data
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count; block_index++) {
        // set block
        output.p_blocks[block_index] = block;
    }
    
    return output;
}

// create a blank chunk
CHUNK__chunk CHUNK__create_null__chunk() {
    // create empty chunk
    return CHUNK__create__chunk(BLOCK__create_null__block());
}

// create chunk where every other block is air and one custom block type
CHUNK__chunk CHUNK__create__chunk__alternating_block_pattern(BLOCK__block first_block, BLOCK__block second_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(first_block);

    // setup every even block to be the chosen block type
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count; block_index += 2) {
        // set block to second type
        output.p_blocks[block_index] = second_block;
    }

    return output;
}

// create chunk where every 3 blocks are a custom type
CHUNK__chunk CHUNK__create__chunk__3_rotating_block_pattern(BLOCK__block first_block, BLOCK__block second_block, BLOCK__block third_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(first_block);

    // setup every even block to be the chosen block type
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count - 3; block_index += 3) {
        // set blocks
        output.p_blocks[block_index + 0] = first_block;
        output.p_blocks[block_index + 1] = second_block;
        output.p_blocks[block_index + 2] = third_block;
    }

    return output;
}

// create chunk with 6 of one type and another seventh in a rotating pattern
CHUNK__chunk CHUNK__create__chunk__7_rotating_block_pattern(BLOCK__block first_six_blocks, BLOCK__block seventh_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(first_six_blocks);

    // setup every seventh block to be the chosen block type
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count - 7; block_index += 7) {
        // set blocks
        output.p_blocks[block_index] = seventh_block;
    }

    return output;
}

// generate a chunk with a bar on all three axes
CHUNK__chunk CHUNK__create__chunk__bars(BLOCK__block air, BLOCK__block bar) {
    CHUNK__chunk output;

    // initialize blank chunk
    output = CHUNK__create__chunk(air);

    // create x bar
    for (CHUNK__block_index x = 0; x < ESS__define__chunk_side_block_count; x++) {
        // set block
        output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(x, 8, 8))] = bar;
    }

    // create y bar
    for (CHUNK__block_index y = 0; y < ESS__define__chunk_side_block_count; y++) {
        // set block
        output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(8, y, 7))] = bar;
    }

    // create z bar
    for (CHUNK__block_index z = 0; z < ESS__define__chunk_side_block_count; z++) {
        // set block
        output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(7, 7, z))] = bar;
    }

    return output;
}

/* Chunks */
typedef struct CHUNK__chunks {
    BASIC__buffer p_chunk_block_data;
} CHUNK__chunks;

// close chunks
void CHUNK__close__chunks(CHUNK__chunks chunks) {
    // close buffer data
    BASIC__close__buffer(chunks.p_chunk_block_data);

    return;
}

// setup chunks
CHUNK__chunks CHUNK__create__chunks(BASIC__buffer chunk_block_data) {
    CHUNK__chunks output;

    // setup output
    output.p_chunk_block_data = chunk_block_data;

    return output;
}

// setup null chunks
CHUNK__chunks CHUNK__create_null__chunks() {
    // setup empty chunks
    return CHUNK__create__chunks(BASIC__create_null__buffer());
}

// calculate chunk dimensions
CHUNK__chunk_count CHUNK__calculate__chunks_count(ESS__dimensions dimensions) {
    // return calculation
    return (CHUNK__chunk_count)ESS__calculate__dimensions_volume(dimensions);
}

// get chunk address
CHUNK__chunk_address CHUNK__get__chunk_pointer_in_chunks(CHUNK__chunks chunks, CHUNK__chunks_index chunks_index) {
    // return data
    return (CHUNK__chunk_address)(chunks.p_chunk_block_data.p_address + (sizeof(CHUNK__chunk) * chunks_index));
}

// set chunk by address
void CHUNK__set__chunk_in_chunks(CHUNK__chunks chunks, CHUNK__chunks_index chunks_index, CHUNK__chunk_address new_chunk_address) {
    CHUNK__chunk_address setting_chunk_address;

    // get chunk
    setting_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, chunks_index);

    // copy block data
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count; block_index++) {
        (*setting_chunk_address).p_blocks[block_index] = (*new_chunk_address).p_blocks[block_index];
    }

    return;
}

// allocate and fill chunks
CHUNK__chunks CHUNK__open__chunks(CHUNK__chunk_count chunk_count, CHUNK__chunk_address default_chunk) {
    CHUNK__chunks output;

    // allocate chunk data
    output.p_chunk_block_data = BASIC__open__buffer(sizeof(CHUNK__chunk) * chunk_count);

    // if allocation denied, return null result
    if (output.p_chunk_block_data.p_address == 0) {
        // return null chunks
        return CHUNK__create_null__chunks();
    }

    // setup chunks
    for (CHUNK__chunk_count chunk_index = 0; chunk_index < chunk_count; chunk_index++) {
        // set one chunk
        CHUNK__set__chunk_in_chunks(output, chunk_index, default_chunk);
    }

    return output;
}

#endif
