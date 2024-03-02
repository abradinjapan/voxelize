#ifndef VOXELIZE__generation
#define VOXELIZE__generation

/* Include */
#include "chunks.h"

/* Seed World Generation */
// the seed type
typedef u64 GENERATION__seed;

// the blueprint for the world generation
typedef struct GENERATION__blueprint {
    GENERATION__seed p_seed;
} GENERATION__blueprint;

// blueprint address
typedef GENERATION__blueprint* GENERATION__blueprint_address;

// create custom blueprint
GENERATION__blueprint GENERATION__create__blueprint(GENERATION__seed seed) {
    GENERATION__blueprint output;

    // setup output
    output.p_seed = seed;

    return output;
}

// create null blueprint
GENERATION__blueprint GENERATION__create_null__blueprint() {
    // return empty
    return GENERATION__create__blueprint(0);
}

/* Generation Function Pointer */
// function pointer that calls a chunk generation function
typedef CHUNK__chunk (*GENERATION__function_address__generate_chunk)(ESS__world_vertex, GENERATION__blueprint_address);

#endif
