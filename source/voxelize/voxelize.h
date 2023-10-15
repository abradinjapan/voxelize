#ifndef VOXELIZE__voxelize
#define VOXELIZE__voxelize

/* Include */
// game configurations
#include "../game/configuration_2.h"

/* Play */
// run the game
void VOX__run__game() {
    GAME__information game_information;

    // create game
    game_information = CONF2__open__game();
    if (game_information.p_error_occured == BASIC__bt__true) {
        // inform of initialization failure
        printf("Game initialization failure; Terminating...\n");
        
        return;
    }

    // setup game
    CONF2__setup__game(&game_information);

    // run game
    while (SDL_QuitRequested() == SDL_FALSE && game_information.p_game_state != GAME__gsi__game_quitting && CONF2__check__error(&game_information) == BASIC__bt__false) {
        // simulate world
        //CONF2__simulate__game();
        
        // display frame
        CONF2__display__frame(&game_information);
    }

    // clean up
    CONF2__close__game(game_information);

    return;
}

#endif
