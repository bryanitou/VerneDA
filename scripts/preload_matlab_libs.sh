#!/bin/bash
echo "PRELOAD MATLAB SCRIPT"

# Get current working directory
cwd_dir=$(pwd)

# Specify the folder containing the .so files
so_folder="$cwd_dir/build/VerneDA-install-debug/lib"

# Check if the folder exists
if [ ! -d "$so_folder" ]; then
  echo "The folder '$so_folder' does not exist."
  exit 1
fi

echo "Searching in .so folder: $so_folder ..."

# STD library problem from LINUX users only
export LD_PRELOAD=$LD_PRELOAD:/lib/x86_64-linux-gnu/libstdc++.so.6 matlab

# Set DACE library
export LD_PRELOAD=$LD_PRELOAD:$cwd_dir/build/VerneDA-install-debug/lib/dace/lib/libdace.so matlab

# Loop through .so files in the folder
for so_file in "$so_folder"/*.so; do
  if [ -f "$so_file" ]; then
    # Set LD_PRELOAD and launch MATLAB for each .so file
    export LD_PRELOAD="$LD_PRELOAD:$so_file" matlab
    echo "Preloading library $so_file to MATLAB..."
  fi
done

matlab