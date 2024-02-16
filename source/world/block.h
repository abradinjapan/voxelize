#ifndef VOXELIZE__block
#define VOXELIZE__block

/* Include */
#include "../base/essentials.h"

/* Defines */
// block ID
typedef u32 BLOCK__block_ID;

// block metadata types
typedef u8 BLOCK__metadatum;
typedef u64 BLOCK__metadatum_index;

// total block size in bytes
#define BLOCK__define__block_byte_count (sizeof(u64)*2)

// null block ID
#define BLOCK__define__null_block_ID -1

// block metadata byte size
#define BLOCK__define__metadata_byte_count (BLOCK__define__block_byte_count-sizeof(BLOCK__block_ID))

/* Block Metadata */
// the extra information of a block
typedef struct __attribute__((packed)) BLOCK__metadata {
    BLOCK__metadatum p_medatadums[BLOCK__define__metadata_byte_count];
} BLOCK__metadata;

// create a block metadata
BLOCK__metadata BLOCK__create__metadata(BLOCK__metadatum filler) {
    BLOCK__metadata output;

    // setup output
    // initialize bytes
    for (BLOCK__metadatum_index index = 0; index < BLOCK__define__metadata_byte_count; index++) {
        // setup byte
        output.p_medatadums[index] = filler;
    }

    return output;
}

// create a null metadata
BLOCK__metadata BLOCK__create_null__metadata() {
    // return empty
    return BLOCK__create__metadata(0);
}

/* Block */
// a block in the world
typedef struct __attribute__((packed)) BLOCK__block {
    BLOCK__block_ID p_block_ID;
    BLOCK__metadata p_metadata;
} BLOCK__block;

// create a block
BLOCK__block BLOCK__create__block(BLOCK__block_ID block_ID, BLOCK__metadata metadata) {
    BLOCK__block output;

    // setup output
    output.p_block_ID = block_ID;
    output.p_metadata = metadata;

    return output;
}

// create a null block
BLOCK__block BLOCK__create_null__block() {
    // return empty
    return BLOCK__create__block(BLOCK__define__null_block_ID, BLOCK__create_null__metadata());
}

#endif
