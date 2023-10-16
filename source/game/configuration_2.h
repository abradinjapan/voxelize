#ifndef VOXELIZE__configuration_2
#define VOXELIZE__configuration_2

/* Include */
// game
#include "game.h"

// random number generator
#include <stdlib.h>

/* Define */
// block count
#define CONF2__block_count 10

// block types
#define CONF2__block__no_block 0
#define CONF2__block__air 1
#define CONF2__block__stone 2
#define CONF2__block__grass 3
#define CONF2__block__sand 4
#define CONF2__block__dirt 5
#define CONF2__block__glass 6
#define CONF2__block__tar 7
#define CONF2__block__red_leaves 8
#define CONF2__block__green_leaves 9

/* Setup Game Data */
// write one pixel
BASIC__address CONF2__write__pixel(BASIC__address output_faces_pointer, TEX__pixel_color red, TEX__pixel_color green, TEX__pixel_color blue, TEX__pixel_color alpha) {
    // write red pixel
    *((TEX__pixel_color*)output_faces_pointer) = red;
    output_faces_pointer += sizeof(TEX__pixel_color);

    // write green pixel
    *((TEX__pixel_color*)output_faces_pointer) = green;
    output_faces_pointer += sizeof(TEX__pixel_color);
    
    // write blue pixel
    *((TEX__pixel_color*)output_faces_pointer) = blue;
    output_faces_pointer += sizeof(TEX__pixel_color);
    
    // write alpha pixel
    *((TEX__pixel_color*)output_faces_pointer) = alpha;
    output_faces_pointer += sizeof(TEX__pixel_color);

    return output_faces_pointer;
}

// create game block faces
TEX__faces CONF2__open__block_faces() {
    TEX__faces output;
    BASIC__address output_faces_pointer;
    TEX__pixel_color color_intensity;

    // setup basic information
    output.p_width = 16;
    output.p_height = 16;
    output.p_count = CONF2__block_count;
    color_intensity = 5;

    // create buffer
    output.p_faces = BASIC__open__buffer(sizeof(TEX__pixel) * (output.p_height * output.p_width) * output.p_count);

    // setup random
    srand(1234567890);

    // setup face pointer
    output_faces_pointer = output.p_faces.p_address;

    // create no face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        for (u64 color = 0; color < TEX__pixel_colors_count; color++) {
            // write data
            *((TEX__pixel_color*)output_faces_pointer) = 0;

            // next color
            output_faces_pointer += sizeof(TEX__pixel_color);
        }
    }

    // create air face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 0, 0, 0, 0);
    }

    // create stone face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 120 + (rand() % color_intensity), 120 + (rand() % color_intensity), 120 + (rand() % color_intensity), 255);
    }

    // create grass face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 50 + (rand() % color_intensity), 240 + (rand() % color_intensity), 50 + (rand() % color_intensity), 255);
    }

    // create sand face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 242 + (rand() % color_intensity), 214 + (rand() % color_intensity), 136 + (rand() % color_intensity), 255);
    }

    // create dirt face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 100 + (rand() % color_intensity), 50 + (rand() % color_intensity), 0 + (rand() % color_intensity), 255);
    }

    // create glass face
    for (u64 pixel = 0; pixel < output.p_width; pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 245, 245, 245, 255);
    }
    for (u64 pixel = 0; pixel < (u64)(output.p_height - 2); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 245, 245, 245, 255);

        for (u64 row = 0; row < (u64)(output.p_width - 2); row++) {
            // write pixel
            output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 255, 255, 255, 0);
        }
        
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 245, 245, 245, 255);
    }
    for (u64 pixel = 0; pixel < output.p_width; pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 245, 245, 245, 255);
    }

    // create tar face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 20 + (rand() % color_intensity), 20 + (rand() % color_intensity), 20 + (rand() % color_intensity), 255);
    }

    // create red leaves face
    for (u64 section = 0; section < output.p_height / 2; section++) {
        // write first row
        for (u64 pixel = 0; pixel < output.p_width; pixel++) {
            if (pixel % 2 == 0) {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 255, 0, 0, 0);
            } else {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 255, 0, 0, 255);
            }
        }
        // write second row
        for (u64 pixel = 0; pixel < output.p_width; pixel++) {
            if (pixel % 2 == 1) {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 255, 0, 0, 0);
            } else {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 255, 0, 0, 255);
            }
        }
    }

    // create green leaves face
    for (u64 section = 0; section < output.p_height / 2; section++) {
        // write first row
        for (u64 pixel = 0; pixel < output.p_width; pixel++) {
            if (pixel % 2 == 0) {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 0, 0, 0, 0);
            } else {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 0, 225, 0, 255);
            }
        }
        // write second row
        for (u64 pixel = 0; pixel < output.p_width; pixel++) {
            if (pixel % 2 == 1) {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 0, 0, 0, 0);
            } else {
                output_faces_pointer = CONF2__write__pixel(output_faces_pointer, 0, 225, 0, 255);
            }
        }
    }

    return output;
}

// create game skins
SKIN__skins CONF2__open__skins() {
    SKIN__skins output;
    SKIN__skin_count no_skin = CONF2__block__no_block;
    SKIN__skin_count air_skin = CONF2__block__air;
    SKIN__skin_count stone_skin = CONF2__block__stone;
    SKIN__skin_count grass_skin = CONF2__block__grass;
    SKIN__skin_count sand_skin = CONF2__block__sand;
    SKIN__skin_count dirt_skin = CONF2__block__dirt;
    SKIN__skin_count glass_skin = CONF2__block__glass;
    SKIN__skin_count tar_skin = CONF2__block__tar;
    SKIN__skin_count red_leaves_skin = CONF2__block__red_leaves;
    SKIN__skin_count green_leaves_skin = CONF2__block__green_leaves;

    // setup output
    output = SKIN__open__block_skins(CONF2__block_count);

    // setup block skins
    SKIN__set__skin__block(output, no_skin, SKIN__create__block__one_skin(no_skin, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, air_skin, SKIN__create__block__one_skin(air_skin, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, stone_skin, SKIN__create__block__one_skin(stone_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, grass_skin, SKIN__create__block__one_skin(grass_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, sand_skin, SKIN__create__block__one_skin(sand_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, dirt_skin, SKIN__create__block__one_skin(dirt_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, glass_skin, SKIN__create__block__one_skin(glass_skin, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, tar_skin, SKIN__create__block__one_skin(tar_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, red_leaves_skin, SKIN__create__block__one_skin(red_leaves_skin, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, green_leaves_skin, SKIN__create__block__one_skin(green_leaves_skin, SKIN__bdt__draw_all_sides));

    return output;
}

/* Setup Configuration */
// opens a game in a specific way
GAME__information CONF2__open__game() {
    GAME__information output;
    BASIC__bt file_error;
    BASIC__buffer vertex_shader;
    BASIC__buffer fragment_shader;

    // setup variables
    output = GAME__create_null__game_information();
    output.p_temporaries = RENDER__open__temporaries();

    // setup shader buffers
    vertex_shader = BASIC__open__buffer_from_file_as_c_string(&file_error, BASIC__create__c_string_reference("./shaders/chunks/vertex.glsl", BASIC__bt__true));
    fragment_shader = BASIC__open__buffer_from_file_as_c_string(&file_error, BASIC__create__c_string_reference("./shaders/chunks/fragment.glsl", BASIC__bt__true));

    // setup graphics
    output.p_graphics = WINDOW__open__window(&(output.p_graphics_error), WINDOW__create__window_configuration(BASIC__create__c_string_reference("Voxelize", BASIC__bt__false), 720, 480));
    if (output.p_graphics_error != WINDOW__et__no_error) {
        // setup error indicator
        output.p_error_occured = BASIC__bt__true;

        // print error information
        printf("WINDOW__et[ %lu ]\n", (u64)output.p_graphics_error);

        return output;
    }

    // setup shaders
    output.p_chunks_shader_program = SHADER__compile__shaders_program(&(output.p_shader_error), &(output.p_shader_error_log), vertex_shader, fragment_shader);
    if (output.p_shader_error != SHADER__et__no_error) {
        // setup error indicator
        output.p_error_occured = BASIC__bt__true;

        // print error information
        printf("SHADER__et[ %lu ]:\n%s\n", (u64)output.p_shader_error, (u8*)&(output.p_shader_error_log.p_log));

        return output;
    }
    SHADER__use__program(output.p_chunks_shader_program);

    // clean up shader source buffers
    BASIC__close__buffer(vertex_shader);
    BASIC__close__buffer(fragment_shader);

    // setup opengl basic settings
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // setup other sdl2 settings
    //SDL_SetWindowFullscreen(output.p_graphics.p_window_context, SDL_WINDOW_FULLSCREEN_DESKTOP);

    // setup controls
    output.p_controls = CONTROLS__open__controls();

    return output;
}

// create small world
CHUNK__chunks CONF2__create__test_world_2() {
    CHUNK__chunks output;
    CHUNK__chunks_index chunks_size = 3;
    CHUNK__chunk chunk;
    CHUNK__chunk_address temp_chunk;

    // create blank chunk
    chunk = CHUNK__create__chunk(CHUNK__create__block(CONF2__block__air));

    // setup chunks
    output = CHUNK__open__chunks(chunks_size, chunks_size, chunks_size, &chunk);

    // build ground
    for (u64 chunks_x = 0; chunks_x < chunks_size; chunks_x++) {
        for (u64 chunks_z = 0; chunks_z < chunks_size; chunks_z++) {
            // get chunk
            temp_chunk = CHUNK__get__chunk_pointer_in_chunks(output, CHUNK__calculate__chunks_index(output, chunks_x, 0, chunks_z));

            // draw in one chunk
            for (u64 block_x = 0; block_x < ESS__define__chunk_side_block_count; block_x++) {
                for (u64 block_z = 0; block_z < ESS__define__chunk_side_block_count; block_z++) {
                    // setup stone
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(block_x, 0, block_z), CHUNK__create__block(CONF2__block__glass));

                    // setup topsoil
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(block_x, 1, block_z), CHUNK__create__block(CONF2__block__grass));
                }
            }
        }
    }

    // create middle chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(CHUNK__create__block(CONF2__block__sand), CHUNK__create__block(CONF2__block__stone), CHUNK__create__block(CONF2__block__grass));
    CHUNK__set__chunk_in_chunks(output, CHUNK__calculate__chunks_index(output, 1, 0, 1), &chunk);

    // create corner chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(CHUNK__create__block(CONF2__block__green_leaves), CHUNK__create__block(CONF2__block__air), CHUNK__create__block(CONF2__block__air));
    CHUNK__set__chunk_in_chunks(output, CHUNK__calculate__chunks_index(output, 2, 2, 2), &chunk);

    return output;
}

// setup game
void CONF2__setup__game(GAME__information* game_information) {
    // setup textures
    (*game_information).p_game_textures = TEX__open__game_textures(CONF2__open__block_faces(), (*game_information).p_chunks_shader_program);

    // setup skins
    (*game_information).p_skins = CONF2__open__skins();

    // setup chunks
    (*game_information).p_chunks = CONF2__create__test_world_2();

    // setup world
    (*game_information).p_world = RENDER__open__world((*game_information).p_chunks);

    // setup drawing information
    RENDER__render__world((*game_information).p_skins, (*game_information).p_chunks, (*game_information).p_world, (*game_information).p_temporaries);

    // fixate mouse
    CONTROLS__update__mouse_lock(BASIC__bt__true);

    return;
}

// closes the game properly
void CONF2__close__game(GAME__information game_information) {
    // clean up
    CONTROLS__close__controls(game_information.p_controls);
    TEX__close__game_textures(game_information.p_game_textures);
    SKIN__close__skins(game_information.p_skins);
    SHADER__close__program(game_information.p_chunks_shader_program);
    WINDOW__close__window(game_information.p_graphics);
    CHUNK__close__chunks(game_information.p_chunks);
    RENDER__close__world(game_information.p_world);
    RENDER__close__temporaries(game_information.p_temporaries);

    return;
}

// displays the next frame
void CONF2__display__frame(GAME__information* game_information) {
    RENDER__transform transform_change;
    RENDER__vertex player_camera_rotation;
    float movement_speed = 1.0f;
    float rotation_speed = 0.5f;

    // setup null change
    transform_change = RENDER__create__transform(RENDER__create_null__vertex(), RENDER__create_null__vertex());

    // update mouse movement
    CONTROLS__update__mouse_position_change(&((*game_information).p_controls));

    // update rotations
    //player_camera_rotation.p_vertices[0] = glm_rad((*game_information).p_controls.p_mouse_position_change.p_mouse_change_y * -1.0f * rotation_speed);
    //player_camera_rotation.p_vertices[1] = glm_rad((*game_information).p_controls.p_mouse_position_change.p_mouse_change_x * rotation_speed);
    transform_change.p_rotation.p_vertices[0] = (*game_information).p_controls.p_mouse_position_change.p_mouse_change_y * -1.0f * rotation_speed;
    transform_change.p_rotation.p_vertices[1] = (*game_information).p_controls.p_mouse_position_change.p_mouse_change_x * rotation_speed;

    // update player position movement (x)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_A)) {
        transform_change.p_position_offset.p_vertices[0] += movement_speed * -1.0f;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_D)) {
        transform_change.p_position_offset.p_vertices[0] += movement_speed;
    }

    // update player position movement (y)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_LSHIFT)) {
        transform_change.p_position_offset.p_vertices[1] += movement_speed;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_SPACE)) {
        transform_change.p_position_offset.p_vertices[1] += movement_speed * -1.0f;
    }

    // update player position movement (z)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_W)) {
        transform_change.p_position_offset.p_vertices[2] += movement_speed * -1.0f;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_S)) {
        transform_change.p_position_offset.p_vertices[2] += movement_speed;
    }

    // check if player requested quit
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_ESCAPE)) {
        (*game_information).p_game_state = GAME__gsi__game_quitting;
    }

    // update window size
    WINDOW__update__current_window_size(&((*game_information).p_graphics));

    // draw everything
    RENDER__draw__world((*game_information).p_game_textures, (*game_information).p_world, (*game_information).p_graphics.p_window_configuration, (*game_information).p_chunks_shader_program, transform_change, player_camera_rotation);

    // display window
    SDL_GL_SwapWindow((*game_information).p_graphics.p_window_context);

    // setup next frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

// checks for errors
BASIC__bt CONF2__check__error(GAME__information* game_information) {
    GLenum error_code;

    // quick thing to quiet compiler warning
    game_information = game_information;

    // check for opengl error
    error_code = glGetError();

    // check if error occured
    if (error_code != GL_NO_ERROR) {
        // print error to stdout
        printf("OpenGL Error: %lu\n", (u64)error_code);

        // return error occured
        return BASIC__bt__true;
    }

    // return no error occured
    return BASIC__bt__false;
}


#endif
