#include <stdio.h>

#include "voxelize/voxelize.h"

int main() {
    // run game loop
    VOX__run__game();
    
    // tell player game is done
    printf("Thanks for playing!\n");

    return 0;
}
