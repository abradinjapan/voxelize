#ifndef VOXELIZE__textures
#define VOXELIZE__textures

/* Include */
// base
#include "../base/essentials.h"

// shaders
#include "shaders.h"

/* Define */
// types
typedef u8 TEX__pixel_color;
typedef u16 TEX__dimension_length;
typedef u16 TEX__face_count;
typedef TEX__face_count TEX__face_index;
typedef u64 TEX__pixel_index;
typedef u64 TEX__block_face_ID;

// values
#define TEX__pixel_colors_count 4
#define TEX__block_faces__texture_unit_ID 0
#define TEX__block_faces__texture_ID (GL_TEXTURE0 + TEX__block_faces__texture_unit_ID)
#define TEX__block_faces__texture_type GL_TEXTURE_2D_ARRAY
#define TEX__block_faces__faces_per_block 6

/* Pixel */
// one pixel
typedef struct TEX__pixel {
    TEX__pixel_color p_red;
    TEX__pixel_color p_green;
    TEX__pixel_color p_blue;
    TEX__pixel_color p_alpha;
} TEX__pixel;

// create custom pixel
TEX__pixel TEX__create__pixel(TEX__pixel_color red, TEX__pixel_color green, TEX__pixel_color blue, TEX__pixel_color alpha) {
    TEX__pixel output;

    // setup output
    output.p_red = red;
    output.p_green = green;
    output.p_blue = blue;
    output.p_alpha = alpha;

    return output;
}

// create null pixel
TEX__pixel TEX__create_null__pixel() {
    // return null pixel
    return TEX__create__pixel(0, 0, 0, 0);
}

/* Texture Faces */
typedef struct TEX__faces {
    BASIC__buffer p_faces;
    TEX__dimension_length p_width;
    TEX__dimension_length p_height;
    TEX__face_count p_count;
} TEX__faces;

// create custom faces struct
TEX__faces TEX__create__faces(BASIC__buffer faces, TEX__dimension_length width, TEX__dimension_length height, TEX__face_count count) {
    TEX__faces output;

    // setup output
    output.p_faces = faces;
    output.p_width = width;
    output.p_height = height;
    output.p_count = count;

    return output;
}

// create null block faces
TEX__faces TEX__create_null__faces() {
    // return null
    return TEX__create__faces(BASIC__create_null__buffer(), 0, 0, 0);
}

// destroy faces
void TEX__destroy__faces(TEX__faces faces) {
    // deallocate
    BASIC__close__buffer(faces.p_faces);

    return;
}

// open faces
TEX__faces TEX__open__faces(TEX__dimension_length width, TEX__dimension_length height, TEX__face_count face_count) {
    TEX__faces output;

    // setup output
    output.p_faces = BASIC__open__buffer(sizeof(TEX__pixel) * width * height * face_count);
    output.p_width = width;
    output.p_height = height;
    output.p_count = face_count;

    return output;
}

/* All Game Textures */
// game texture type
typedef enum TEX__gtt {
    // game texture types
    TEX__gtt__block_faces,
    TEX__gtt__LIMIT,

    // count
    TEX__gtt__COUNT = TEX__gtt__LIMIT,
} TEX__gtt;

// game textures
typedef struct TEX__game_textures {
    TEX__faces p_block_faces;
    GLuint p_block_faces_handle;
} TEX__game_textures;

// create game textures
TEX__game_textures TEX__create__game_textures(TEX__faces block_faces, GLuint block_faces_handle) {
    TEX__game_textures output;

    // setup output
    output.p_block_faces = block_faces;
    output.p_block_faces_handle = block_faces_handle;

    return output;
}

// create null game textures
TEX__game_textures TEX__create_null__game_textures() {
    // return null
    return TEX__create__game_textures(TEX__create_null__faces(), 0);
}

// bind a specific texture buffer to be used in rendering
void TEX__bind__game_textures__specific(TEX__game_textures game_textures, TEX__gtt game_texture_type, SHADER__program shader_program) {
    // bind according to type
    switch (game_texture_type) {
        case TEX__gtt__block_faces:
            // set active texture (one per game texture type)
            glActiveTexture(TEX__block_faces__texture_ID);

            // bind texture buffer
            glBindTexture(TEX__block_faces__texture_type, game_textures.p_block_faces_handle);

            // set active texture unit in opengl shader uniform
            glUniform1i(glGetUniformLocation(shader_program.p_program_ID, "GLOBAL_current_texture_unit"), TEX__block_faces__texture_unit_ID);

            break;
        default:
            break;
    }

    return;
}

// unbind a specific texture buffer to be used in rendering
void TEX__unbind__game_textures__specific(TEX__game_textures game_textures, TEX__gtt game_texture_type) {
    // quiet compiler warning
    game_textures = game_textures;

    // unbind according to type
    switch (game_texture_type) {
        case TEX__gtt__block_faces:
            // unbind texture buffer
            glBindTexture(TEX__block_faces__texture_type, 0);
            
            break;
        default:
            break;
    }

    return;
}

// setup game textures on the cpu and gpu
TEX__game_textures TEX__open__game_textures(TEX__faces block_faces, SHADER__program shader_program) {
    TEX__game_textures output;

    // setup cpu side data for block faces
    output.p_block_faces = block_faces;
    glCreateTextures(TEX__block_faces__texture_type, 1, &(output.p_block_faces_handle));

    // setup opengl side for block faces
    TEX__bind__game_textures__specific(output, TEX__gtt__block_faces, shader_program);
    glTexParameteri(TEX__block_faces__texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(TEX__block_faces__texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(TEX__block_faces__texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(TEX__block_faces__texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage3D(TEX__block_faces__texture_type, 0, GL_RGBA8, block_faces.p_width, block_faces.p_height, block_faces.p_count, 0, GL_RGBA /* Do not change to GL_RGBA8, crashes */, GL_UNSIGNED_BYTE, block_faces.p_faces.p_address);
    TEX__unbind__game_textures__specific(output, TEX__gtt__block_faces);

    return output;
}

// close the game textures on the cpu and gpu
void TEX__close__game_textures(TEX__game_textures game_textures) {
    // close opengl handles
    glDeleteTextures(1, &(game_textures.p_block_faces_handle));

    // destroy cpu data
    TEX__destroy__faces(game_textures.p_block_faces);

    return;
}

/* Generating Textures */
// calculate write to pointer in faces
BASIC__address TEX__calculate__faces_pointer(TEX__faces faces, TEX__face_index face_index) {
    // return calculation
    return faces.p_faces.p_address + (sizeof(TEX__pixel) * faces.p_width * faces.p_height * face_index);
}

// write one pixel
BASIC__address TEX__write__pixel(BASIC__address output_faces_pointer, TEX__pixel pixel) {
    // write red pixel
    *((TEX__pixel_color*)output_faces_pointer) = pixel.p_red;
    output_faces_pointer += sizeof(TEX__pixel_color);

    // write green pixel
    *((TEX__pixel_color*)output_faces_pointer) = pixel.p_green;
    output_faces_pointer += sizeof(TEX__pixel_color);
    
    // write blue pixel
    *((TEX__pixel_color*)output_faces_pointer) = pixel.p_blue;
    output_faces_pointer += sizeof(TEX__pixel_color);
    
    // write alpha pixel
    *((TEX__pixel_color*)output_faces_pointer) = pixel.p_alpha;
    output_faces_pointer += sizeof(TEX__pixel_color);

    return output_faces_pointer;
}

// create a solid colored face
void TEX__generate_face__one_color(TEX__faces faces, TEX__face_index face_index, TEX__pixel pixel) {
    BASIC__address write_to;

    // get write to pointer
    write_to = TEX__calculate__faces_pointer(faces, face_index);

    // write data
    for (TEX__face_index width = 0; width < faces.p_width; width++) {
        for (TEX__face_index height = 0; height < faces.p_height; height++) {
            // write pixel
            write_to = TEX__write__pixel(write_to, pixel);
        }
    }

    return;
}

// create a semi randomly colored face
void TEX__generate_face__one_color_range(TEX__faces faces, TEX__face_index face_index, TEX__pixel initial_pixel, RANDOM__context* random_context, TEX__pixel_color color_intensity) {
    BASIC__address write_to;
    TEX__pixel temp_pixel;

    // get write to pointer
    write_to = TEX__calculate__faces_pointer(faces, face_index);

    // write pixels
    for (TEX__face_index width = 0; width < faces.p_width; width++) {
        for (TEX__face_index height = 0; height < faces.p_height; height++) {
            // create pixel
            temp_pixel = initial_pixel;

            // randomize colors
            temp_pixel.p_red += (RANDOM__generate_number__mark_1(random_context) % color_intensity);
            temp_pixel.p_green += (RANDOM__generate_number__mark_1(random_context) % color_intensity);
            temp_pixel.p_blue += (RANDOM__generate_number__mark_1(random_context) % color_intensity);

            // write pixel
            write_to = TEX__write__pixel(write_to, temp_pixel);
        }
    }

    return;
}

// create a box shaped texture
void TEX__generate_face__box_texture(TEX__faces faces, TEX__face_index face_index, TEX__pixel border, TEX__pixel center) {
    BASIC__address write_to;

    // get write to pointer
    write_to = TEX__calculate__faces_pointer(faces, face_index);

    // write top row
    for (TEX__pixel_index width = 0; width < faces.p_width; width++) {
        // write pixel
        write_to = TEX__write__pixel(write_to, border);
    }

    // write body
    for (TEX__pixel_index height = 1; height < faces.p_height - 1; height++) {
        // write left wall
        write_to = TEX__write__pixel(write_to, border);

        // write center
        for (TEX__pixel_index width = 1; width < faces.p_width - 1; width++) {
            // write center
            write_to = TEX__write__pixel(write_to, center);
        }

        // write right wall
        write_to = TEX__write__pixel(write_to, border);
    }

    // write bottom row
    for (TEX__pixel_index width = 0; width < faces.p_width; width++) {
        // write pixel
        write_to = TEX__write__pixel(write_to, border);
    }

    return;
}

// create a checkerboard styled texture
void TEX__generate_face__checkerboard(TEX__faces faces, TEX__face_index face_index, TEX__pixel a, TEX__pixel b) {
    BASIC__address write_to;

    // get write to pointer
    write_to = TEX__calculate__faces_pointer(faces, face_index);

    // draw each double row
    for (TEX__pixel_index row = 0; row < faces.p_width; row += 2) {
        // draw the first row
        for (u64 pixel = 0; pixel < faces.p_width; pixel++) {
            if (pixel % 2 == 0) {
                write_to = TEX__write__pixel(write_to, a);
            } else {
                write_to = TEX__write__pixel(write_to, b);
            }
        }
        // draw the second row
        for (u64 pixel = 0; pixel < faces.p_width; pixel++) {
            if (pixel % 2 == 1) {
                write_to = TEX__write__pixel(write_to, a);
            } else {
                write_to = TEX__write__pixel(write_to, b);
            }
        }
    }

    return;
}

#endif
