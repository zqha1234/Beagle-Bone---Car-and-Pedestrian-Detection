#ifndef SHAREDMEM_H
#define SHAREDMEM_H


#include <stdbool.h>
#include <stdint.h>
// WARNING:
// Fields in the struct must be aligned to match ARM's alignment
// bool/char, uint8_t: byte aligned
// int/long, uint32_t: word (4 byte) aligned
// double, uint64_t: dword (8 byte) aligned
// Add padding fields (char _p1) to pad out to alignment.

enum LED_COLOR_OPTIONS {
    RED,
    GREEN,
    BLUE,
    NUM_COLOR_OPTIONS
};

// My Shared Memory Structure
// ----------------------------------------------------------------
typedef struct {
    bool isProgramFinished;
    uint8_t pad1, pad2, pad3;
    int32_t LEDColorOption;
    int32_t LEDDisplayOption;
} sharedMemStruct_t;


#endif
