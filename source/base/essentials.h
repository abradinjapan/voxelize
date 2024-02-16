#ifndef VOXELIZE__essentials
#define VOXELIZE__essentials

/* Include */
#include "basic.h"
#include "random.h"
#include "list.h"

/* Defines */
// the world axis defines are the actual game coordinates
// world axis defines
typedef u64 ESS__world_axis; // the global axis type
typedef ESS__world_axis ESS__world_x;
typedef ESS__world_axis ESS__world_y;
typedef ESS__world_axis ESS__world_z;

// the dimension defines are not for game dimensions, they are a generic xyz space size storage
// dimension defines
typedef u64 ESS__dimension_axis;
typedef ESS__dimension_axis ESS__dimension_x;
typedef ESS__dimension_axis ESS__dimension_y;
typedef ESS__dimension_axis ESS__dimension_z;
typedef ESS__dimension_axis ESS__dimensions_index;
typedef ESS__dimension_axis ESS__dimensions_volume;

// basics
#define ESS__define__bits_per_byte 8

/* Game Details */
// world positioning
#define ESS__define__bits_per_block 16 // The amount of bits that contain one block for an entities position axis
#define ESS__define__bits_per_block__range__start 0
#define ESS__define__bits_per_block__range__end ((1<<ESS__define__bits_per_block)-1)
#define ESS__define__bits_per_block__total_count (ESS__define__bits_per_block__range__end+1)
#define ESS__define__world_axis_middle ((ESS__world_axis)1<<((sizeof(ESS__world_axis)*ESS__define__bits_per_byte)-1)) // this is the value that is the middle of each axis (X Y Z)

// the size of one chunk dimension in blocks
#define ESS__define__chunk_side_block_count 16

// the size of two chunk dimensions in blocks
#define ESS__define__chunk_slice_block_count (ESS__define__chunk_side_block_count*ESS__define__chunk_side_block_count)

// the total amount of blocks in a chunk (all sides are always the same length)
#define ESS__define__chunk_total_block_count (ESS__define__chunk_side_block_count*ESS__define__chunk_slice_block_count)

// view limits
#define ESS__define__near_plane 0.01f
#define ESS__define__far_plane 10000.0f

/* World Vertices */
// a position in the game world
// world vertices
typedef struct ESS__world_vertex {
    ESS__world_x p_x;
    ESS__world_y p_y;
    ESS__world_z p_z;
} ESS__world_vertex;

// create world vertices
ESS__world_vertex ESS__create__world_vertex(ESS__world_x x, ESS__world_y y, ESS__world_z z) {
    ESS__world_vertex output;

    // setup output
    output.p_x = x;
    output.p_y = y;
    output.p_z = z;

    return output;
}

// create null world vertex
ESS__world_vertex ESS__create_null__world_vertex() {
    // return empty vertex
    return ESS__create__world_vertex(0, 0, 0);
}

// group add vertices
ESS__world_vertex ESS__calculate__add_world_vertices(ESS__world_vertex a, ESS__world_vertex b) {
    return ESS__create__world_vertex(a.p_x + b.p_x, a.p_y + b.p_y, a.p_z + b.p_z);
}

// group subtract vertices
ESS__world_vertex ESS__calculate__subtract_world_vertices(ESS__world_vertex a, ESS__world_vertex b) {
    return ESS__create__world_vertex(a.p_x - b.p_x, a.p_y - b.p_y, a.p_z - b.p_z);
}

/* Box */
// a datatype that lays out a three-dimensional box shaped area
typedef struct ESS__world_box {
    ESS__world_vertex p_right_up_front; // the point that lays to the right, up & front of the box (X, Y, Z)
    ESS__world_vertex p_left_down_back; // the point that lays to the left, down & back of the box (-X, -Y, -Z)
} ESS__world_box;

// create custom world box
ESS__world_box ESS__create__world_box(ESS__world_vertex right_up_front, ESS__world_vertex left_down_back) {
    ESS__world_box output;

    // setup output
    output.p_right_up_front = right_up_front;
    output.p_left_down_back = left_down_back;

    return output;
}

// create null world box
ESS__world_box ESS__create_null__world_box() {
    // return empty world box
    return ESS__create__world_box(ESS__create_null__world_vertex(), ESS__create_null__world_vertex());
}

// check to see if a coordinate is within a box (inclusive includes the edge cases)
BASIC__bt ESS__calculate__position_is_in_box__inclusive(ESS__world_box box, ESS__world_vertex position) {
    // return calculation
    return (box.p_left_down_back.p_x <= position.p_x && box.p_right_up_front.p_x >= position.p_x) && (box.p_left_down_back.p_y <= position.p_y && box.p_right_up_front.p_y >= position.p_y) && (box.p_left_down_back.p_z <= position.p_z && box.p_right_up_front.p_z >= position.p_z);
}

// check to see if a coordinate is within a box (exclusive does not include edge cases)
BASIC__bt ESS__calculate__position_is_in_box__exclusive(ESS__world_box box, ESS__world_vertex position) {
    // return calculation
    return (box.p_left_down_back.p_x < position.p_x && box.p_right_up_front.p_x > position.p_x) && (box.p_left_down_back.p_y < position.p_y && box.p_right_up_front.p_y > position.p_y) && (box.p_left_down_back.p_z < position.p_z && box.p_right_up_front.p_z > position.p_z);
}

/* Dimensions */
// integer dimensions of a 3d object
typedef struct ESS__dimensions {
    ESS__dimension_x p_width;
    ESS__dimension_y p_height;
    ESS__dimension_z p_depth;
} ESS__dimensions;

// create a dimension size
ESS__dimensions ESS__create__dimensions(ESS__dimension_x width, ESS__dimension_y height, ESS__dimension_z depth) {
    ESS__dimensions output;

    // setup output
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;
    
    return output;
}

// create a null dimension
ESS__dimensions ESS__create_null__dimensions() {
    // return empty
    return ESS__create__dimensions(0, 0, 0);
}

// calculate a dimension's array size
ESS__dimensions_volume ESS__calculate__dimensions_volume(ESS__dimensions dimensions) {
    return dimensions.p_width * dimensions.p_height * dimensions.p_depth;
}

// calculate a dimension's index
ESS__dimensions_index ESS__calculate__dimensions_index(ESS__dimensions dimensions, ESS__dimension_x x, ESS__dimension_y y, ESS__dimension_z z) {
    return (dimensions.p_height * dimensions.p_width * z) + (dimensions.p_width * y) + x;
}

/* Game Calculations */
// get the vertex for the center of the world
ESS__world_vertex ESS__calculate__world_center() {
    // return middle vertex
    return ESS__create__world_vertex(ESS__define__world_axis_middle, ESS__define__world_axis_middle, ESS__define__world_axis_middle);
}

// get the size of one block in world coordinates
ESS__world_axis ESS__calculate__block_size_in_world_coordinates() {
    // return value
    return (ESS__world_axis)ESS__define__bits_per_block__total_count;
}

// get the side length of one chunk in world cooridinates
ESS__world_axis ESS__calculate__chunk_side_size_in_world_coordinates() {
    return ESS__calculate__block_size_in_world_coordinates() * ESS__define__chunk_side_block_count;
}

// calculate the world vertex size of a chunk
ESS__world_vertex ESS__calculate__chunk_box_size_in_world_coordinates() {
    return ESS__create__world_vertex(ESS__calculate__chunk_side_size_in_world_coordinates(), ESS__calculate__chunk_side_size_in_world_coordinates(), ESS__calculate__chunk_side_size_in_world_coordinates());
}

// check to see if a coordinate is within a chunk
BASIC__bt ESS__calculate__coords_are_in_chunk(ESS__world_vertex chunk_position, ESS__world_vertex subject_position) {
    ESS__world_box chunk_box;

    // create box
    chunk_box = ESS__create__world_box(chunk_position, ESS__calculate__subtract_world_vertices(chunk_position, ESS__calculate__chunk_box_size_in_world_coordinates()));

    // return calculation
    return ESS__calculate__position_is_in_box__exclusive(chunk_box, subject_position);
}

#endif
