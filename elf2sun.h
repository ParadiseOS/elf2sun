#pragma once

#include <stdint.h>

typedef struct {
    uint32_t text_size;             // Program text size
    char* text_data;                // Program text data

    uint32_t data_size;             // Program data section size
    char* data_data;                // Program data section data

    uint32_t ro_data_size;          // Program read-only size
    char* ro_data_data;             // Program read-only data

    uint32_t bss_size;              // Program bss size
    char* bss_data;                 // Program bss data
    uint32_t bss_alloc_size;        // Program bss allocation size
} Program;

typedef struct {
    char name[16];                  // Null Terminated string
    uint32_t offset;                // Offset from start of file
    uint32_t text_size;             // Size of text section
    uint32_t data_size;             // Size of data section
    uint32_t rodata_size;           // Size of read-only section
    uint32_t bss_size;              // Size of bss section
    uint32_t bss_alloc;             // Size of bss section in memory
} TableEntry;