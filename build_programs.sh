#!/usr/bin/env sh

echo "Building elf2sun"
gcc elf2sun.c -o elf2sun

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