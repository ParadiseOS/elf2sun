#!/usr/bin/env sh

# This script builds all c files in programs/ as a 32-bit ELF executable and puts them in build/

for file in programs/*.c; do
  [ -f "$file" ] || continue
  filename=$(basename -- "$file" .c)
  gcc "$file" -o "build/$filename.out" -Wall -Wextra -m32 -static -nostdlib
done

# To build in 32 bit you need multilib gcc