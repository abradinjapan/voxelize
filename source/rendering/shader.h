#ifndef VOXELIZE__shader
#define VOXELIZE__shader

/* Include */
// base
#include "../base/essentials.h"

// dependencies
#include <SDL2/SDL.h>

/* Define */
#define SHADER__opengl_error_log_length 1024

/* Error */
// error type
typedef enum SHADER__et {
    // error codes
    SHADER__et__no_error,
    SHADER__et__vertex_shader_not_compiled,
    SHADER__et__fragment_shader_not_compiled,
    SHADER__et__shaders_could_not_link,
    SHADER__et__LIMIT,

    // count
    SHADER__et__COUNT = SHADER__et__LIMIT,
} SHADER__et;

/* OpenGL Error */
typedef struct SHADER__error_log {
    u8 p_log[SHADER__opengl_error_log_length];
    GLenum p_opengl_error_code;
} SHADER__error_log;

// create opengl shader error information log
SHADER__error_log SHADER__create_null__error_log() {
    SHADER__error_log output;

    // setup output
    for (u64 character_index = 0; character_index < SHADER__opengl_error_log_length; character_index += 1) {
        output.p_log[character_index] = 0;
    }
    output.p_opengl_error_code = GL_NO_ERROR;

    return output;
}

/* Shader */
typedef struct SHADER__shader {
    GLuint p_shader_ID;
    BASIC__buffer p_program;
} SHADER__shader;

typedef struct SHADER__program {
    GLuint p_program_ID;
    SHADER__shader p_vertex_shader;
    SHADER__shader p_fragment_shader;
} SHADER__program;

// create custom shader (struct)
SHADER__shader SHADER__create__shader(GLuint shader_ID, BASIC__buffer program) {
    SHADER__shader output;

    // setup output
    output.p_shader_ID = shader_ID;
    output.p_program = program;

    return output;
}

// create recognizably empty shader
SHADER__shader SHADER__create_null__shader() {
    // return null
    return SHADER__create__shader(0, BASIC__create_null__buffer());
}

// send shader code to opengl to be compiled
SHADER__shader SHADER__compile__shader(SHADER__et* error, SHADER__error_log* error_log, BASIC__buffer shader_data, GLenum shader_type) {
    SHADER__shader output;
    GLint error_log_length;

    // setup output
    output = SHADER__create_null__shader();

    // create shader space
    output.p_shader_ID = glCreateShader(shader_type);

    // send program to gpu
    glShaderSource(output.p_shader_ID, 1, (const GLchar* const*)&(shader_data.p_address), NULL);

    // compile shader
    glCompileShader(output.p_shader_ID);

    // check for errors
    glGetShaderiv(output.p_shader_ID, GL_INFO_LOG_LENGTH, &error_log_length);
    if (error_log_length > 0) {
        // get internal shader error type
        if (shader_type == GL_VERTEX_SHADER) {
            *error = SHADER__et__vertex_shader_not_compiled;
        } else if (shader_type == GL_FRAGMENT_SHADER) {
            *error = SHADER__et__fragment_shader_not_compiled;
        }

        // get opengl error data
        glGetShaderInfoLog(output.p_shader_ID, SHADER__opengl_error_log_length, NULL, (GLchar*)&((*error_log).p_log));
        (*error_log).p_opengl_error_code = glGetError();
    }

    return output;
}

// create null shaders program
SHADER__program SHADER__create_null__shader_program() {
    SHADER__program output;

    // setup output
    output.p_program_ID = 0;
    output.p_vertex_shader = SHADER__create_null__shader();
    output.p_fragment_shader = SHADER__create_null__shader();

    return output;
}

// create and link opengl shader program
SHADER__program SHADER__compile__shaders_program(SHADER__et* error, SHADER__error_log* error_log, BASIC__buffer vertex_shader, BASIC__buffer fragment_shader) {
    SHADER__program output;
    GLint error_log_length;

    // setup errors to no error
    *error = SHADER__et__no_error;

    // setup output
    output.p_program_ID = 0;
    output.p_vertex_shader = SHADER__create_null__shader();
    output.p_fragment_shader = SHADER__create_null__shader();

    // setup blank error
    *error = SHADER__et__no_error;

    // compile shaders
    output.p_vertex_shader = SHADER__compile__shader(error, error_log, vertex_shader, GL_VERTEX_SHADER);
    if (*error != SHADER__et__no_error) {
        return output;
    }

    output.p_fragment_shader = SHADER__compile__shader(error, error_log, fragment_shader, GL_FRAGMENT_SHADER);
    if (*error != SHADER__et__no_error) {
        return output;
    }

    // create shader program
    output.p_program_ID = glCreateProgram();

    // setup shader linking
    glAttachShader(output.p_program_ID, output.p_vertex_shader.p_shader_ID);
    glAttachShader(output.p_program_ID, output.p_fragment_shader.p_shader_ID);
    
    // link shaders
    glLinkProgram(output.p_program_ID);
    
    // check for errors
    glGetProgramiv(output.p_program_ID, GL_LINK_STATUS, &error_log_length);
    if (error_log_length > 1) {
        // get internal error type
        *error = SHADER__et__shaders_could_not_link;

        // get opengl error data
        glGetProgramInfoLog(output.p_program_ID, SHADER__opengl_error_log_length, NULL, (GLchar*)&((*error_log).p_log));
        (*error_log).p_opengl_error_code = glGetError();
    }

    return output;
}

// tell opengl to use shader program
void SHADER__use__program(SHADER__program shaders_program) {
    glUseProgram(shaders_program.p_program_ID);

    return;
}

// tell opengl to discard shader program
void SHADER__close__program(SHADER__program shaders_program) {
    glDeleteShader(shaders_program.p_vertex_shader.p_shader_ID);
    glDeleteShader(shaders_program.p_fragment_shader.p_shader_ID);
    glDeleteProgram(shaders_program.p_program_ID);

    return;
}

#endif
