#include "common.h"

//
// --- Functions ---
//
void mem_set(void *data_pointer, int value, size_t bytes_size) {
    if (bytes_size % 8 == 0) {
        // Zero memory by 8 bytes.
        For (bytes_size / 8) {
            ((u64 *)data_pointer)[it] = value;
        }
    } else if (bytes_size % 4 == 0) {
        // Zero memory by 4 bytes.
        For (bytes_size / 4) {
            ((u32 *)data_pointer)[it] = value;
        }
    } else if (bytes_size % 2 == 0) {
        // Zero memory by 2 bytes.
        For (bytes_size / 2) {
            ((u16 *)data_pointer)[it] = value;
        }
    } else {
        // Zero memory by 1 byte.
        For (bytes_size) {
            ((u8 *)data_pointer)[it] = value;
        }
    }
}

void mem_zero(void *data_pointer, size_t bytes_size) {
    if (bytes_size % 8 == 0) {
        // Zero memory by 8 bytes.
        For (bytes_size / 8) {
            ((u64 *)data_pointer)[it] = 0;
        }
    } else if (bytes_size % 4 == 0) {
        // Zero memory by 4 bytes.
        For (bytes_size / 4) {
            ((u32 *)data_pointer)[it] = 0;
        }
    } else if (bytes_size % 2 == 0) {
        // Zero memory by 2 bytes.
        For (bytes_size / 2) {
            ((u16 *)data_pointer)[it] = 0;
        }
    } else {
        // Zero memory by 1 byte.
        For (bytes_size) {
            ((u8 *)data_pointer)[it] = 0;
        }
    }
}

int string_length(const char *text) {
    int i = 0;
    while (text[i] != '\0') { i++; }
    return i;
}
