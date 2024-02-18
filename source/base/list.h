#ifndef VOXELIZE__list
#define VOXELIZE__list

/* Include */
#include "basic.h"

/* List */
// list types
typedef u64 LIST__list_filled_index;
typedef u64 LIST__list_increase;

// list object
typedef struct LIST__list {
    BASIC__buffer p_buffer;
    LIST__list_filled_index p_filled_index;
    LIST__list_increase p_increase;
} LIST__list;

// create a list
LIST__list LIST__create__list(BASIC__buffer buffer, LIST__list_filled_index filled_index, LIST__list_increase increase) {
    LIST__list output;

    // setup output
    output.p_buffer = buffer;
    output.p_filled_index = filled_index;
    output.p_increase = increase;

    return output;
}

// create a null list
LIST__list LIST__create_null__list() {
    // return empty list
    return LIST__create__list(BASIC__create_null__buffer(), 0, 0);
}

// open a list
LIST__list LIST__open__list(LIST__list_increase increase, BASIC__bt* error_occured) {
    LIST__list output;
    BASIC__buffer allocation;

    // allocate list
    allocation = BASIC__open__buffer(increase);

    // check list validity
    if (allocation.p_address == 0) {
        // set error
        *error_occured = BASIC__bt__true;

        // return empty
        return LIST__create_null__list();
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
void LIST__close__list(LIST__list list) {
    // free buffer
    BASIC__close__buffer(list.p_buffer);

    return;
}

// expand a list
void LIST__list__expand(LIST__list* list, BASIC__bt* error_occured) {
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
void LIST__list__request__space(LIST__list* list, BASIC__length byte_count, BASIC__bt* error_occured) {
    // expand the list until there is enough space
    while ((u64)(*list).p_buffer.p_length < ((*list).p_filled_index + byte_count)) {
        // expand the list
        LIST__list__expand(list, error_occured);

        // check for error
        if (*error_occured == BASIC__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// calculate the tip of the list
BASIC__address LIST__calculate__list_current(LIST__list* list) {
    return (*list).p_buffer.p_address + (*list).p_filled_index;
}

// add a buffer to a list
void LIST__list__append__buffer(LIST__list* list, BASIC__buffer buffer, BASIC__bt* memory_error_occured) {
    // request space
    LIST__list__request__space(list, sizeof(BASIC__buffer), memory_error_occured);

    // append data
    (*(BASIC__buffer*)LIST__calculate__list_current(list)) = buffer;

    // increase fill
    (*list).p_filled_index += sizeof(BASIC__buffer);

    return;
}

// add a list to a list
void LIST__list__append__list(LIST__list* list, LIST__list data, BASIC__bt* memory_error_occured) {
    // request space
    LIST__list__request__space(list, sizeof(LIST__list), memory_error_occured);

    // append data
    (*(LIST__list*)LIST__calculate__list_current(list)) = data;

    // increase fill
    (*list).p_filled_index += sizeof(LIST__list);

    return;
}

#endif
