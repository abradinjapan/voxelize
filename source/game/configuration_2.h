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
// create game block faces
TEX__faces CONF2__open__block_faces(RANDOM__context* random_context) {
    TEX__faces output;
    TEX__pixel_color color_intensity = 5;

    // open faces
    output = TEX__open__faces(16, 16, CONF2__block_count);

    // generate faces
    TEX__generate_face__one_color(output, CONF2__block__no_block, TEX__create__pixel(0, 0, 0, 0)); // no face
    TEX__generate_face__one_color(output, CONF2__block__air, TEX__create__pixel(0, 0, 0, 0)); // air face
    TEX__generate_face__one_color_range(output, CONF2__block__stone, TEX__create__pixel(120, 120, 120, 255), random_context, color_intensity); // stone face
    TEX__generate_face__one_color_range(output, CONF2__block__grass, TEX__create__pixel(50, 240, 50, 255), random_context, color_intensity); // grass face
    TEX__generate_face__one_color_range(output, CONF2__block__sand, TEX__create__pixel(242, 214, 136, 255), random_context, color_intensity); // sand face
    TEX__generate_face__one_color_range(output, CONF2__block__dirt, TEX__create__pixel(100, 50, 0, 255), random_context, color_intensity); // dirt face
    TEX__generate_face__box_texture(output, CONF2__block__glass, TEX__create__pixel(245, 245, 245, 255), TEX__create__pixel(0, 0, 0, 0)); // glass face
    TEX__generate_face__one_color_range(output, CONF2__block__tar, TEX__create__pixel(20, 20, 20, 255), random_context, color_intensity); // tar face
    TEX__generate_face__checkerboard(output, CONF2__block__red_leaves, TEX__create__pixel(255, 0, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // red leaves face
    TEX__generate_face__checkerboard(output, CONF2__block__green_leaves, TEX__create__pixel(0, 225, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // green leaves face

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

    // setup temporaries
    output.p_temporaries = RENDER__open__temporaries();

    // setup randoms
    output.p_random_pixel_context = RANDOM__create__context(1234567, 1);

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

// create test world
void CONF2__create__test_world(GAME__information* game_information, ESS__world_vertex camera_position) {
    CHUNK__chunks_index chunks_size = 5;
    CHUNK__chunk chunk;

    // open chunk
    chunk = CHUNK__create__chunk__bars(CHUNK__create__block(CONF2__block__air), CHUNK__create__block(CONF2__block__stone));
    
    // setup chunks
    (*game_information).p_chunks = CHUNK__open__chunks(chunks_size, chunks_size, chunks_size, &chunk);

    // setup world positions
    (*game_information).p_positions = POS__open__positions(camera_position, camera_position, (*game_information).p_chunks);

    return;
}

// create small world
void CONF2__create__test_world_2(GAME__information* game_information, ESS__world_vertex camera_position) {
    CHUNK__chunks_index chunks_size = 3;
    CHUNK__chunk chunk;
    CHUNK__chunk_address temp_chunk = 0;

    // create blank chunk
    chunk = CHUNK__create__chunk(CHUNK__create__block(CONF2__block__air));

    // setup chunks
    (*game_information).p_chunks = CHUNK__open__chunks(chunks_size, chunks_size, chunks_size, &chunk);

    // build ground
    for (u64 chunks_x = 0; chunks_x < chunks_size; chunks_x++) {
        for (u64 chunks_z = 0; chunks_z < chunks_size; chunks_z++) {
            // get chunk
            temp_chunk = CHUNK__get__chunk_pointer_in_chunks((*game_information).p_chunks, CHUNK__calculate__chunks_index((*game_information).p_chunks, chunks_x, 0, chunks_z));

            // draw in one chunk
            for (u64 block_x = 0; block_x < ESS__define__chunk_side_block_count; block_x++) {
                for (u64 block_z = 0; block_z < ESS__define__chunk_side_block_count; block_z++) {
                    // setup stone
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(CHUNK__create__block_position(block_x, 0, block_z)), CHUNK__create__block(CONF2__block__glass));

                    // setup topsoil
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(CHUNK__create__block_position(block_x, 1, block_z)), CHUNK__create__block(CONF2__block__grass));
                }
            }
        }
    }

    // create middle chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(CHUNK__create__block(CONF2__block__sand), CHUNK__create__block(CONF2__block__stone), CHUNK__create__block(CONF2__block__grass));
    CHUNK__set__chunk_in_chunks((*game_information).p_chunks, CHUNK__calculate__chunks_index((*game_information).p_chunks, 1, 0, 1), &chunk);

    // create corner chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(CHUNK__create__block(CONF2__block__green_leaves), CHUNK__create__block(CONF2__block__air), CHUNK__create__block(CONF2__block__air));
    CHUNK__set__chunk_in_chunks((*game_information).p_chunks, CHUNK__calculate__chunks_index((*game_information).p_chunks, 2, 2, 2), &chunk);

    // setup world positions
    (*game_information).p_positions = POS__open__positions(camera_position, camera_position, (*game_information).p_chunks);

    return;
}

// setup game
void CONF2__setup__game(GAME__information* game_information) {
    // setup textures
    (*game_information).p_game_textures = TEX__open__game_textures(CONF2__open__block_faces(&((*game_information).p_random_pixel_context)), (*game_information).p_chunks_shader_program);

    // setup skins
    (*game_information).p_skins = CONF2__open__skins();

    // setup chunks
    CONF2__create__test_world(game_information, ESS__calculate__world_center());

    // setup world
    (*game_information).p_world = RENDER__open__world((*game_information).p_chunks);

    // setup drawing information
    RENDER__render__world((*game_information).p_skins, (*game_information).p_chunks, (*game_information).p_positions, (*game_information).p_world, (*game_information).p_temporaries);

    // setup camera rotation
    (*game_information).p_camera_rotation = RENDER__create__vertex(0.0f, 0.0f, 0.0f);

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
    POS__close__positions(game_information.p_positions);
    CHUNK__close__chunks(game_information.p_chunks);
    RENDER__close__world(game_information.p_world);
    RENDER__close__temporaries(game_information.p_temporaries);

    return;
}

// performs block placing and rerendering
void CONF2__do__block_placement(GAME__information* game_information, CHUNK__block_data block) {
    CHUNK__chunks_index chunks_index = -1;
    CHUNK__block_index block_index = -1;
    BASIC__bt in_chunk = BASIC__bt__false;

    // find chunk
    for (CHUNK__chunks_index i = 0; i < (*game_information).p_positions.p_chunk_body_count; i++) {
        // check range
        if (ESS__calculate__coords_are_in_chunk(((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[i], (*game_information).p_positions.p_camera_position)) {
            // found chunk
            in_chunk = BASIC__bt__true;

            // mark index
            chunks_index = i;

            break;
        }
    }

    // if chunk was found
    if (in_chunk == BASIC__bt__true) {
        // get block position
        block_index = POS__calculate__block_index_from_world_position(((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index], (*game_information).p_positions.p_camera_position);

        // DEBUG
        //printf("Placing Block!\n\tCamera Position: [ %lu, %lu, %lu ]\n\tChunk Position: [ %lu, %lu, %lu ]\n\tDifference: [ %li, %li, %li ]\n", (*game_information).p_positions.p_camera_position.p_x, (*game_information).p_positions.p_camera_position.p_y, (*game_information).p_positions.p_camera_position.p_z, ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_x, ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_y, ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_z, (*game_information).p_positions.p_camera_position.p_x - ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_x, (*game_information).p_positions.p_camera_position.p_y - ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_y, (*game_information).p_positions.p_camera_position.p_z - ((ESS__world_vertex*)(*game_information).p_positions.p_chunk_body_positions.p_address)[chunks_index].p_z);

        // update block
        ((CHUNK__chunk*)(*game_information).p_chunks.p_chunk_block_data.p_address)[chunks_index].p_blocks[block_index] = block;

        // rerender world
        RENDER__render__world((*game_information).p_skins, (*game_information).p_chunks, (*game_information).p_positions, (*game_information).p_world, (*game_information).p_temporaries);
    }

    return;
}

// displays the next frame
void CONF2__display__frame(GAME__information* game_information) {
    float rotation_speed = 0.5f;
    ESS__world_axis shift = 2;

    // update mouse movement
    CONTROLS__update__mouse_position_change(&((*game_information).p_controls));

    // update rotations
    (*game_information).p_camera_rotation.p_vertices[0] += (*game_information).p_controls.p_mouse_position_change.p_mouse_change_y * -1.0f * rotation_speed;
    (*game_information).p_camera_rotation.p_vertices[1] += (*game_information).p_controls.p_mouse_position_change.p_mouse_change_x * rotation_speed;

    // update player position movement (x)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_A)) {
        (*game_information).p_positions.p_camera_position.p_x -= ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_D)) {
        (*game_information).p_positions.p_camera_position.p_x += ESS__define__bits_per_block__total_count >> shift;
    }

    // update player position movement (y)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_LSHIFT)) {
        (*game_information).p_positions.p_camera_position.p_y += ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_SPACE)) {
        (*game_information).p_positions.p_camera_position.p_y -= ESS__define__bits_per_block__total_count >> shift;
    }

    // update player position movement (z)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_W)) {
        (*game_information).p_positions.p_camera_position.p_z -= ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_S)) {
        (*game_information).p_positions.p_camera_position.p_z += ESS__define__bits_per_block__total_count >> shift;
    }

    // block placing
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_E)) {
        // do block placement
        CONF2__do__block_placement(game_information, CHUNK__create__block(CONF2__block__red_leaves));
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_Q)) {
        // do block placement
        CONF2__do__block_placement(game_information, CHUNK__create__block(CONF2__block__air));
    }

    // check if player requested quit
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_ESCAPE)) {
        (*game_information).p_game_state = GAME__gsi__game_quitting;
    }

    // update window size
    WINDOW__update__current_window_size(&((*game_information).p_graphics));

    // draw everything
    RENDER__draw__world((*game_information).p_game_textures, (*game_information).p_world, (*game_information).p_graphics.p_window_configuration, (*game_information).p_chunks_shader_program, (*game_information).p_positions, (*game_information).p_camera_rotation);

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
