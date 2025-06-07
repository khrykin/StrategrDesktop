#!/bin/bash

set -e

# Clone vcpkg if it doesn't exist
if [ ! -d "vcpkg" ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git vcpkg
fi

echo "Bootstrapping vcpkg..."

./vcpkg/bootstrap-vcpkg.sh
