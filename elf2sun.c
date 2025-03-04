#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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
 */
Program* parse_program(const char* filename) {

}

int main(int argc, char** argv) {

    // Argument checking
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf_file1> [elf_file2] ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 256) {
        fprintf(stderr, "Too many executables...\n");
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
        program[i] = parse_program(argv[i]);
    }

    printf("Constructing Sun file\n");

    return EXIT_SUCCESS;
}