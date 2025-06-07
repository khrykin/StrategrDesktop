#!/bin/bash

# Exit on error
set -e

# Create dependencies directory if it doesn't exist
mkdir -p dependencies

# Clone vcpkg if it doesn't exist
if [ ! -d "dependencies/vcpkg" ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git dependencies/vcpkg
fi

# Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
./dependencies/vcpkg/bootstrap-vcpkg.sh

# Set VCPKG_ROOT environment variable
export VCPKG_ROOT="$(pwd)/dependencies/vcpkg"

# Detect architecture and set triplet
ARCH=$(uname -m)
if [ "$ARCH" = "arm64" ]; then
    TRIPLET="arm64-osx"
else
    TRIPLET="x64-osx"
fi

# Install dependencies for both debug and release
echo "Installing dependencies for $TRIPLET..."
./dependencies/vcpkg/vcpkg install --triplet $TRIPLET

echo "Vcpkg setup complete!"
echo "To use vcpkg in your shell, add this line to your ~/.zshrc or ~/.bashrc:"
echo "export VCPKG_ROOT=\"$(pwd)/dependencies/vcpkg\"" 