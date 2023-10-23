#ifndef VOXELIZE__render
#define VOXELIZE__render

/* Include */
// game internals
#include "../base/essentials.h"
#include "game_window.h"
#include "shaders.h"
#include "textures.h"
#include "skin.h"
#include "../storage/chunks.h"
#include "../storage/chunk_surfaces.h"
#include "../storage/positions.h"

// drawing dependencies
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <math.h>

/* Define */
// vertex size information
typedef f32 RENDER__axis;
typedef RENDER__axis RENDER__x;
typedef RENDER__axis RENDER__y;
typedef RENDER__axis RENDER__z;

// chunk vbo data
typedef u8 RENDER__compact_axis;
typedef RENDER__compact_axis RENDER__compact_x; // chunk relative vertex offset
typedef RENDER__compact_axis RENDER__compact_y; // chunk relative vertex offset
typedef RENDER__compact_axis RENDER__compact_z; // chunk relative vertex offset
typedef u8 RENDER__compact_texture_xy; // texture sub indices
typedef TEX__face_count RENDER__compact_texture_z; // texture number / 2d texture array texture selector

// chunk vbo organizers
typedef u32 RENDER__compact_vertex_half;
typedef u64 RENDER__compact_vertex;

// chunk vbo data indexing
typedef u32 RENDER__vertex_index; // index into chunk's vertex array
typedef RENDER__vertex_index RENDER__vertex_count; // vertex count

// chunk indexing
typedef CHUNK__chunks_index RENDER__object_index;
typedef RENDER__object_index RENDER__object_count;
typedef RENDER__object_index RENDER__object_axis;

// object sizes
typedef ESS__dimension_x RENDER__object_x;
typedef ESS__dimension_y RENDER__object_y;
typedef ESS__dimension_z RENDER__object_z;

// chunk vertex statistics
#define RENDER__chunk_side_vertex_count 129 // ((256 / 2) + 1) <- Because doing 0 - 256 would require 9 bits, the detail is cut in half to 0 -> 128 || 129 || 0 -> (16 * 8)
#define RENDER__chunk_slice_vertex_count (RENDER__chunk_side_vertex_count*RENDER__chunk_side_vertex_count)
#define RENDER__chunk_total_vertex_count (RENDER__chunk_slice_vertex_count*RENDER__chunk_side_vertex_count)
#define RENDER__chunk_total_vertex_size (RENDER__chunk_total_vertex_count*sizeof(RENDER__compact_vertex))
#define RENDER__vertices_per_block_strip ((RENDER__chunk_side_vertex_count-1)/ESS__define__chunk_side_block_count) // the distance from one block to the next

// chunk draw type information
#define RENDER__chunk_draw_type GL_ARRAY_BUFFER

/* Graphical Positions */
// three point vertex
typedef struct RENDER__vertex {
    vec3 p_vertices;
} RENDER__vertex;

// create custom vertex
RENDER__vertex RENDER__create__vertex(RENDER__x x, RENDER__y y, RENDER__z z) {
    RENDER__vertex output;

    // setup output
    output.p_vertices[0] = x;
    output.p_vertices[1] = y;
    output.p_vertices[2] = z;

    return output;
}

// create null vertex
RENDER__vertex RENDER__create_null__vertex() {
    // return null vertex
    return RENDER__create__vertex(0.0f, 0.0f, 0.0f);
}

// information necessary to hold an object in space
typedef struct RENDER__transform {
    RENDER__vertex p_position_offset;
    RENDER__vertex p_rotation;
} RENDER__transform;

// create custom transform
RENDER__transform RENDER__create__transform(RENDER__vertex position_offset, RENDER__vertex rotation) {
    RENDER__transform output;

    // setup output
    output.p_position_offset = position_offset;
    output.p_rotation = rotation;

    return output;
}

// create null transform
RENDER__transform RENDER__create_null__transform() {
    // return empty transform
    return RENDER__create__transform(RENDER__create_null__vertex(), RENDER__create_null__vertex());
}

/* Matrix */
// matrix wrapper struct
typedef struct RENDER__matrix_f32 {
    mat4 p_matrix;
} RENDER__matrix_f32;

// create custom matrix
RENDER__matrix_f32 RENDER__create__matrix_f32(mat4 matrix) {
    RENDER__matrix_f32 output;

    // create matrix
    for (u64 i = 0; i < 4; i++) {
        for (u64 j = 0; j < 4; j++) {
            output.p_matrix[i][j] = matrix[i][j];
        }
    }

    return output;
}

/* Rendering Temporaries */
// temporaries
typedef struct RENDER__temporaries {
    BASIC__buffer p_vertices;
} RENDER__temporaries;

// create temporaries
RENDER__temporaries RENDER__open__temporaries() {
    RENDER__temporaries output;

    // setup output
    output.p_vertices = BASIC__open__buffer(RENDER__chunk_total_vertex_size);

    return output;
}

// destroy temporaries
void RENDER__close__temporaries(RENDER__temporaries temporaries) {
    // free buffers
    BASIC__close__buffer(temporaries.p_vertices);

    return;
}

// clear temporaries
void RENDER__clear__temporaries(RENDER__temporaries temporaries) {
    for (u64 i = 0; i < RENDER__chunk_total_vertex_size; i++) {
        ((u8*)temporaries.p_vertices.p_address)[i] = 0;
    }
}

/* The Visible World */
// object type
typedef enum RENDER__ot {
    RENDER__ot__chunk_body,
    RENDER__ot__chunk_XY_surface,
    RENDER__ot__chunk_YZ_surface,
    RENDER__ot__chunk_XZ_surface,
} RENDER__ot;

// an object's opengl, transform and vertex count information
typedef struct RENDER__object_handle {
    RENDER__vertex_count p_vertex_count;
    GLuint p_vao;
    GLuint p_vbo;
} RENDER__object_handle;

// chunk handle addressing
typedef RENDER__object_handle* RENDER__object_handle_address;

// create custom chunk handle
RENDER__object_handle RENDER__create__object_handle(RENDER__vertex_count vertex_count, GLuint vao, GLuint vbo) {
    RENDER__object_handle output;

    // setup output
    output.p_vertex_count = vertex_count;
    output.p_vao = vao;
    output.p_vbo = vbo;

    return output;
}

// create null handle
RENDER__object_handle RENDER__create_null__handle() {
    // return empty
    return RENDER__create__object_handle(0, 0, 0);
}

// everything rendered in the game
typedef struct RENDER__world {
    // opengl vbo handles
    BASIC__buffer p_all_handles; // the main allocation for all handles

    // sub allocations for vbo handles
    BASIC__buffer p_chunk_body_handles;
    BASIC__buffer p_chunk_XY_surface_handles;
    BASIC__buffer p_chunk_YZ_surface_handles;
    BASIC__buffer p_chunk_XZ_surface_handles;

    // counts
    RENDER__object_count p_handle_total_count;
    RENDER__object_count p_chunk_bodies_count;
    RENDER__object_count p_chunk_XY_surfaces_count;
    RENDER__object_count p_chunk_YZ_surfaces_count;
    RENDER__object_count p_chunk_XZ_surfaces_count;
} RENDER__world;

// close the visible world
void RENDER__close__world(RENDER__world world) {
    // close handles
    for (RENDER__object_index i = 0; i < world.p_handle_total_count; i++) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_all_handles.p_address)[i].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_all_handles.p_address)[i].p_vao));
    }

    // close buffers
    BASIC__close__buffer(world.p_all_handles);

    return;
}

// setup a sub allocation
BASIC__buffer RENDER__calculate__allocation_sub_handle(BASIC__buffer original_allocation, u64 handle_offset, u64 handle_count) {
    BASIC__buffer output;

    // setup output
    output.p_address = original_allocation.p_address + (sizeof(RENDER__object_handle) * handle_offset);
    output.p_length = sizeof(RENDER__object_handle) * handle_count;

    return output;
}

// open visible world
RENDER__world RENDER__open__world(CHUNK__chunks chunks) {
    RENDER__world output;

    // setup counts
    output.p_chunk_bodies_count = (RENDER__object_count)CHUNK__calculate__chunks_count(chunks.p_dimensions);
    output.p_chunk_XY_surfaces_count = (RENDER__object_count)CHUNKSURFACE__calculate__XY_surfaces_volume(chunks.p_dimensions);
    output.p_chunk_YZ_surfaces_count = (RENDER__object_count)CHUNKSURFACE__calculate__YZ_surfaces_volume(chunks.p_dimensions);
    output.p_chunk_XZ_surfaces_count = (RENDER__object_count)CHUNKSURFACE__calculate__XZ_surfaces_volume(chunks.p_dimensions);

    // setup total count
    output.p_handle_total_count = output.p_chunk_bodies_count + output.p_chunk_XY_surfaces_count + output.p_chunk_YZ_surfaces_count + output.p_chunk_XZ_surfaces_count;

    // allocate chunk handles
    output.p_all_handles = BASIC__open__buffer(output.p_handle_total_count * sizeof(RENDER__object_handle));

    // setup sub allocated handles
    output.p_chunk_body_handles = RENDER__calculate__allocation_sub_handle(output.p_all_handles, 0, output.p_chunk_bodies_count);
    output.p_chunk_XY_surface_handles = RENDER__calculate__allocation_sub_handle(output.p_all_handles, output.p_chunk_bodies_count, output.p_chunk_XY_surfaces_count);
    output.p_chunk_YZ_surface_handles = RENDER__calculate__allocation_sub_handle(output.p_all_handles, output.p_chunk_bodies_count + output.p_chunk_XY_surfaces_count, output.p_chunk_YZ_surfaces_count);
    output.p_chunk_XZ_surface_handles = RENDER__calculate__allocation_sub_handle(output.p_all_handles, output.p_chunk_bodies_count + output.p_chunk_XY_surfaces_count + output.p_chunk_YZ_surfaces_count, output.p_chunk_XZ_surfaces_count);

    // setup blank handles
    for (RENDER__object_index i = 0; i < output.p_handle_total_count; i++) {
        // create blank handle
        ((RENDER__object_handle*)output.p_all_handles.p_address)[i] = RENDER__create_null__handle();
    }

    return output;
}

/* Render The World */
// assemble one vertex
RENDER__compact_vertex RENDER__render__one_vertex(RENDER__compact_x x, RENDER__compact_y y, RENDER__compact_z z, RENDER__compact_texture_xy texture_x, RENDER__compact_texture_xy texture_y, RENDER__compact_texture_z texture_z) {
    RENDER__compact_vertex output;

    // null initialize output
    output = 0;

    // setup reserved space & texture z
    output <<= 16; // split into 16 twice because of intel bitshifting limitations
    output <<= 16;
    output += (RENDER__compact_vertex)texture_z;

    // setup texture x and texture y
    output <<= 8;
    output += (RENDER__compact_vertex)((texture_y << 4) + texture_x);

    // setup texture z
    output <<= 8;
    output += (RENDER__compact_vertex)z;
    
    // setup texture y
    output <<= 8;
    output += (RENDER__compact_vertex)y;
    
    // setup texture x
    output <<= 8;
    output += (RENDER__compact_vertex)x;
    
    return output;
}

// render vertex and setup to render next vertex
void RENDER__render_and_next__compact_vertex(RENDER__vertex_index* index, RENDER__compact_vertex* output, RENDER__compact_vertex vertex) {
    // write data
    output[*index] = vertex;

    // advance index
    *index += 1;

    return;
}

// draw left & right face
RENDER__vertex_index RENDER__render__X_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z + offset_size, texture_max, texture_max, texture_z));

    return vertex_index;
}

// draw top & bottom face
RENDER__vertex_index RENDER__render__Y_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z + offset_size, texture_max, texture_max, texture_z));

    return vertex_index;
}

// draw back & front face
RENDER__vertex_index RENDER__render__Z_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y + offset_size, z, texture_max, texture_max, texture_z));

    return vertex_index;
}

// create the vertices for the chunk insides and send it to opengl
void RENDER__render__chunk_body(SKIN__skins skins, CHUNK__chunk_address chunk_address, RENDER__object_index handle_index, RENDER__world world, RENDER__temporaries temps) {
    RENDER__vertex_index vertex_index;
    CHUNK__block_ID block_center_type;
    CHUNK__block_ID block_side_type;
    RENDER__x chunk_x;
    RENDER__y chunk_y;
    RENDER__z chunk_z;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk data index
    vertex_index = 0;

    // for each chunk slice
    for (CHUNK__chunk_axis_index chunk_slice = 0; chunk_slice < ESS__define__chunk_side_block_count; chunk_slice++) { // Z
        // for each chunk strip
        for (CHUNK__chunk_axis_index chunk_strip = 0; chunk_strip < ESS__define__chunk_side_block_count; chunk_strip++) { // Y
            // for each block
            for (CHUNK__chunk_axis_index chunk_block = 0; chunk_block < ESS__define__chunk_side_block_count; chunk_block++) { // X
                // get center block type
                block_center_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block, chunk_strip, chunk_slice)));

                // try draw left face
                // check if face is on outside (if so, do not draw)
                if (chunk_block > 0) {
                    // get block on left side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block - 1, chunk_strip, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__X_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__left]);
                    }
                }

                // try draw right face
                // check if face is on outside (if so, do not draw)
                if (chunk_block < ESS__define__chunk_side_block_count - 1) {
                    // get block on right side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block + 1, chunk_strip, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block + 1) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__X_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__right]);
                    }
                }

                // try draw top face
                // check if face is on outside (if so, do not draw)
                if (chunk_strip < ESS__define__chunk_side_block_count - 1) {
                    // get block on top side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block, chunk_strip + 1, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip + 1) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Y_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__top]);
                    }
                }

                // try draw bottom face
                // check if face is on outside (if so, do not draw)
                if (chunk_strip > 0) {
                    // get block on bottom side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block, chunk_strip - 1, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Y_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__bottom]);
                    }
                }
                
                // try draw back face
                // check if face is on outside (if so, do not draw)
                if (chunk_slice < ESS__define__chunk_side_block_count - 1) {
                    // get block on back side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block, chunk_strip, chunk_slice + 1)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice + 1) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Z_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__back]);
                    }
                }

                // try draw front face
                // check if face is on outside (if so, do not draw)
                if (chunk_slice > 0) {
                    // get block on front side
                    block_side_type = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(chunk_address, CHUNK__calculate__block_index(chunk_block, chunk_strip, chunk_slice - 1)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Z_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__front]);
                    }
                }
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// check if a side can be rendered
BASIC__bt RENDER__calculate__chunk_surface_face_is_renderable(SKIN__bdt center_block_draw_type, SKIN__bdt outside_block_draw_type) {
    return (BASIC__bt)(center_block_draw_type == SKIN__bdt__draw_all_sides || (center_block_draw_type == SKIN__bdt__draw_only_one_side && outside_block_draw_type == SKIN__bdt__draw_all_sides) || (center_block_draw_type > SKIN__bdt__dont_draw && outside_block_draw_type == SKIN__bdt__dont_draw));
}

// render a chunk XY surface
void RENDER__render__chunk_XY_surface(SKIN__skins skins, CHUNK__chunks chunks, RENDER__object_index handle_index, CHUNK__chunks_index center_chunks_index, CHUNK__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    CHUNK__chunk_address center_chunk_address;
    CHUNK__chunk_address outside_chunk_address;
    CHUNK__block_ID center_block_ID;
    CHUNK__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each x axis
    for (RENDER__object_index block_x = 0; block_x < ESS__define__chunk_side_block_count; block_x++) {
        // for each y axis
        for (RENDER__object_index block_y = 0; block_y < ESS__define__chunk_side_block_count; block_y++) {
            // get block data
            center_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(center_chunk_address, CHUNK__calculate__block_index(block_x, block_y, ESS__define__chunk_side_block_count - 1)));
            outside_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(outside_chunk_address, CHUNK__calculate__block_index(block_x, block_y, 0)));
            
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = 0;
                
                // draw face
                vertex_index = RENDER__render__Z_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__front]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = 0;
                
                // draw face
                vertex_index = RENDER__render__Z_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__back]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render a chunk YZ surface
void RENDER__render__chunk_YZ_surface(SKIN__skins skins, CHUNK__chunks chunks, RENDER__object_index handle_index, CHUNK__chunks_index center_chunks_index, CHUNK__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    CHUNK__chunk_address center_chunk_address;
    CHUNK__chunk_address outside_chunk_address;
    CHUNK__block_ID center_block_ID;
    CHUNK__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each y axis
    for (RENDER__object_index block_y = 0; block_y < ESS__define__chunk_side_block_count; block_y++) {
        // for each z axis
        for (RENDER__object_index block_z = 0; block_z < ESS__define__chunk_side_block_count; block_z++) {
            // get block data
            center_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(center_chunk_address, CHUNK__calculate__block_index(ESS__define__chunk_side_block_count - 1, block_y, block_z)));
            outside_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(outside_chunk_address, CHUNK__calculate__block_index(0, block_y, block_z)));
        
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = 0;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__X_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__left]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = 0;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__X_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__right]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render a chunk XZ surface
void RENDER__render__chunk_XZ_surface(SKIN__skins skins, CHUNK__chunks chunks, RENDER__object_index handle_index, CHUNK__chunks_index center_chunks_index, CHUNK__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    CHUNK__chunk_address center_chunk_address;
    CHUNK__chunk_address outside_chunk_address;
    CHUNK__block_ID center_block_ID;
    CHUNK__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = CHUNK__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each x axis
    for (RENDER__object_index block_x = 0; block_x < ESS__define__chunk_side_block_count; block_x++) {
        // for each y axis
        for (RENDER__object_index block_z = 0; block_z < ESS__define__chunk_side_block_count; block_z++) {
            // get block data
            center_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(center_chunk_address, CHUNK__calculate__block_index(block_x, ESS__define__chunk_side_block_count - 1, block_z)));
            outside_block_ID = CHUNK__get__block_ID_from_block_data(CHUNK__get__block_data_from_chunk_address(outside_chunk_address, CHUNK__calculate__block_index(block_x, 0, block_z)));
        
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = 0;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__Y_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__top]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = 0;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__Y_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__bottom]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render everything in the world
void RENDER__render__world(SKIN__skins skins, CHUNK__chunks chunks, POS__positions positions, RENDER__world world, RENDER__temporaries temps) {
    // render each chunk body
    for (RENDER__object_index handle_ID = 0; handle_ID < world.p_chunk_bodies_count; handle_ID++) {
        // render one chunk
        RENDER__render__chunk_body(skins, CHUNK__get__chunk_pointer_in_chunks(chunks, handle_ID), handle_ID, world, temps);
    }

    // render each chunk XY surface
    for (RENDER__object_index x = 0; x < positions.p_chunk_XY_surface_dimensions.p_width; x++) {
        for (RENDER__object_index y = 0; y < positions.p_chunk_XY_surface_dimensions.p_height; y++) {
            for (RENDER__object_index z = 0; z < positions.p_chunk_XY_surface_dimensions.p_depth; z++) {
                // render one surface
                RENDER__render__chunk_XY_surface(skins, chunks, ESS__calculate__dimensions_index(positions.p_chunk_XY_surface_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x, y, z + 1), world, temps);
            }
        }
    }

    // render each chunk YZ surface
    for (RENDER__object_index x = 0; x < positions.p_chunk_YZ_surface_dimensions.p_width; x++) {
        for (RENDER__object_index y = 0; y < positions.p_chunk_YZ_surface_dimensions.p_height; y++) {
            for (RENDER__object_index z = 0; z < positions.p_chunk_YZ_surface_dimensions.p_depth; z++) {
                // render one surface
                RENDER__render__chunk_YZ_surface(skins, chunks, ESS__calculate__dimensions_index(positions.p_chunk_YZ_surface_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x + 1, y, z), world, temps);
            }
        }
    }

    // render each chunk YZ surface
    for (RENDER__object_index x = 0; x < positions.p_chunk_XZ_surface_dimensions.p_width; x++) {
        for (RENDER__object_index y = 0; y < positions.p_chunk_XZ_surface_dimensions.p_height; y++) {
            for (RENDER__object_index z = 0; z < positions.p_chunk_XZ_surface_dimensions.p_depth; z++) {
                // render one surface
                RENDER__render__chunk_XZ_surface(skins, chunks, ESS__calculate__dimensions_index(positions.p_chunk_XZ_surface_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x, y, z), ESS__calculate__dimensions_index(positions.p_chunk_body_dimensions, x, y + 1, z), world, temps);
            }
        }
    }

    return;
}

/* Draw World */
// calculate the difference between the two positions
RENDER__axis RENDER__calculate__render_axis(ESS__world_axis camera_axis, ESS__world_axis object_axis) {
    if (camera_axis >= object_axis) {
        return ((f32)(camera_axis - object_axis)) / ((f32)ESS__define__bits_per_block__total_count);
    } else {
        return (((f32)(object_axis - camera_axis)) / ((f32)ESS__define__bits_per_block__total_count)) * -1.0f;
    }
}

// calculate the floating point offset from the camera position and the object position
RENDER__vertex RENDER__calculate__camera_position_offset(ESS__world_vertex camera_position, ESS__world_vertex object_position) {
    RENDER__vertex output;

    // setup output
    output.p_vertices[0] = RENDER__calculate__render_axis(camera_position.p_x, object_position.p_x);
    output.p_vertices[1] = RENDER__calculate__render_axis(camera_position.p_y, object_position.p_y);
    output.p_vertices[2] = RENDER__calculate__render_axis(camera_position.p_z, object_position.p_z);

    return output;
}

// send transform matrix to GPU
void RENDER__send__transform_matrix__opengl(GLuint program_ID, mat4* transform) {
    // send data
    glUniformMatrix4fv(glGetUniformLocation(program_ID, "GLOBAL_transform"), 1, GL_FALSE, (RENDER__axis*)transform);

    return;
}

// send transform matrix to opengl
void RENDER__send__transform_matrix__voxelize(SHADER__program shader_program, RENDER__matrix_f32 transform) {
    // send data
    RENDER__send__transform_matrix__opengl(shader_program.p_program_ID, &(transform.p_matrix));

    return;
}

/*// update object transform and render to opengl
void RENDER__calculate_and_send__transform_matrix(RENDER__object_handle_address handle_address, WINDOW__window_configuration window, SHADER__program shader_program, RENDER__transform object_transform, RENDER__vertex camera_rotation) {
    mat4 final_transform;
    mat4 perspective;
    mat4 view;
    mat4 model;

    // calculation values
    vec3 camera_position = {0.0f, 0.0f, 0.0f};
    vec3 camera_target = {0.0f, 0.0f, 2.0f};
    vec3 camera_up = {0.0f, 1.0f, 0.0f};
    vec3 rotate_x = {1.0f, 0.0f, 0.0f};
    vec3 rotate_y = {0.0f, 1.0f, 0.0f};
    vec3 rotate_z = {0.0f, 0.0f, 1.0f};
    vec3 object_movement;
    vec3 object_rotation;

    // update world object transform information
    (*handle_address).p_transform = RENDER__calculate__add_object_transforms((*handle_address).p_transform, object_transform);

    // setup temps
    object_movement[0] = (*handle_address).p_transform.p_position_offset.p_vertices[0];
    object_movement[1] = (*handle_address).p_transform.p_position_offset.p_vertices[1];
    object_movement[2] = (*handle_address).p_transform.p_position_offset.p_vertices[2];
    object_rotation[0] = (*handle_address).p_transform.p_rotation.p_vertices[0];
    object_rotation[1] = (*handle_address).p_transform.p_rotation.p_vertices[1];
    object_rotation[2] = (*handle_address).p_transform.p_rotation.p_vertices[2];

    // setup perspective
    glm_perspective(glm_rad(45.0f), ((float)window.p_width) / ((float)window.p_height), ESS__define__near_plane, ESS__define__far_plane, perspective);

    // setup view
    glm_lookat(camera_position, camera_target, camera_up, view);

    // setup model
    glm_mat4_identity(model);
    //glm_rotate_at(model, camera_position, object_rotation[0], rotate_x); // world object rotations
    //glm_rotate_at(model, camera_position, object_rotation[1], rotate_y);
    //glm_rotate_at(model, camera_position, object_rotation[2], rotate_z);
    glm_translate(model, object_movement);
    glm_rotate_at(model, camera_position, camera_rotation.p_vertices[0], rotate_x); // player camera rotations
    glm_rotate_at(model, camera_position, camera_rotation.p_vertices[1], rotate_y);

    // create final transform
    glm_mat4_mul(perspective, view, final_transform);
    glm_mat4_mul(final_transform, model, final_transform);

    // send final transform to opengl shaders
    RENDER__send__transform_matrix(shader_program.p_program_ID, &final_transform);

    return;
}*/

RENDER__matrix_f32 RENDER__calculate__transform_matrix(WINDOW__window_configuration window, RENDER__vertex camera_rotation, ESS__world_vertex camera_world_position, ESS__world_vertex object_world_position) {
    RENDER__matrix_f32 output; // final transform
    mat4 perspective;
    mat4 view;
    mat4 player_camera_rotation_matrix;
    mat4 model;

    // calculation values
    vec3 camera_position = {0.0f, 0.0f, 0.0f};
    vec3 camera_target = {0.0f, 0.0f, 2.0f};
    vec3 camera_up = {0.0f, 1.0f, 0.0f};
    vec3 rotate_x = {1.0f, 0.0f, 0.0f};
    vec3 rotate_y = {0.0f, 1.0f, 0.0f};
    RENDER__vertex camera_movement;
    RENDER__axis camera_yaw;
    RENDER__axis camera_pitch;

    // setup camera rotation
    camera_yaw = glm_rad(camera_rotation.p_vertices[0]);
    camera_pitch = glm_rad(camera_rotation.p_vertices[1]);

    // setup camera offset
    camera_movement = RENDER__calculate__camera_position_offset(camera_world_position, object_world_position);

    // setup perspective
    glm_perspective(glm_rad(45.0f), ((float)window.p_width) / ((float)window.p_height), ESS__define__near_plane, ESS__define__far_plane, perspective);

    // setup view
    glm_lookat(camera_position, camera_target, camera_up, view);

    // setup player rotation
    glm_mat4_identity(player_camera_rotation_matrix);
    glm_rotate_at(player_camera_rotation_matrix, camera_position, camera_yaw, rotate_x);
    glm_rotate_at(player_camera_rotation_matrix, camera_position, camera_pitch, rotate_y);

    // setup model
    glm_mat4_identity(model);
    glm_translate(model, camera_movement.p_vertices);

    // create final transform
    glm_mat4_mul(perspective, view, output.p_matrix);
    glm_mat4_mul(output.p_matrix, player_camera_rotation_matrix, output.p_matrix);
    glm_mat4_mul(output.p_matrix, model, output.p_matrix);

    return output;
}

// draw everything
void RENDER__draw__world(TEX__game_textures game_textures, RENDER__world world, WINDOW__window_configuration window, SHADER__program shader_program, POS__positions positions, RENDER__vertex camera_rotation) {
    RENDER__matrix_f32 final_transform;

    // bind proper textures
    TEX__bind__game_textures__specific(game_textures, TEX__gtt__block_faces, shader_program);

    // draw all block handles
    for (RENDER__object_index i = 0; i < world.p_chunk_bodies_count; i++) {
        // create and send matrix to opengl
        final_transform = RENDER__calculate__transform_matrix(window, camera_rotation, positions.p_camera_position, ((ESS__world_vertex*)positions.p_chunk_body_positions.p_address)[i]);
        RENDER__send__transform_matrix__voxelize(shader_program, final_transform);

        // bind chunk
        glBindVertexArray(((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[i].p_vao);
        glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[i].p_vbo);

        // draw chunk
        glDrawArrays(GL_TRIANGLES, 0, ((RENDER__object_handle*)world.p_chunk_body_handles.p_address)[i].p_vertex_count);
    }

    // render all chunk XY surfaces
    for (RENDER__object_index i = 0; i < world.p_chunk_XY_surfaces_count; i++) {
        // create and send matrix to opengl
        final_transform = RENDER__calculate__transform_matrix(window, camera_rotation, positions.p_camera_position, ((ESS__world_vertex*)positions.p_chunk_XY_surface_positions.p_address)[i]);
        RENDER__send__transform_matrix__voxelize(shader_program, final_transform);

        // bind chunk
        glBindVertexArray(((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[i].p_vao);
        glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[i].p_vbo);

        // draw chunk
        glDrawArrays(GL_TRIANGLES, 0, ((RENDER__object_handle*)world.p_chunk_XY_surface_handles.p_address)[i].p_vertex_count);
    }

    // render all chunk YZ surfaces
    for (RENDER__object_index i = 0; i < world.p_chunk_YZ_surfaces_count; i++) {
        // create and send matrix to opengl
        final_transform = RENDER__calculate__transform_matrix(window, camera_rotation, positions.p_camera_position, ((ESS__world_vertex*)positions.p_chunk_YZ_surface_positions.p_address)[i]);
        RENDER__send__transform_matrix__voxelize(shader_program, final_transform);

        // bind chunk
        glBindVertexArray(((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[i].p_vao);
        glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[i].p_vbo);

        // draw chunk
        glDrawArrays(GL_TRIANGLES, 0, ((RENDER__object_handle*)world.p_chunk_YZ_surface_handles.p_address)[i].p_vertex_count);
    }

    // render all chunk XZ surfaces
    for (RENDER__object_index i = 0; i < world.p_chunk_XZ_surfaces_count; i++) {
        // create and send matrix to opengl
        final_transform = RENDER__calculate__transform_matrix(window, camera_rotation, positions.p_camera_position, ((ESS__world_vertex*)positions.p_chunk_XZ_surface_positions.p_address)[i]);
        RENDER__send__transform_matrix__voxelize(shader_program, final_transform);

        // bind chunk
        glBindVertexArray(((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[i].p_vao);
        glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[i].p_vbo);

        // draw chunk
        glDrawArrays(GL_TRIANGLES, 0, ((RENDER__object_handle*)world.p_chunk_XZ_surface_handles.p_address)[i].p_vertex_count);
    }

    return;
}

#endif
