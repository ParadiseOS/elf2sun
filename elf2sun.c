#define _XOPEN_SOURCE 500 // For pread
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include "elf2sun.h"

/**
 * @brief Checks if a file is a valid elf exe
 * @param filename The name of the file
 * @return int 1 if valid, 0 if not
 */
int is_valid_elf_file(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(filename);
        return 0;
    }

    unsigned char e_ident[EI_NIDENT];
    if (read(fd, e_ident, EI_NIDENT) != EI_NIDENT) {
        close(fd);
        fprintf(stderr, "%s: Not a valid ELF file (could not read header)\n", filename);
        return 0;
    }

    Elf32_Half e_type;
    if (read(fd, &e_type, sizeof(Elf32_Half)) != sizeof(Elf32_Half)) {
        close(fd);
        fprintf(stderr, "%s: Not a valid ELF file (could not read type)\n", filename);
        return 0;
    }

    Elf32_Half e_machine;
    if (read(fd, &e_machine, sizeof(Elf32_Half)) != sizeof(Elf32_Half)) {
        close(fd);
        fprintf(stderr, "%s: Not a valid ELF file (could not read machine)\n", filename);
        return 0;
    }

    close(fd);

    if (e_ident[EI_MAG0] != ELFMAG0 ||
        e_ident[EI_MAG1] != ELFMAG1 ||
        e_ident[EI_MAG2] != ELFMAG2 ||
        e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "%s: Not a valid ELF file (incrrect magic bytes)\n", filename);
        return 0;
    }

    if (e_ident[EI_CLASS] != ELFCLASS32) {
        fprintf(stderr, "%s: Not a valid ELF file (not 32-bit)\n", filename);
        return 0;
    }
    
    if (e_machine != EM_386) {
        fprintf(stderr, "%s: Not a valid ELF file (not Intel x86)\n", filename);
        return 0;
    }

    return 1;
}

/**
 * @brief Create a program struct of a file
 * @param filename The name of the file
 * @return Program* 
 */
Program* parse_program(const char* filename) {
    Program* program = (Program*)malloc(sizeof(Program));

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(filename);
        return 0;
    }

    // Read header
    Elf32_Ehdr elf_header;
    pread(fd, &elf_header, sizeof(Elf32_Ehdr), 0); // Reads header

    // Read section headers
    Elf32_Shdr section_headers[elf_header.e_shnum];
    pread(fd, section_headers, sizeof(Elf32_Shdr) * elf_header.e_shnum, elf_header.e_shoff);

    // Read program headers
    Elf32_Phdr program_headers[elf_header.e_phnum];
    pread(fd, program_headers, sizeof(Elf32_Phdr) * elf_header.e_phnum, elf_header.e_phoff);

    // Read section header string table
    Elf32_Shdr sh_strtab = section_headers[elf_header.e_shstrndx];
    char* sh_strs = malloc(sh_strtab.sh_size);
    pread(fd, sh_strs, sh_strtab.sh_size, sh_strtab.sh_offset);

    for (int i = 0; i < elf_header.e_shnum; i++) {
        char* name = &sh_strs[section_headers[i].sh_name];

        if (strcmp(name, ".text") == 0) {                                       // Text Section
            program->text_size = section_headers[i].sh_size;
            program->text_data = malloc(program->text_size);
            Elf32_Off text_offset = section_headers[i].sh_offset;
            pread(fd, program->text_data, program->text_size, text_offset);
        } else if (strcmp(name, ".data") == 0) {                                // Data Section
            program->data_size = section_headers[i].sh_size;
            program->data_data = malloc(program->data_size);
            Elf32_Off data_offset = section_headers[i].sh_offset;
            pread(fd, program->data_data, program->data_size, data_offset);
        } else if (strcmp(name, ".rodata") == 0) {                              // Read-only Section
            program->rodata_size = section_headers[i].sh_size;
            program->rodata_data = malloc(program->rodata_size);
            Elf32_Off rodata_offset = section_headers[i].sh_offset;
            pread(fd, program->rodata_data, program->rodata_size, rodata_offset);
        } else if (strcmp(name, ".bss") == 0) {                                 // BSS Section
            program->bss_size = section_headers[i].sh_size;
            for (int k = 0; k < elf_header.e_phnum; k++) {
                Elf32_Phdr* ph = &program_headers[k];

                if (ph->p_type == PT_LOAD &&
                    section_headers[i].sh_addr >= ph->p_vaddr &&
                    section_headers[i].sh_addr < ph->p_vaddr + ph->p_memsz) {
                    program->bss_alloc_size = ph->p_memsz;
                }
            }
        }
    }

    free(sh_strs);
    close(fd);

    return program;
}

int main(int argc, char** argv) {

    // Argument checking
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf_file1> [elf_file2] ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 257) {
        fprintf(stderr, "Too many arguments...\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (!is_valid_elf_file(argv[i])) {
            fprintf(stderr, "Error: %s is not a valid ELF file.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    printf("Valid ELF Files...\n");

    int executable_count = argc-1;
    Program* program[executable_count];

    for (int i = 1; i < argc; i++) {
        printf("Collecting data on %s...\n", argv[i]);
        program[i-1] = parse_program(argv[i]);
    }

    printf("Constructing sun executable\n");
    // Create new file
    // Add magic bytes
    // Add executable count in 1 byte

    for (int i = 0; i < executable_count; i++) {
        free(program[i]->text_data);
        free(program[i]->data_data);
        free(program[i]->rodata_data);
        free(program[i]);
    }

    return EXIT_SUCCESS;
}