#!/bin/bash

set -e

# Clone vcpkg if it doesn't exist. Check for bootstrap-vcpkg.sh rather than
# just the directory: restoring the vcpkg/downloads cache creates a vcpkg/
# directory before this script runs, which would otherwise fool a
# directory-existence check into skipping the clone. Clone into a scratch
# dir and merge with rsync, since git clone refuses a non-empty target and
# vcpkg/downloads may already be populated from that cache restore.
if [ ! -f "vcpkg/bootstrap-vcpkg.sh" ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git vcpkg-clone
    rsync -a vcpkg-clone/ vcpkg/
    rm -rf vcpkg-clone
fi

echo "Bootstrapping vcpkg..."

./vcpkg/bootstrap-vcpkg.sh

./scripts/generate_qtbase_overlay_port.sh
