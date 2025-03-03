#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int is_elf_file(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(filename);
        return 0;
    }

    unsigned char e_ident[EI_NIDENT];
    if (read(fd, e_ident, EI_NIDENT) != EI_NIDENT) {
        close(fd);
        fprintf(stderr, "%s: Not a valid ElF file (could not read header)\n", filename);
        return 0;
    }

    close(fd);

    if (e_ident[EI_MAG0] == ELFMAG0 &&
        e_ident[EI_MAG1] == ELFMAG1 &&
        e_ident[EI_MAG2] == ELFMAG2 &&
        e_ident[EI_MAG3] == ELFMAG3) {
        return 1;
    }

    fprintf(stderr, "%s: Not a valid ELF file (incrrect magic bytes)\n", filename);
    return 0;
}

int main(int argc, char** argv) {

    // Argument checking
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf_file1> [elf_file2] ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (!is_elf_file(argv[i])) {
            fprintf(stderr, "Error: %s is not a valid ELF file.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    printf("Valid ELF Files...\n");

    return EXIT_SUCCESS;
}