#ifndef VOXELIZE__chunk_surfaces
#define VOXELIZE__chunk_surfaces

/* Include */
#include "chunks.h"

/* Volume Calculations */
ESS__dimensions CHUNKSURFACE__calculate__XY_surface_dimensions_from_chunks_dimensions(ESS__dimensions chunks_dimensions) {
    return ESS__create__dimensions(chunks_dimensions.p_width, chunks_dimensions.p_height, chunks_dimensions.p_depth - 1);
}

ESS__dimensions CHUNKSURFACE__calculate__YZ_surface_dimensions_from_chunks_dimensions(ESS__dimensions chunks_dimensions) {
    return ESS__create__dimensions(chunks_dimensions.p_width - 1, chunks_dimensions.p_height, chunks_dimensions.p_depth);
}

ESS__dimensions CHUNKSURFACE__calculate__XZ_surface_dimensions_from_chunks_dimensions(ESS__dimensions chunks_dimensions) {
    return ESS__create__dimensions(chunks_dimensions.p_width, chunks_dimensions.p_height - 1, chunks_dimensions.p_depth);
}

ESS__dimensions_volume CHUNKSURFACE__calculate__XY_surfaces_volume(ESS__dimensions chunks_dimensions) {
    // return calculation
    return ESS__calculate__dimensions_volume(CHUNKSURFACE__calculate__XY_surface_dimensions_from_chunks_dimensions(chunks_dimensions));
}

ESS__dimensions_volume CHUNKSURFACE__calculate__YZ_surfaces_volume(ESS__dimensions chunks_dimensions) {
    // return calculation
    return ESS__calculate__dimensions_volume(CHUNKSURFACE__calculate__YZ_surface_dimensions_from_chunks_dimensions(chunks_dimensions));
}

ESS__dimensions_volume CHUNKSURFACE__calculate__XZ_surfaces_volume(ESS__dimensions chunks_dimensions) {
    // return calculation
    return ESS__calculate__dimensions_volume(CHUNKSURFACE__calculate__XZ_surface_dimensions_from_chunks_dimensions(chunks_dimensions));
}

#endif
