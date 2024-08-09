#ifndef VOXELIZE__random
#define VOXELIZE__random

/* Include */
// basic
#include "basic.h"

/* Define */
// seed type
typedef u64 RANDOM__seed;
typedef u64 RANDOM__iteration;
typedef u64 RANDOM__number;

/* Context */
// context for one random generator
typedef struct RANDOM__context {
    RANDOM__seed p_seed;
    RANDOM__iteration p_iteration;
} RANDOM__context;

// create custom context
RANDOM__context RANDOM__create__context(RANDOM__seed seed, RANDOM__iteration iteration) {
    RANDOM__context output;

    // setup output
    output.p_seed = seed;
    output.p_iteration = iteration;

    return output;
}

// create null context
RANDOM__context RANDOM__create_null__context() {
    return RANDOM__create__context(0, 0);
}

/* Generators */
// generate one number from a specific seed and iteration
RANDOM__number RANDOM__generate_number__mark_1(RANDOM__context* context, RANDOM__iteration iteration) {
    return (*context).p_seed * iteration * 998877665544332211;
}

// generate one random number and advance the iteration by 1
RANDOM__number RANDOM__generate_number_and_advance__mark_1(RANDOM__context* context) {
    RANDOM__number output;

    // generate number
    output = RANDOM__generate_number__mark_1(context, (*context).p_iteration);

    // increment iteration
    (*context).p_iteration++;

    return output;
}

// generate random numbe

#endif
