# When compiling each program elf2sun will attempt to build from a given build.sh in the root of the program subfolder
# Here is an example of the flags needed to build a file main.c in a program folder called example
gcc main.c -T ../linker.ld -o ../../build/example.out -Wall -Wextra -fno-pic -fno-asynchronous-unwind-tables -m32 -static -nostdlib -Wl,--build-id=none

# Here is an example of the building that same file but using the ParadiseOS Standard Library in the program
gcc -c main.c -o main.o -Wall -Wextra -fno-pic -fno-asynchronous-unwind-tables -m32 -masm=intel -I../../../libp/include/
gcc main.o ../libp.o -T ../linker.ld -o ../../build/example.out -m32 -static -nostdlib -Wl,--build-id=none

# Ensure that you use the provided linker and put the exe in build/

# Other object files you can include
# start.o - This is an alternate entry instead of main
# libpm.o - This is the ParadiseOS math library