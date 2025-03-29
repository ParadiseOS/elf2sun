#!/usr/bin/env sh

# This script builds all c files in programs/ as a 32-bit ELF executable and puts them in build/

# Iterate over subdirectories in programs/
for dir in programs/*/; do
  if [ -d "$dir" ]; then
    if [ -f "$dir/build.sh" ]; then
      echo "Running build.sh in $dir"
      (cd "$dir" && bash build.sh)
    else
      echo "No build.sh found in $dir, skipping..."
    fi
  fi
done

# Run elf2sun with all .out files in build/
echo "Running elf2sun with output files in build/"
./elf2sun build/*.out

# To build in 32 bit you need multilib gcc