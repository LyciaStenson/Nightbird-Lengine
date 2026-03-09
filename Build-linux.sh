#!/bin/bash

echo "Building Linux project files."
premake5 gmake
if [ $? -ne 0 ]; then
    echo "Failed to build project files."
    exit 1
fi

echo "Building..."
make -j$(nproc) config=debug_desktop
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

echo "Built successfully."
