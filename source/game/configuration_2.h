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

// block face types
typedef enum CONF2__bft {
    CONF2__bft__no_block,
    CONF2__bft__air,
    CONF2__bft__stone,
    CONF2__bft__grass,
    CONF2__bft__sand,
    CONF2__bft__dirt,
    CONF2__bft__glass,
    CONF2__bft__tar,
    CONF2__bft__red_leaves,
    CONF2__bft__green_leaves,

    // count
    CONF2__bft__COUNT,
} CONF2__bft;

/* Setup Game Data */
// create game block faces
TEX__faces CONF2__open__block_faces(RANDOM__context* random_context) {
    TEX__faces output;
    TEX__pixel_color color_intensity = 5;

    // open faces
    output = TEX__open__faces(16, 16, CONF2__block_count);

    // generate faces
    TEX__generate_face__one_color(output, CONF2__bft__no_block, TEX__create__pixel(0, 0, 0, 0)); // no face
    TEX__generate_face__one_color(output, CONF2__bft__air, TEX__create__pixel(0, 0, 0, 0)); // air face
    TEX__generate_face__one_color_range(output, CONF2__bft__stone, TEX__create__pixel(120, 120, 120, 255), random_context, color_intensity); // stone face
    TEX__generate_face__one_color_range(output, CONF2__bft__grass, TEX__create__pixel(50, 240, 50, 255), random_context, color_intensity); // grass face
    TEX__generate_face__one_color_range(output, CONF2__bft__sand, TEX__create__pixel(242, 214, 136, 255), random_context, color_intensity); // sand face
    TEX__generate_face__one_color_range(output, CONF2__bft__dirt, TEX__create__pixel(100, 50, 0, 255), random_context, color_intensity); // dirt face
    TEX__generate_face__box_texture(output, CONF2__bft__glass, TEX__create__pixel(245, 245, 245, 255), TEX__create__pixel(0, 0, 0, 0)); // glass face
    TEX__generate_face__one_color_range(output, CONF2__bft__tar, TEX__create__pixel(20, 20, 20, 255), random_context, color_intensity); // tar face
    TEX__generate_face__checkerboard(output, CONF2__bft__red_leaves, TEX__create__pixel(255, 0, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // red leaves face
    TEX__generate_face__checkerboard(output, CONF2__bft__green_leaves, TEX__create__pixel(0, 225, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // green leaves face

    return output;
}

// create game skins
SKIN__skins CONF2__open__skins() {
    SKIN__skins output;
    SKIN__skin_count no_skin = CONF2__bft__no_block;
    SKIN__skin_count air_skin = CONF2__bft__air;
    SKIN__skin_count stone_skin = CONF2__bft__stone;
    SKIN__skin_count grass_skin = CONF2__bft__grass;
    SKIN__skin_count sand_skin = CONF2__bft__sand;
    SKIN__skin_count dirt_skin = CONF2__bft__dirt;
    SKIN__skin_count glass_skin = CONF2__bft__glass;
    SKIN__skin_count tar_skin = CONF2__bft__tar;
    SKIN__skin_count red_leaves_skin = CONF2__bft__red_leaves;
    SKIN__skin_count green_leaves_skin = CONF2__bft__green_leaves;

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
    ESS__dimensions chunks_dimensions = ESS__create__dimensions(chunks_size, chunks_size, chunks_size);
    CHUNK__chunk chunk;

    // blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bft__air, BLOCK__create_null__metadata());
    BLOCK__block bar_block = BLOCK__create__block(CONF2__bft__stone, BLOCK__create_null__metadata());

    // setup world positions
    (*game_information).p_world_manager.p_positioning = POS__open__positioning(camera_position, camera_position, chunks_dimensions);

    // open chunk
    chunk = CHUNK__create__chunk__bars(air_block, bar_block);
    
    // setup chunks
    (*game_information).p_world_manager.p_chunks = CHUNK__open__chunks(ESS__calculate__dimensions_volume(chunks_dimensions), &chunk);

    return;
}

// create small world
void CONF2__create__test_world_2(GAME__information* game_information, ESS__world_vertex camera_position) {
    CHUNK__chunks_index chunks_size = 3;
    ESS__dimensions chunks_dimensions = ESS__create__dimensions(chunks_size, chunks_size, chunks_size);
    CHUNK__chunk chunk;
    CHUNK__chunk_address temp_chunk = 0;

    // blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bft__air, BLOCK__create_null__metadata());
    BLOCK__block glass_block = BLOCK__create__block(CONF2__bft__glass, BLOCK__create_null__metadata());
    BLOCK__block grass_block = BLOCK__create__block(CONF2__bft__grass, BLOCK__create_null__metadata());
    BLOCK__block sand_block = BLOCK__create__block(CONF2__bft__sand, BLOCK__create_null__metadata());
    BLOCK__block stone_block = BLOCK__create__block(CONF2__bft__stone, BLOCK__create_null__metadata());
    BLOCK__block green_leaves_block = BLOCK__create__block(CONF2__bft__green_leaves, BLOCK__create_null__metadata());

    // setup world positions
    (*game_information).p_world_manager.p_positioning = POS__open__positioning(camera_position, camera_position, chunks_dimensions);

    // create blank chunk
    chunk = CHUNK__create__chunk(air_block);

    // setup chunks
    (*game_information).p_world_manager.p_chunks = CHUNK__open__chunks((*game_information).p_world_manager.p_positioning.p_chunk_body_count, &chunk);

    // build ground
    for (u64 chunks_x = 0; chunks_x < chunks_size; chunks_x++) {
        for (u64 chunks_z = 0; chunks_z < chunks_size; chunks_z++) {
            // get chunk
            temp_chunk = CHUNK__get__chunk_pointer_in_chunks((*game_information).p_world_manager.p_chunks, ESS__calculate__dimensions_index((*game_information).p_world_manager.p_positioning.p_chunk_body_dimensions, chunks_x, 0, chunks_z));

            // draw in one chunk
            for (u64 block_x = 0; block_x < ESS__define__chunk_side_block_count; block_x++) {
                for (u64 block_z = 0; block_z < ESS__define__chunk_side_block_count; block_z++) {
                    // setup stone
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(CHUNK__create__block_position(block_x, 0, block_z)), glass_block);

                    // setup topsoil
                    CHUNK__set__block_data_from_chunk_address(temp_chunk, CHUNK__calculate__block_index(CHUNK__create__block_position(block_x, 1, block_z)), grass_block);
                }
            }
        }
    }

    // create middle chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(sand_block, stone_block, grass_block);
    CHUNK__set__chunk_in_chunks((*game_information).p_world_manager.p_chunks, ESS__calculate__dimensions_index((*game_information).p_world_manager.p_positioning.p_chunk_body_dimensions, 1, 0, 1), &chunk);

    // create corner chunk
    chunk = CHUNK__create__chunk__3_rotating_block_pattern(green_leaves_block, air_block, air_block);
    CHUNK__set__chunk_in_chunks((*game_information).p_world_manager.p_chunks, ESS__calculate__dimensions_index((*game_information).p_world_manager.p_positioning.p_chunk_body_dimensions, 2, 2, 2), &chunk);

    return;
}

// generate chunks based on world position
CHUNK__chunk CONF2__generate_chunks__ground_and_air(ESS__world_vertex chunk_position) {
    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bft__air, BLOCK__create_null__metadata());
    BLOCK__block grass_block = BLOCK__create__block(CONF2__bft__grass, BLOCK__create_null__metadata());

    // return function based on height
    // is above ground
    if (chunk_position.p_y > ESS__calculate__world_center().p_y) {
        // return air
        return CHUNK__create__chunk(air_block);
    // is ground
    } else {
        // return ground
        return CHUNK__create__chunk__3_rotating_block_pattern(air_block, air_block, grass_block);
    }
}

// create a world with the world manager
void CONF2__open__world(GAME__information* game_information, ESS__dimensions chunks_dimensions, ESS__world_vertex chunks_initial_position, ESS__world_vertex camera_position) {
    // open world manager
    (*game_information).p_world_manager = MANAGER__open__world_manager(&CONF2__generate_chunks__ground_and_air, chunks_dimensions, chunks_initial_position, camera_position);

    return;
}

// setup game
void CONF2__setup__game(GAME__information* game_information) {
    ESS__world_vertex camera_position = ESS__calculate__world_center();

    // setup textures
    (*game_information).p_game_textures = TEX__open__game_textures(CONF2__open__block_faces(&((*game_information).p_random_pixel_context)), (*game_information).p_chunks_shader_program);

    // setup skins
    (*game_information).p_skins = CONF2__open__skins();

    // setup chunks
    //CONF2__create__test_world_2(game_information, ESS__calculate__world_center());

    // setup world
    //(*game_information).p_world_manager.p_rendered_world = RENDER__open__world((*game_information).p_world_manager.p_positioning);

    // open world manager
    CONF2__open__world(game_information, ESS__create__dimensions(3, 3, 3), camera_position, camera_position);

    // generate chunks
    MANAGER__build__world((*game_information).p_world_manager, camera_position);

    // setup drawing information
    RENDER__render__entire_world((*game_information).p_skins, (*game_information).p_world_manager.p_chunks, (*game_information).p_world_manager.p_positioning, (*game_information).p_world_manager.p_rendered_world, (*game_information).p_temporaries);

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
    MANAGER__close__world_manager(game_information.p_world_manager);
    RENDER__close__temporaries(game_information.p_temporaries);

    return;
}

// performs block placing and rerendering
void CONF2__do__block_placement(GAME__information* game_information, BLOCK__block block) {
    CHUNK__chunks_index chunks_index = -1;
    CHUNK__block_index block_index = -1;
    BASIC__bt in_chunk = BASIC__bt__false;

    // find chunk
    for (CHUNK__chunks_index i = 0; i < (*game_information).p_world_manager.p_positioning.p_chunk_body_count; i++) {
        // check range
        if (ESS__calculate__coords_are_in_chunk(((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[i], (*game_information).p_world_manager.p_positioning.p_camera_position)) {
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
        block_index = POS__calculate__block_index_from_world_position(((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index], (*game_information).p_world_manager.p_positioning.p_camera_position);

        // DEBUG
        //printf("Placing Block!\n\tCamera Position: [ %lu, %lu, %lu ]\n\tChunk Position: [ %lu, %lu, %lu ]\n\tDifference: [ %li, %li, %li ]\n", (*game_information).p_world_manager.p_positioning.p_camera_position.p_x, (*game_information).p_world_manager.p_positioning.p_camera_position.p_y, (*game_information).p_world_manager.p_positioning.p_camera_position.p_z, ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_x, ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_y, ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_z, (*game_information).p_world_manager.p_positioning.p_camera_position.p_x - ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_x, (*game_information).p_world_manager.p_positioning.p_camera_position.p_y - ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_y, (*game_information).p_world_manager.p_positioning.p_camera_position.p_z - ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index].p_z);

        // update block
        ((CHUNK__chunk*)(*game_information).p_world_manager.p_chunks.p_chunk_block_data.p_address)[chunks_index].p_blocks[block_index] = block;

        // rerender affected chunk body
        RENDER__render__chunk_body((*game_information).p_skins, CHUNK__get__chunk_pointer_in_chunks((*game_information).p_world_manager.p_chunks, chunks_index), chunks_index, (*game_information).p_world_manager.p_rendered_world, (*game_information).p_temporaries);

        // rerender surfaces
        // TODO
    }

    return;
}

// displays the next frame
void CONF2__display__frame(GAME__information* game_information) {
    float rotation_speed = 0.5f;
    ESS__world_axis shift = 2;

    // blocks
    BLOCK__block place_block = BLOCK__create__block(CONF2__bft__red_leaves, BLOCK__create_null__metadata());
    BLOCK__block break_block = BLOCK__create__block(CONF2__bft__air, BLOCK__create_null__metadata());

    // update mouse movement
    CONTROLS__update__mouse_position_change(&((*game_information).p_controls));

    // update rotations
    (*game_information).p_camera_rotation.p_vertices[0] += (*game_information).p_controls.p_mouse_position_change.p_mouse_change_y * -1.0f * rotation_speed;
    (*game_information).p_camera_rotation.p_vertices[1] += (*game_information).p_controls.p_mouse_position_change.p_mouse_change_x * rotation_speed;

    // update player position movement (x)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_A)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_x -= ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_D)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_x += ESS__define__bits_per_block__total_count >> shift;
    }

    // update player position movement (y)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_LSHIFT)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_y += ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_SPACE)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_y -= ESS__define__bits_per_block__total_count >> shift;
    }

    // update player position movement (z)
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_W)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_z -= ESS__define__bits_per_block__total_count >> shift;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_S)) {
        (*game_information).p_world_manager.p_positioning.p_camera_position.p_z += ESS__define__bits_per_block__total_count >> shift;
    }

    // block placing
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_E)) {
        // do block placement
        CONF2__do__block_placement(game_information, place_block);
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_Q)) {
        // do block placement
        CONF2__do__block_placement(game_information, break_block);
    }

    // check if player requested quit
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_ESCAPE)) {
        (*game_information).p_game_state = GAME__gsi__game_quitting;
    }

    // update window size
    WINDOW__update__current_window_size(&((*game_information).p_graphics));

    // draw everything
    RENDER__draw__world((*game_information).p_game_textures, (*game_information).p_world_manager.p_rendered_world, (*game_information).p_graphics.p_window_configuration, (*game_information).p_chunks_shader_program, (*game_information).p_world_manager.p_positioning, (*game_information).p_camera_rotation);

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
