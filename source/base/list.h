#ifndef VOXELIZE__list
#define VOXELIZE__list

/* Include */
#include "basic.h"

/* List */
// list types
typedef u64 LIST__list_filled_index;
typedef u64 LIST__list_increase;

// list object
typedef struct ANVIL__list {
    BASIC__buffer p_buffer;
    LIST__list_filled_index p_filled_index;
    LIST__list_increase p_increase;
} ANVIL__list;

// create a list
ANVIL__list ANVIL__create__list(BASIC__buffer buffer, LIST__list_filled_index filled_index, LIST__list_increase increase) {
    ANVIL__list output;

    // setup output
    output.p_buffer = buffer;
    output.p_filled_index = filled_index;
    output.p_increase = increase;

    return output;
}

// create a null list
ANVIL__list ANVIL__create_null__list() {
    // return empty list
    return ANVIL__create__list(BASIC__create_null__buffer(), 0, 0);
}

// open a list
ANVIL__list ANVIL__open__list(LIST__list_increase increase, BASIC__bt* error_occured) {
    ANVIL__list output;
    BASIC__buffer allocation;

    // allocate list
    allocation = BASIC__open__buffer(increase);

    // check list validity
    if (allocation.p_address == 0) {
        // set error
        *error_occured = BASIC__bt__true;

        // return empty
        return ANVIL__create_null__list();
    // list is valid
    } else {
        // set error to false
        *error_occured = BASIC__bt__false;
    }

    // setup output
    output.p_buffer = allocation;
    output.p_filled_index = 0;
    output.p_increase = increase;

    return output;
}

// destroy a list
void ANVIL__close__list(ANVIL__list list) {
    // free buffer
    BASIC__close__buffer(list.p_buffer);

    return;
}

// expand a list
void ANVIL__list__expand(ANVIL__list* list, BASIC__bt* error_occured) {
    LIST__list_filled_index new_size;
    BASIC__buffer new_allocation;

    // calculate new buffer size
    new_size = ((u64)(*list).p_buffer.p_length) + (*list).p_increase;

    // request new memory
    new_allocation = BASIC__open__buffer(new_size);

    // check for failure
    if (new_allocation.p_address == 0) {
        // set error
        *error_occured = BASIC__bt__true;

        // return unmodified list
        return;
    }

    // copy old data to new list
    for (LIST__list_filled_index i = 0; i < (*list).p_filled_index; i++) {
        // copy one byte
        ((u8*)new_allocation.p_address)[i] = ((u8*)(*list).p_buffer.p_address)[i];
    }

    // free old buffer
    BASIC__close__buffer((*list).p_buffer);

    // setup new list allocation
    (*list).p_buffer = new_allocation;

    return;
}

// request space for the list
void ANVIL__list__request__space(ANVIL__list* list, BASIC__length byte_count, BASIC__bt* error_occured) {
    // expand the list until there is enough space
    while ((u64)(*list).p_buffer.p_length < ((*list).p_filled_index + byte_count)) {
        // expand the list
        ANVIL__list__expand(list, error_occured);

        // check for error
        if (*error_occured == BASIC__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// calculate the tip of the list
BASIC__address ANVIL__calculate__list_current(ANVIL__list* list) {
    return (*list).p_buffer.p_address + (*list).p_filled_index;
}

// add a buffer to a list
void ANVIL__list__append__buffer(ANVIL__list* list, BASIC__buffer buffer, BASIC__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(BASIC__buffer), memory_error_occured);

    // append data
    (*(BASIC__buffer*)ANVIL__calculate__list_current(list)) = buffer;

    // increase fill
    (*list).p_filled_index += sizeof(BASIC__buffer);

    return;
}

// add a list to a list
void ANVIL__list__append__list(ANVIL__list* list, ANVIL__list data, BASIC__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(ANVIL__list), memory_error_occured);

    // append data
    (*(ANVIL__list*)ANVIL__calculate__list_current(list)) = data;

    // increase fill
    (*list).p_filled_index += sizeof(ANVIL__list);

    return;
}

#endif
