#ifndef VOXELIZE__block
#define VOXELIZE__block

/* Include */
#include "../base/essentials.h"

/* Defines */
// block ID
typedef u16 BLOCK__block_ID;

// liquid types
typedef u8 BLOCK__liquid_type; // actually a u4, lower bits
typedef u8 BLOCK__liquid_height; // actually a u4, higher bits
typedef u8 BLOCK__liquid_data; // both liquid type and height

// block axes
typedef u8 BLOCK__axis;
typedef BLOCK__axis BLOCK__block_x;
typedef BLOCK__axis BLOCK__block_y;
typedef BLOCK__axis BLOCK__block_z;

// null block ID
#define BLOCK__define__null_block_ID 0

// liquid data
#define BLOCK__define__null_liquid_ID 0
#define BLOCK__define__liquid_min_height 0
#define BLOCK__define__liquid_max_height ESS__define__nibble_max_value

/* Block */
// a block in the world
typedef struct __attribute__((packed)) BLOCK__block {
    BLOCK__block_ID p_block_ID;
} BLOCK__block;

// create a block
BLOCK__block BLOCK__create__block(BLOCK__block_ID block_ID) {
    BLOCK__block output;

    // setup output
    output.p_block_ID = block_ID;

    return output;
}

// create a null block
BLOCK__block BLOCK__create_null__block() {
    // return empty
    return BLOCK__create__block(BLOCK__define__null_block_ID);
}

/* Liquid */
// liquid data
typedef struct __attribute__((packed)) BLOCK__liquid {
    BLOCK__liquid_data p_data;
} BLOCK__liquid;

// create a custom liquid
BLOCK__liquid BLOCK__create__liquid(BLOCK__liquid_type liquid_type, BLOCK__liquid_height liquid_height) {
    BLOCK__liquid output;

    // setup output
    output.p_data = (liquid_type & ESS__define__nibble_max_value) + ((liquid_height & ESS__define__nibble_max_value) << ESS__define__bits_per_nibble);

    return output;
}

// create null liquid
BLOCK__liquid BLOCK__create_null__liquid() {
    // return empty
    return BLOCK__create__liquid(BLOCK__define__null_liquid_ID, BLOCK__define__liquid_max_height);
}

/* Super Block */
typedef struct __attribute__((packed)) BLOCK__superblock {
    BLOCK__block p_block;
    BLOCK__liquid p_liquid;
} BLOCK__superblock;

// create custom super block
BLOCK__superblock BLOCK__create__superblock(BLOCK__block block, BLOCK__liquid liquid) {
    BLOCK__superblock output;

    // setup output
    output.p_block = block;
    output.p_liquid = liquid;

    return output;
}

// create null superblock
BLOCK__superblock BLOCK__create_null__superblock() {
    // return empty
    return BLOCK__create__superblock(BLOCK__create_null__block(), BLOCK__create_null__liquid());
}

#endif
