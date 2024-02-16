#ifndef VOXELIZE__generation
#define VOXELIZE__generation

/* Include */
#include "chunks.h"

/* Define */
// function pointer that calls a chunk generation function
typedef CHUNK__chunk (*GENERATION__function_address__generate_chunk)(ESS__world_vertex);

#endif
