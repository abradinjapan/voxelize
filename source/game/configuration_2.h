#ifndef VOXELIZE__configuration_2
#define VOXELIZE__configuration_2

/* Include */
// game
#include "game.h"

// random number generator
#include <stdlib.h>

/* Define */
// blocks
typedef enum CONF2__bt {
    CONF2__bt__no_block,
    CONF2__bt__air,
    CONF2__bt__stone,
    CONF2__bt__grass,
    CONF2__bt__sand,
    CONF2__bt__dirt,
    CONF2__bt__glass,
    CONF2__bt__tar,
    CONF2__bt__red_leaves,
    CONF2__bt__green_leaves,
    CONF2__bt__oak_log,

    // count
    CONF2__bt__COUNT,
} CONF2__bt;

// block face types
typedef enum CONF2__bft {
    CONF2__bft__no_face,
    CONF2__bft__stone,
    CONF2__bft__grass,
    CONF2__bft__sand,
    CONF2__bft__dirt,
    CONF2__bft__glass,
    CONF2__bft__tar,
    CONF2__bft__red_leaves,
    CONF2__bft__green_leaves,
    CONF2__bft__oak_log_bark, // the front, back, left and right faces
    CONF2__bft__oak_log_core, // the top and bottom faces

    // count
    CONF2__bft__COUNT,
} CONF2__bft;

/* Setup Game Data */
// create game block faces
TEX__faces CONF2__open__block_faces(RANDOM__context* random_context) {
    TEX__faces output;
    TEX__pixel_color color_intensity = 5;

    // open faces
    output = TEX__open__faces(16, 16, CONF2__bft__COUNT);

    // generate faces
    TEX__generate_face__one_color(output, CONF2__bft__no_face, TEX__create__pixel(0, 0, 0, 0)); // no face
    TEX__generate_face__one_color_range(output, CONF2__bft__stone, TEX__create__pixel(120, 120, 120, 255), random_context, color_intensity); // stone face
    TEX__generate_face__one_color_range(output, CONF2__bft__grass, TEX__create__pixel(50, 240, 50, 255), random_context, color_intensity); // grass face
    TEX__generate_face__one_color_range(output, CONF2__bft__sand, TEX__create__pixel(242, 214, 136, 255), random_context, color_intensity); // sand face
    TEX__generate_face__one_color_range(output, CONF2__bft__dirt, TEX__create__pixel(100, 50, 0, 255), random_context, color_intensity); // dirt face
    TEX__generate_face__box_texture(output, CONF2__bft__glass, TEX__create__pixel(245, 245, 245, 255), TEX__create__pixel(0, 0, 0, 0)); // glass face
    TEX__generate_face__one_color_range(output, CONF2__bft__tar, TEX__create__pixel(20, 20, 20, 255), random_context, color_intensity); // tar face
    TEX__generate_face__checkerboard(output, CONF2__bft__red_leaves, TEX__create__pixel(255, 0, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // red leaves face
    TEX__generate_face__checkerboard(output, CONF2__bft__green_leaves, TEX__create__pixel(0, 225, 0, 255), TEX__create__pixel(0, 0, 0, 0)); // green leaves face
    TEX__generate_face__vertical_stripes(output, CONF2__bft__oak_log_bark, TEX__create__pixel(115, 50, 0, 255), TEX__create__pixel(180, 50, 0, 255)); // oak log bark face
    TEX__generate_face__box_texture(output, CONF2__bft__oak_log_core, TEX__create__pixel(115, 50, 0, 255), TEX__create__pixel(255, 150, 80, 255)); // oak log core face

    return output;
}

// create game skins
SKIN__skins CONF2__open__skins() {
    SKIN__skins output;

    // setup output
    output = SKIN__open__block_skins(CONF2__bt__COUNT);

    // setup block skins
    SKIN__set__skin__block(output, CONF2__bt__no_block, SKIN__create__block__one_skin(CONF2__bft__no_face, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, CONF2__bt__air, SKIN__create__block__one_skin(CONF2__bft__no_face, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, CONF2__bt__stone, SKIN__create__block__one_skin(CONF2__bft__stone, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__bt__grass, SKIN__create__block(CONF2__bft__grass, CONF2__bft__grass, CONF2__bft__grass, CONF2__bft__dirt, CONF2__bft__grass, CONF2__bft__grass, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__bt__sand, SKIN__create__block__one_skin(CONF2__bft__sand, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__bt__dirt, SKIN__create__block__one_skin(CONF2__bft__dirt, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__bt__glass, SKIN__create__block__one_skin(CONF2__bft__glass, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, CONF2__bt__tar, SKIN__create__block__one_skin(CONF2__bft__tar, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__bt__red_leaves, SKIN__create__block__one_skin(CONF2__bft__red_leaves, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, CONF2__bt__green_leaves, SKIN__create__block__one_skin(CONF2__bft__green_leaves, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, CONF2__bt__oak_log, SKIN__create__block(CONF2__bft__oak_log_bark, CONF2__bft__oak_log_bark, CONF2__bft__oak_log_core, CONF2__bft__oak_log_core, CONF2__bft__oak_log_bark, CONF2__bft__oak_log_bark, SKIN__bdt__draw_all_sides));

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

/*// create small world
void CONF2__create__test_world_2(GAME__information* game_information, ESS__world_vertex camera_position) {
    CHUNK__chunks_index chunks_size = 3;
    ESS__dimensions chunks_dimensions = ESS__create__dimensions(chunks_size, chunks_size, chunks_size);
    CHUNK__chunk chunk;
    CHUNK__chunk_address temp_chunk = 0;

    // blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block glass_block = BLOCK__create__block(CONF2__bt__glass);
    BLOCK__block grass_block = BLOCK__create__block(CONF2__bt__grass);
    BLOCK__block sand_block = BLOCK__create__block(CONF2__bt__sand);
    BLOCK__block stone_block = BLOCK__create__block(CONF2__bt__stone);
    BLOCK__block green_leaves_block = BLOCK__create__block(CONF2__bt__green_leaves);

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
}*/

// generate chunks based on world position
CHUNK__chunk CONF2__generate_chunks__ground_and_air(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block grass_block = BLOCK__create__block(CONF2__bt__grass);

    // quiet compiler warning
    blueprint = blueprint;

    // return function based on height
    // is above ground
    if (chunk_position.p_y < ESS__calculate__world_center().p_y) {
        // return air
        return CHUNK__create__chunk(air_block);
    // is ground
    } else {
        // return ground
        return CHUNK__create__chunk__3_rotating_block_pattern(air_block, air_block, grass_block);
    }
}

// generate chunks based on world position
CHUNK__chunk CONF2__generate_chunks__floating_sand(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block sand_block = BLOCK__create__block(CONF2__bt__sand);

    // quiet compiler warning
    blueprint = blueprint;

    // return chunk based on position
    if (chunk_position.p_x % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0 && chunk_position.p_y % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0 && chunk_position.p_z % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0) {
        // return sand
        return CHUNK__create__chunk__3_rotating_block_pattern(air_block, air_block, sand_block);
    } else {
        // return air
        return CHUNK__create__chunk(air_block);
    }
}

// generate chunks based on world position
CHUNK__chunk CONF2__generate_chunks__tar_cubes(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block tar_block = BLOCK__create__block(CONF2__bt__tar);

    // quiet compiler warning
    blueprint = blueprint;

    // return chunk based on position
    if (chunk_position.p_x % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0 && chunk_position.p_y % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0 && chunk_position.p_z % (ESS__calculate__chunk_side_size_in_world_coordinates() * 2) == 0) {
        // return sand
        return CHUNK__create__chunk(tar_block);
    } else {
        // return air
        return CHUNK__create__chunk(air_block);
    }
}

// generate chunks based on world position
CHUNK__chunk CONF2__generate_chunks__bars(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block glass_block = BLOCK__create__block(CONF2__bt__glass);

    // quiet compiler warning
    chunk_position = chunk_position;
    blueprint = blueprint;

    // return chunk
    return CHUNK__create__chunk__bars(air_block, glass_block);
}

// generate a tree in a chunk
CHUNK__chunk CONF2__generate_chunks__tree(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    CHUNK__chunk output;

    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block oak_block = BLOCK__create__block(CONF2__bt__oak_log);
    BLOCK__block leaf_block = BLOCK__create__block(CONF2__bt__green_leaves);

    // quiet compiler warning
    chunk_position = chunk_position;
    blueprint = blueprint;

    // clear chunk with air
    output = CHUNK__create__chunk(air_block);

    // create logs
    for (CHUNK__block_index y = 0; y < ESS__define__chunk_side_block_count / 2; y++) {
        // generate log
        output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(7, y, 7))] = oak_block;
    }

    // create leaves
    for (BLOCK__block_x x = 6; x <= 8; x++) {
        for (BLOCK__block_y y = 6; y <= 8; y++) {
            for (BLOCK__block_z z = 6; z <= 8; z++) {
                // if not a log
                if (output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(x, y, z))].p_block_ID != CONF2__bt__oak_log) {
                    // generate leaf
                    output.p_blocks[CHUNK__calculate__block_index(CHUNK__create__block_position(x, y, z))] = leaf_block;
                }
            }
        }
    }

    return output;
}

// generate a flat world
CHUNK__chunk CONF2__generate_chunks__flat_world(ESS__world_vertex chunk_position, GENERATION__blueprint_address blueprint) {
    CHUNK__chunk output;
    ESS__world_vertex world_center = ESS__calculate__world_center();

    // setup blocks
    BLOCK__block air_block = BLOCK__create__block(CONF2__bt__air);
    BLOCK__block grass_block = BLOCK__create__block(CONF2__bt__grass);
    BLOCK__block stone_block = BLOCK__create__block(CONF2__bt__stone);
    BLOCK__block oak_block = BLOCK__create__block(CONF2__bt__oak_log);
    BLOCK__block leaf_block = BLOCK__create__block(CONF2__bt__green_leaves);

    // quiet compiler warning
    blueprint = blueprint;

    // generate world
    // if an above ground chunk
    if (chunk_position.p_y < world_center.p_y + ESS__calculate__chunk_side_size_in_world_coordinates()) {
        // return an air chunk
        output = CHUNK__create__chunk(air_block);
    // if the ground surface chunk
    } else if (chunk_position.p_y == world_center.p_y + ESS__calculate__chunk_side_size_in_world_coordinates()) {
        // make a grass chunk
        output = CHUNK__create__chunk(grass_block);
    // otherwise, it is a stone chunk
    } else {
        output = CHUNK__create__chunk(stone_block);
    }

    return output;
}

// setup game
void CONF2__setup__game(GAME__information* game_information) {
    ESS__world_vertex camera_position = ESS__calculate__world_center();//ESS__create__world_vertex(ESS__calculate__chunk_side_size_in_world_coordinates() * 10, ESS__calculate__chunk_side_size_in_world_coordinates() * 10, ESS__calculate__chunk_side_size_in_world_coordinates() * 10);

    // setup textures
    (*game_information).p_game_textures = TEX__open__game_textures(CONF2__open__block_faces(&((*game_information).p_random_pixel_context)), (*game_information).p_chunks_shader_program);

    // setup skins
    (*game_information).p_skins = CONF2__open__skins();

    // open world manager
    (*game_information).p_world_manager = MANAGER__open__world_manager(&CONF2__generate_chunks__flat_world, ESS__create__dimensions(5, 5, 5), camera_position);

    // generate chunks
    MANAGER__initialize__world((*game_information).p_world_manager, (*game_information).p_world_manager.p_positioning.p_camera_position, (*game_information).p_skins, (*game_information).p_temporaries);

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
    MANAGER__chunk_body_slot_index chunks_index = -1;
    BASIC__bt in_chunk = BASIC__bt__false;

    // find chunk
    for (MANAGER__chunk_body_slot_index i = 0; i < (*game_information).p_world_manager.p_positioning.p_chunk_body_count; i++) {
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
        // get block position and index
        CHUNK__block_position block_position = POS__calculate__block_local_position_from_world_position(((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index], (*game_information).p_world_manager.p_positioning.p_camera_position);
        CHUNK__block_index block_index = CHUNK__calculate__block_index(block_position);

        /*// DEBUG
        printf("Placing Block!\n\tCamera Position: ");
        ESS__print__world_vertex((*game_information).p_world_manager.p_positioning.p_camera_position);
        printf("\n\tChunk Position: ");
        ESS__print__world_vertex(((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index]);
        printf("\n\tDifference: ");
        ESS__print__world_vertex(ESS__calculate__subtract_world_vertices((*game_information).p_world_manager.p_positioning.p_camera_position, ((ESS__world_vertex*)(*game_information).p_world_manager.p_positioning.p_chunk_body_positions.p_address)[chunks_index]));
        printf("\n");*/

        // update block
        ((CHUNK__chunk*)(*game_information).p_world_manager.p_chunks.p_chunk_block_data.p_address)[chunks_index].p_blocks[block_index] = block;

        // rerender affected chunk body
        RENDER__render__chunk_body((*game_information).p_skins, CHUNK__get__chunk_pointer_in_chunks((*game_information).p_world_manager.p_chunks, chunks_index), chunks_index, (*game_information).p_world_manager.p_rendered_world, (*game_information).p_temporaries);

        // rerender surfaces
        // get surfaces
        //MANAGER__chunk_YZ_surface_slot_index left_surface = MANAGER__find__unavaliable_slot_index((*game_information).p_world_manager.p_positioning.p_chunk_YZ_surface_count, (*game_information).p_world_manager.p_chunk_YZ_surface_slots, (*game_information).p_world_manager.p_positioning.p_chunk_YZ_surface_positions, );
    }

    return;
}

// displays the next frame
void CONF2__display__frame(GAME__information* game_information) {
    float rotation_speed = 0.5f;
    ESS__world_axis shift = 2;

    // blocks
    BLOCK__block place_block = BLOCK__create__block(CONF2__bt__red_leaves);
    BLOCK__block break_block = BLOCK__create__block(CONF2__bt__air);

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

    // update world
    MANAGER__update__world((*game_information).p_world_manager, (*game_information).p_skins, (*game_information).p_temporaries);

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
