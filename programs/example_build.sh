# When compiling each program elf to sun will attempt to build from a given build.sh in the root of the program subfolder
# Here is an example of the flags needed to build a file main.c in a program folder called example.out
gcc main.c -T ../linker.ld -o ../../build/example.out -Wall -Wextra -m32 -static -nostdlib -Wl,--build-id=none