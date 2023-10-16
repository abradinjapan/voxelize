#ifndef VOXELIZE__skin
#define VOXELIZE__skin

/* Include */
// texture information
#include "textures.h"

/* Define */
typedef TEX__block_face_ID SKIN__skin;
typedef u64 SKIN__skin_count;

/* Skin */
// block skin type
typedef enum SKIN__bst {
    SKIN__bst__left,
    SKIN__bst__right,
    SKIN__bst__top,
    SKIN__bst__bottom,
    SKIN__bst__back,
    SKIN__bst__front,

    SKIN__bst__COUNT,
} SKIN__bst;

// block draw type
typedef enum SKIN__bdt {
    SKIN__bdt__dont_draw,
    SKIN__bdt__draw_only_one_side,
    SKIN__bdt__draw_all_sides,

    SKIN__bdt__COUNT,
} SKIN__bdt;

// a container for a texture's z coords for each block's 6 sides
typedef struct SKIN__block {
    SKIN__skin p_faces[SKIN__bst__COUNT];
    SKIN__bdt p_draw_type;
} SKIN__block;

// create block skin data
SKIN__block SKIN__create__block(SKIN__skin left, SKIN__skin right, SKIN__skin top, SKIN__skin bottom, SKIN__skin back, SKIN__skin front, SKIN__bdt draw_type) {
    SKIN__block output;

    // setup output
    output.p_faces[SKIN__bst__left] = left;
    output.p_faces[SKIN__bst__right] = right;
    output.p_faces[SKIN__bst__top] = top;
    output.p_faces[SKIN__bst__bottom] = bottom;
    output.p_faces[SKIN__bst__back] = back;
    output.p_faces[SKIN__bst__front] = front;
    output.p_draw_type = draw_type;

    return output;
}

// create block skin data with only one skin
SKIN__block SKIN__create__block__one_skin(SKIN__skin skin, SKIN__bdt draw_type) {
    // return block skin
    return SKIN__create__block(skin, skin, skin, skin, skin, skin, draw_type);
}

/* Skins */
// all game skins
typedef struct SKIN__skins {
    BASIC__buffer p_block_skins;
    SKIN__skin_count p_block_skins_count;
} SKIN__skins;

// create game skins
SKIN__skins SKIN__create__skins(BASIC__buffer block_skins, SKIN__skin_count block_skin_count) {
    SKIN__skins output;

    // setup output
    output.p_block_skins = block_skins;
    output.p_block_skins_count = block_skin_count;

    return output;
}

// open block skins
SKIN__skins SKIN__open__block_skins(SKIN__skin_count skin_count) {
    SKIN__skins output;

    // setup allocation
    output.p_block_skins = BASIC__open__buffer(sizeof(SKIN__block) * skin_count);

    // setup skin count upon allocation success
    if (output.p_block_skins.p_address != 0) {
        output.p_block_skins_count = skin_count;
    // nullify skin count in case of allocation failure
    } else {
        output.p_block_skins_count = 0;
    }

    return output;
}

// destroy game skins
void SKIN__close__skins(SKIN__skins skins) {
    // free memory
    BASIC__close__buffer(skins.p_block_skins);

    return;
}

// get a block skin out of all skins
SKIN__block SKIN__get__skin__block(SKIN__skins skins, SKIN__skin_count block_skin_index) {
    // return block skin
    return ((SKIN__block*)skins.p_block_skins.p_address)[block_skin_index];
}

// set a block skin in a block skin array
void SKIN__set__skin__block(SKIN__skins skins, SKIN__skin_count block_skin_index, SKIN__block block) {
    // set block skin
    ((SKIN__block*)skins.p_block_skins.p_address)[block_skin_index] = block;

    return;
}

#endif
