#ifndef VOXELIZE__block
#define VOXELIZE__block

/* Include */
#include "../base/essentials.h"

/* Defines */
// block ID
typedef u16 BLOCK__solid_ID;

// liquid types
typedef u8 BLOCK__liquid_type; // actually a u4, lower bits
typedef u8 BLOCK__liquid_height; // actually a u4, higher bits
typedef u8 BLOCK__liquid_data; // both liquid type and height

// block axes
typedef u8 BLOCK__axis;
typedef BLOCK__axis BLOCK__block_x;
typedef BLOCK__axis BLOCK__block_y;
typedef BLOCK__axis BLOCK__block_z;

// null solid ID
#define BLOCK__define__null_solid_ID 0

// liquid data
#define BLOCK__define__null_liquid_ID 0
#define BLOCK__define__liquid_min_height 0
#define BLOCK__define__liquid_max_height ESS__define__nibble_max_value

/* Solid */
// a solid block in the world
typedef struct __attribute__((packed)) BLOCK__solid {
    BLOCK__solid_ID p_ID;
} BLOCK__solid;

// create a block
BLOCK__solid BLOCK__create__solid(BLOCK__solid_ID solid_ID) {
    BLOCK__solid output;

    // setup output
    output.p_ID = solid_ID;

    return output;
}

// create a null solid
BLOCK__solid BLOCK__create_null__solid() {
    // return empty
    return BLOCK__create__solid(BLOCK__define__null_solid_ID);
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

/* Block */
typedef struct __attribute__((packed)) BLOCK__block {
    BLOCK__solid p_solid;
    BLOCK__liquid p_liquid;
} BLOCK__block;

// create custom block
BLOCK__block BLOCK__create__block(BLOCK__solid solid, BLOCK__liquid liquid) {
    BLOCK__block output;

    // setup output
    output.p_solid = solid;
    output.p_liquid = liquid;

    return output;
}

// create null block
BLOCK__block BLOCK__create_null__block() {
    // return empty
    return BLOCK__create__block(BLOCK__create_null__solid(), BLOCK__create_null__liquid());
}

/* Handy Functions */
// create a default block with only solid ID changed
BLOCK__block BLOCK__create__block_only_solid(BLOCK__solid_ID solid_ID) {
    // return data
    return BLOCK__create__block(BLOCK__create__solid(solid_ID), BLOCK__create_null__liquid());
}

#endif
