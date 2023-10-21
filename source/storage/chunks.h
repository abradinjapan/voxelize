#ifndef VOXELIZE__chunks
#define VOXELIZE__chunks

/* Include */
#include "../base/essentials.h"

/* Define */
// coordinates
typedef ESS__world_x CHUNK__coordinate_x;
typedef ESS__world_y CHUNK__coordinate_y;
typedef ESS__world_z CHUNK__coordinate_z;

// the data container of one block (block type ID but also unique block meta data)
typedef u64 CHUNK__block_data;

// the index of a block in the one dimensional chunk blocks array
typedef u16 CHUNK__block_index;

// the block ID
typedef u64 CHUNK__block_ID;

// the sides of a chunk
typedef u8 CHUNK__chunk_axis_index;
typedef CHUNK__chunk_axis_index CHUNK__chunk_x;
typedef CHUNK__chunk_axis_index CHUNK__chunk_y;
typedef CHUNK__chunk_axis_index CHUNK__chunk_z;

// the index of a chunk into a 3d chunk array
typedef u32 CHUNK__chunks_index;

// the sides of the chunks
typedef u8 CHUNK__chunks_axis_index;
typedef CHUNK__chunks_axis_index CHUNK__chunks_x;
typedef CHUNK__chunks_axis_index CHUNK__chunks_y;
typedef CHUNK__chunks_axis_index CHUNK__chunks_z;

/* Block */
// get block type from block data
CHUNK__block_ID CHUNK__get__block_ID_from_block_data(CHUNK__block_data block_data) {
    // extract block type and return
    return (CHUNK__block_ID)block_data;
}

// create block data from raw block type
CHUNK__block_data CHUNK__create__block(CHUNK__block_ID block_ID) {
    // setup the data for the block (can be more complex later as block data has other states packed in)
    return (CHUNK__block_data)block_ID;
}

// create empty block
CHUNK__block_data CHUNK__create_null__block() {
    // return empty
    return CHUNK__create__block(0);
}

/* Chunk */
typedef struct CHUNK__chunk {
    CHUNK__block_data p_blocks[ESS__define__chunk_total_block_count];
} CHUNK__chunk;

// chunk address
typedef CHUNK__chunk* CHUNK__chunk_address;

// create a chunk with one specific block data for every block
CHUNK__chunk CHUNK__create__chunk(CHUNK__block_data block_data) {
    CHUNK__chunk output;

    // setup block data
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count; block_index++) {
        // set block
        output.p_blocks[block_index] = block_data;
    }
    
    return output;
}

// create a blank chunk
CHUNK__chunk CHUNK__create_null__chunk() {
    // create empty chunk
    return CHUNK__create__chunk(CHUNK__create_null__block());
}

// create chunk where every other block is air and one custom block type
CHUNK__chunk CHUNK__create__chunk__alternating_block_pattern(CHUNK__block_ID first_block, CHUNK__block_ID second_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(CHUNK__create__block(first_block));

    // setup every even block to be the chosen block type
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count; block_index += 2) {
        // set block to second type
        output.p_blocks[block_index] = CHUNK__create__block(second_block);
    }

    return output;
}

// create chunk where every 3 blocks are a custom type
CHUNK__chunk CHUNK__create__chunk__3_rotating_block_pattern(CHUNK__block_data first_block, CHUNK__block_data second_block, CHUNK__block_data third_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(CHUNK__create__block(first_block));

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
CHUNK__chunk CHUNK__create__chunk__7_rotating_block_pattern(CHUNK__block_data first_six_blocks, CHUNK__block_data seventh_block) {
    CHUNK__chunk output;

    // setup output to all first type
    output = CHUNK__create__chunk(CHUNK__create__block(first_six_blocks));

    // setup every seventh block to be the chosen block type
    for (CHUNK__block_index block_index = 0; block_index < ESS__define__chunk_total_block_count - 7; block_index += 7) {
        // set blocks
        output.p_blocks[block_index] = seventh_block;
    }

    return output;
}

// calculate block index from the block's chunk internal coords
CHUNK__block_index CHUNK__calculate__block_index(CHUNK__chunk_x block_x, CHUNK__chunk_y block_y, CHUNK__chunk_z block_z) {
    // calculate (z is forward and back, y is up and down, x is left and right)
    return (block_z * ESS__define__chunk_slice_block_count) + (block_y * ESS__define__chunk_side_block_count) + block_x;
}

// set block
void CHUNK__set__block_data_from_chunk_address(CHUNK__chunk_address chunk_address, CHUNK__block_index block_index, CHUNK__block_data block_data) {
    // set data
    (*chunk_address).p_blocks[block_index] = block_data;

    return;
}

// get block
CHUNK__block_data CHUNK__get__block_data_from_chunk_address(CHUNK__chunk_address chunk_address, CHUNK__block_index block_index) {
    // return data
    return (*chunk_address).p_blocks[block_index];
}

/* Chunks */
typedef struct CHUNK__chunks {
    BASIC__buffer p_chunk_block_data;
    CHUNK__chunks_x p_width;
    CHUNK__chunks_y p_height;
    CHUNK__chunks_z p_depth;
} CHUNK__chunks;

// close chunks
void CHUNK__close__chunks(CHUNK__chunks chunks) {
    // close buffer data
    BASIC__close__buffer(chunks.p_chunk_block_data);

    return;
}

// setup chunks
CHUNK__chunks CHUNK__create__chunks(BASIC__buffer chunk_block_data, CHUNK__chunks_x width, CHUNK__chunks_y height, CHUNK__chunks_z depth) {
    CHUNK__chunks output;

    // setup output
    output.p_chunk_block_data = chunk_block_data;
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;

    return output;
}

// setup null chunks
CHUNK__chunks CHUNK__create_null__chunks() {
    // setup empty chunks
    return CHUNK__create__chunks(BASIC__create_null__buffer(), 0, 0, 0);
}

// calculate chunk index from coords
CHUNK__chunks_index CHUNK__calculate__chunks_index(CHUNK__chunks chunks, CHUNK__chunks_x x, CHUNK__chunks_y y, CHUNK__chunks_z z) {
    // return chunk index
    return (chunks.p_height * chunks.p_width * z) + (chunks.p_width * y) + x;
}

/*// calculate the world position of a chunk in chunks
CHUNK__position CHUNK__calculate__chunk_world_position(CHUNK__position initial_position, CHUNK__chunks_x x, CHUNK__chunks_y y, CHUNK__chunks_z z) {
    CHUNK__position output;

    // setup output
    output.p_x = initial_position.p_x + (x * ESS__calculate__chunk_size_in_world_coordinates());
    output.p_y = initial_position.p_y + (y * ESS__calculate__chunk_size_in_world_coordinates());
    output.p_z = initial_position.p_z + (z * ESS__calculate__chunk_size_in_world_coordinates());

    return output;
}*/

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
CHUNK__chunks CHUNK__open__chunks(CHUNK__chunks_x width, CHUNK__chunks_y height, CHUNK__chunks_z depth, CHUNK__chunk_address default_chunk) {
    CHUNK__chunks output;

    // allocate chunk data
    output.p_chunk_block_data = BASIC__open__buffer(sizeof(CHUNK__chunk) * width * height * depth);

    // if allocation denied, return null result
    if (output.p_chunk_block_data.p_address == 0) {
        // return null chunks
        return CHUNK__create_null__chunks();
    }

    // setup size data
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;

    // setup chunks
    // x
    for (CHUNK__chunks_x chunks_width = 0; chunks_width < width; chunks_width++) {
        // y
        for (CHUNK__chunks_y chunks_height = 0; chunks_height < height; chunks_height++) {
            // z
            for (CHUNK__chunks_z chunks_depth = 0; chunks_depth < depth; chunks_depth++) {
                // setup chunk data
                CHUNK__set__chunk_in_chunks(output, CHUNK__calculate__chunks_index(output, chunks_width, chunks_height, chunks_depth), default_chunk);
            }
        }
    }

    return output;
}

#endif
