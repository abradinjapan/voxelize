#ifndef VOXELIZE__generation
#define VOXELIZE__generation

/* Include */
#include "chunks.h"

/* Seed World Generation */
// the seed type
typedef u64 GENERATION__seed;

// empty seed
#define GENERATION__define__null_seed 0

/* Terrain Features */
// terrain types
typedef ESS__world_axis GENERATION__peak_axis;
typedef GENERATION__peak_axis GENERATION__peak_x;
typedef GENERATION__peak_axis GENERATION__peak_y;
typedef GENERATION__peak_axis GENERATION__peak_z;
typedef GENERATION__peak_y GENERATION__height;
typedef GENERATION__height GENERATION__ocean_height;
typedef GENERATION__height GENERATION__terrain_height;
typedef GENERATION__terrain_height GENERATION__terrain_height__shortness;
typedef GENERATION__terrain_height GENERATION__terrain_height__tallness;

// null height
#define GENERATION__define__null_height 0

// distance between peaks
#define GENERATION__define__peak_spread_distance (ESS__calculate__chunk_side_size_in_world_coordinates()*1)

// peak coordinate
typedef struct GENERATION__peak_coordinate {
    GENERATION__peak_x p_x;
    GENERATION__peak_z p_z;
} GENERATION__peak_coordinate;

// create custom peak
GENERATION__peak_coordinate GENERATION__create__peak_coordinate(GENERATION__peak_x x, GENERATION__peak_z z) {
    GENERATION__peak_coordinate output;

    // setup output
    output.p_x = x;
    output.p_z = z;

    return output;
}

// create null peak
GENERATION__peak_coordinate GENERATION__create_null__peak_coordinate() {
    // return empty
    return GENERATION__create__peak_coordinate(0, 0);
}

// terrain generation structure
typedef struct GENERATION__terrain {
    // heights
    GENERATION__ocean_height p_ocean_height; // the height at which open terrain will be water
    GENERATION__terrain_height__tallness p_tallest_terrain_height; // the highest height (towards -Y (up)) that terrain can generate to (not including structures)
    GENERATION__terrain_height__shortness p_shortest_terrain_height; // the shortest height (towards Y (down)) that terrain can generate to (not including structures)
} GENERATION__terrain;

// create custom terrain
GENERATION__terrain GENERATION__create__terrain(GENERATION__ocean_height ocean_height, GENERATION__terrain_height__tallness terrain_maximum_tallness, GENERATION__terrain_height__shortness terrain_minimum_shortness) {
    GENERATION__terrain output;

    // setup output
    output.p_ocean_height = ocean_height;
    output.p_tallest_terrain_height = terrain_maximum_tallness;
    output.p_shortest_terrain_height = terrain_minimum_shortness;

    return output;
}

// create null terrain
GENERATION__terrain GENERATION__create_null__terrain() {
    // return empty
    return GENERATION__create__terrain(GENERATION__define__null_height, GENERATION__define__null_height, GENERATION__define__null_height);
}

/* Blueprint */
// the blueprint for the world generation
typedef struct GENERATION__blueprint {
    // random context
    RANDOM__context p_random_context;

    // world wide seed
    GENERATION__seed p_master_seed;

    // terrain seeds (derived from master seed)
    GENERATION__seed p_terrain_x_seed;
    GENERATION__seed p_terrain_z_seed;

    // structure seed
    GENERATION__seed p_structure_seed;

    // terrain data
    GENERATION__terrain p_terrain;
} GENERATION__blueprint;

// blueprint address
typedef GENERATION__blueprint* GENERATION__blueprint_address;

// create custom blueprint
GENERATION__blueprint GENERATION__create__blueprint(RANDOM__context random_context, GENERATION__seed master_seed, GENERATION__seed terrain_x_seed, GENERATION__seed terrain_z_seed, GENERATION__seed structure_seed, GENERATION__terrain terrain) {
    GENERATION__blueprint output;

    // setup output
    output.p_random_context = random_context;
    output.p_master_seed = master_seed;
    output.p_terrain_x_seed = terrain_x_seed;
    output.p_terrain_z_seed = terrain_z_seed;
    output.p_structure_seed = structure_seed;
    output.p_terrain = terrain;

    return output;
}

// create null blueprint
GENERATION__blueprint GENERATION__create_null__blueprint() {
    // return empty
    return GENERATION__create__blueprint(RANDOM__create_null__context(), GENERATION__define__null_seed, GENERATION__define__null_seed, GENERATION__define__null_seed, GENERATION__define__null_seed, GENERATION__create_null__terrain());
}

// open blueprint from seed
GENERATION__blueprint GENERATION__open__blueprint(GENERATION__seed master_seed, GENERATION__terrain terrain) {
    GENERATION__blueprint output;

    // setup seeds
    output.p_master_seed = master_seed;
    output.p_terrain_x_seed = output.p_master_seed - 2;
    output.p_terrain_z_seed = output.p_master_seed - 3;
    output.p_structure_seed = output.p_master_seed - 4;

    // setup random context
    output.p_random_context = RANDOM__create__context(output.p_master_seed - 1, 0);

    // setup terrain
    //GENERATION__height max_min_height = ESS__calculate__chunk_box_size_in_world_coordinates().p_y * 2;
    //output.p_terrain = GENERATION__create__terrain(ESS__calculate__world_center().p_y, ESS__calculate__world_center().p_y + max_min_height, ESS__calculate__world_center().p_y - max_min_height);
    output.p_terrain = terrain;

    return output;
}

/* Terrain Height */
// calculate cosine interpolation
f32 GENERATION__calculate__cosine_interpolation__floating_point(f32 point_a, f32 point_b, f32 in_between) {
    // do math for height
    f32 modified_in_between = (1 - cosf(in_between * BASIC__define__pi)) * 0.5;

    // return interpolation
    return (point_a * (1.0f - modified_in_between)) + (point_b * modified_in_between); // dont forget adjustment!
}

// calculate regular interpolation
GENERATION__height GENERATION__calculate__terrain_height_interpolation(GENERATION__peak_axis range_start, GENERATION__peak_axis range_end, BASIC__integer_fraction in_between) {
    // return interpolation
    return ((range_start * (in_between.p_bottom - in_between.p_top)) + (range_end * in_between.p_top)) / in_between.p_bottom + range_end;
}

// calculate regular interpolation
GENERATION__height GENERATION__calculate__terrain_height_interpolation__2(GENERATION__peak_axis range_start, GENERATION__peak_axis range_end, BASIC__integer_fraction in_between) {
    // calculate higher value
    if (range_start > range_end) {
        return (((range_start - range_end) * in_between.p_top) / in_between.p_bottom) + range_end;
    } else if (range_end > range_start) {
        return (((range_end - range_start) * in_between.p_top) / in_between.p_bottom) + range_start;
    } else {
        // equal, just return one of them
        return range_start;
    }
}

// calculate peak from coords (peaks are always the starting point of a chunk coord's xz values)
GENERATION__peak_coordinate GENERATION__calculate__nearest_peak_from_coordinate__always_round_down(ESS__world_vertex coordinate, GENERATION__peak_coordinate peak_offsets) {
    GENERATION__peak_coordinate output;

    // calculate peak
    output.p_x = (coordinate.p_x / GENERATION__define__peak_spread_distance) + (peak_offsets.p_x * GENERATION__define__peak_spread_distance);
    output.p_z = (coordinate.p_z / GENERATION__define__peak_spread_distance) + (peak_offsets.p_z * GENERATION__define__peak_spread_distance);

    return output;
}

// calculate peak height from peak coordinates
GENERATION__terrain_height GENERATION__calculate__peak_coordinate_height__one_axis(GENERATION__blueprint_address blueprint, GENERATION__peak_axis peak_number) {
    return ((RANDOM__generate_number_and_advance__mark_1(&(*blueprint).p_random_context) * (*blueprint).p_terrain_x_seed * peak_number) % ((*blueprint).p_terrain.p_shortest_terrain_height - (*blueprint).p_terrain.p_tallest_terrain_height)) + (*blueprint).p_terrain.p_shortest_terrain_height;
}

// calculate terrain height from world x & z
GENERATION__terrain_height GENERATION__calculate__terrain_height(GENERATION__blueprint_address blueprint, ESS__world_vertex xz_point) {
    // get nearest peak coordinates
    GENERATION__peak_coordinate peak_x0z0 = GENERATION__calculate__nearest_peak_from_coordinate__always_round_down(xz_point, GENERATION__create__peak_coordinate(0, 0));
    GENERATION__peak_coordinate peak_x1z0 = GENERATION__calculate__nearest_peak_from_coordinate__always_round_down(xz_point, GENERATION__create__peak_coordinate(1, 0));
    GENERATION__peak_coordinate peak_x0z1 = GENERATION__calculate__nearest_peak_from_coordinate__always_round_down(xz_point, GENERATION__create__peak_coordinate(0, 1));
    GENERATION__peak_coordinate peak_x1z1 = GENERATION__calculate__nearest_peak_from_coordinate__always_round_down(xz_point, GENERATION__create__peak_coordinate(1, 1));

    // calculate heights of coordinates
    GENERATION__terrain_height peak_height_x0z0 = GENERATION__calculate__peak_coordinate_height__one_axis(blueprint, peak_x0z0.p_x);
    GENERATION__terrain_height peak_height_x1z0 = GENERATION__calculate__peak_coordinate_height__one_axis(blueprint, peak_x1z0.p_x);

    // return middle height of one coordinate
    return GENERATION__calculate__terrain_height_interpolation__2(peak_height_x0z0, peak_height_x1z0, BASIC__create__integer_fraction(peak_height_x1z0 - peak_height_x0z0, (*blueprint).p_terrain.p_shortest_terrain_height - (*blueprint).p_terrain.p_tallest_terrain_height));
}

/*// calculate the height of terrain given
GENERATION__terrain_height GENERATION__calculate__terrain_height(GENERATION__blueprint_address blueprint, ESS__world_vertex xz_point) {
    return (RANDOM__generate_number__mark_1(&(*blueprint).p_random_context, xz_point.p_x) % ((*blueprint).p_terrain.p_tallest_terrain_height - (*blueprint).p_terrain.p_shortest_terrain_height)) + (*blueprint).p_terrain.p_shortest_terrain_height;
}*/

/* Generation Function Pointer */
// function pointer that calls a chunk generation function
typedef CHUNK__chunk (*GENERATION__function_address__generate_chunk)(ESS__world_vertex, GENERATION__blueprint_address);

#endif
