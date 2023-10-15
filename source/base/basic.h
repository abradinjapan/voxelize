#ifndef VOXELIZE__basic
#define VOXELIZE__basic

/* Include */
// C
#include <stdint.h>
#include <stdlib.h>

/* Define */
// base types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

// buffer & allocation types
typedef void* BASIC__address;
typedef u64 BASIC__length;
typedef u64 BASIC__data;

// boolean type
typedef enum BASIC__bt {
    // boolean types
    BASIC__bt__false,
    BASIC__bt__true,
    BASIC__bt__LIMIT,

    // count
    BASIC__bt__COUNT = BASIC__bt__LIMIT,
} BASIC__bt;

/* Allocate */
// create allocation
BASIC__address BASIC__open__allocation(BASIC__length length) {
    // return allocation pointer
    return (BASIC__address)malloc(length);
}

// destroy allocation
void BASIC__close__allocation(BASIC__address address, BASIC__length length) {
    // quiet compiler
    length = length;
    
    // return memory
    free(address);

    return;
}

/* Buffer */
typedef struct BASIC__buffer {
    // virtual pointer to first byte of allocation
    BASIC__address p_address;

    // length of memory in bytes
    BASIC__length p_length;
} BASIC__buffer;

// deallocate buffer
void BASIC__destroy__buffer(BASIC__buffer buffer) {
    // close buffer allocation
    BASIC__close__allocation(buffer.p_address, buffer.p_length);

    return;
}

// create buffer with anything as internal data
BASIC__buffer BASIC__create_custom__buffer(BASIC__address address, BASIC__length length) {
    BASIC__buffer output;

    // setup output
    output.p_address = address;
    output.p_length = length;

    return output;
}

// create custom buffer as [ 0, 0 ]
BASIC__buffer BASIC__create_null__buffer() {
    return BASIC__create_custom__buffer(0, 0);
}

// allocate buffer with 'length' size in bytes
BASIC__buffer BASIC__create__buffer(BASIC__length length) {
    return BASIC__create_custom__buffer(BASIC__open__allocation(length), length);
}

// create buffer copy with null termination
BASIC__buffer BASIC__create__null_terminated_copy(BASIC__buffer subject) {
    BASIC__buffer output;

    // setup output
    output = BASIC__create__buffer(subject.p_length + sizeof(u8));

    // check for allocation failure
    if (output.p_address == 0) {
        // return null
        return BASIC__create_null__buffer();
    }

    // copy data to new buffer
    for (u64 index = 0; index < subject.p_length; index++) {
        // copy byte
        ((u8*)output.p_address)[index] = ((u8*)subject.p_address)[index];
    }

    // add null termination
    ((u8*)output.p_address)[subject.p_length] = 0;

    // return new buffer
    return output;
}

// treat a static c string as a buffer with a length, including null termination optional
BASIC__buffer BASIC__create__c_string_reference(const char* c_string, BASIC__bt include_null_termination) {
    u64 length;

    // setup variables
    length = 0;

    // get string length
    while (c_string[length] != 0) {
        length++;
    }

    // if null termination desired in buffer length
    if (include_null_termination == BASIC__bt__true) {
        // include null termination
        length++;
    }

    // return buffer
    return BASIC__create_custom__buffer((BASIC__address)c_string, length);
}

// create buffer from file
BASIC__buffer BASIC__create__buffer_from_file(BASIC__bt* buffer_error_occured, BASIC__bt* file_open_error_occured, BASIC__bt* file_read_error_occured, BASIC__buffer file_address) {
    BASIC__buffer output;
    BASIC__buffer temp;
    size_t read_length;
    FILE* f;

    // setup output
    output = BASIC__create_null__buffer();

    // create temporary file name buffer with null termination
    temp = BASIC__create__buffer(file_address.p_length + 1);
    if (temp.p_address == 0) {
        // set error type
        *buffer_error_occured = BASIC__bt__true;

        // return output
        return BASIC__create_null__buffer();
    } else {
        // set error as false
        *buffer_error_occured = BASIC__bt__false;
    }

    // copy file name to temp file name
    for (u64 i = 0; i < file_address.p_length; i++) {
        ((u8*)temp.p_address)[i] = ((u8*)file_address.p_address)[i];
    }

    // add null termination
    ((u8*)temp.p_address)[temp.p_length - 1] = 0;

    // attempt to load file
    f = fopen((const char*)temp.p_address, "r");
    if (f == 0) {
        *file_open_error_occured = BASIC__bt__true;

        goto BASIC__label__clean_up;
    } else {
        *file_open_error_occured = BASIC__bt__false;
    }

    // get file length
    fseek(f, 0, SEEK_END);
    output.p_length = ftell(f);
    fseek(f, 0, SEEK_SET);

    // allocate output buffer
    output = BASIC__create__buffer(output.p_length);
    if (output.p_address == 0) {
        // set error type
        *buffer_error_occured = BASIC__bt__true;

        // return output
        return BASIC__create_null__buffer();
    } else {
        // set error as false
        *buffer_error_occured = BASIC__bt__false;
    }

    // get file data
    read_length = fread(output.p_address, 1, output.p_length, f);
    if (read_length != output.p_length) {
        *file_read_error_occured = BASIC__bt__true;
    } else {
        *file_read_error_occured = BASIC__bt__false;
    }

    // close the file handle
    fclose(f);

    // free temp file name
    BASIC__label__clean_up:
    BASIC__destroy__buffer(temp);

    return output;
}

// write buffer to file
void BASIC__create__file_from_buffer(BASIC__bt* buffer_error_occured, BASIC__bt* file_open_error_occured, BASIC__bt* file_read_error_occured, BASIC__buffer file_address, BASIC__buffer file_data) {
    BASIC__buffer temp;
    size_t write_length;
    FILE* f;

    // create temporary file name buffer with null termination
    temp = BASIC__create__buffer(file_address.p_length + 1);
    if (temp.p_address == 0) {
        // set error type
        *buffer_error_occured = BASIC__bt__true;

        // return output
        return;
    } else {
        // set error as false
        *buffer_error_occured = BASIC__bt__false;
    }

    // copy file name to temp file name
    for (u64 i = 0; i < file_address.p_length; i++) {
        ((u8*)temp.p_address)[i] = ((u8*)file_address.p_address)[i];
    }

    // add null termination
    ((u8*)temp.p_address)[temp.p_length - 1] = 0;

    // attempt to load file
    f = fopen((const char*)temp.p_address, "w+b");
    if (f == 0) {
        *file_open_error_occured = BASIC__bt__true;

        goto BASIC__label__clean_up;
    } else {
        *file_open_error_occured = BASIC__bt__false;
    }

    // set file data
    write_length = fwrite(file_data.p_address, file_data.p_length, 1, f);
    if (write_length != file_data.p_length) {
        *file_read_error_occured = BASIC__bt__true;
    } else {
        *file_read_error_occured = BASIC__bt__false;
    }

    // close the file handle
    fclose(f);

    // free temp file name
    BASIC__label__clean_up:
    BASIC__destroy__buffer(temp);

    return;
}

// create buffer from file as c string
BASIC__buffer BASIC__open__buffer_from_file_as_c_string(BASIC__bt* file_error, BASIC__buffer file_path) {
    BASIC__buffer output;
    BASIC__buffer file_data;

    // setup other variables
    *file_error = BASIC__bt__false;

    // open file
    file_data = BASIC__create__buffer_from_file(file_error, file_error, file_error, file_path);

    // create copy with null termination appended
    output = BASIC__create__null_terminated_copy(file_data);

    // free file data
    BASIC__destroy__buffer(file_data);

    return output;
}

// read data from a buffer into a variable
BASIC__data BASIC__read__buffer(BASIC__address address, BASIC__length byte_count) {
    BASIC__data output;

    // setup output
    output = 0;

    // read data to output
    for (BASIC__length i = 0; i < byte_count; i++) {
        // read byte
        ((u8*)(&output))[i] = ((u8*)(address))[i];
    }

    // return data
    return output;
}

// write data from a variable into a buffer
void BASIC__write__buffer(BASIC__data data, BASIC__address address, BASIC__length byte_count) {
    // write data to buffer
    for (BASIC__length i = 0; i < byte_count; i++) {
        // write byte
        ((u8*)(address))[i] = ((u8*)(&data))[i];
    }

    return;
}

#endif
