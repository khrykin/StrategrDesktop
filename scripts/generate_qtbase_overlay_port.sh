#!/bin/bash

# Regenerates ports/qtbase, our overlay port for qtbase. It is not committed
# because it's almost entirely a verbatim copy of vcpkg's own upstream
# qtbase port -- committing it would mean tracking generated/derived files.
#
# Instead we commit only what's genuinely ours in qtbase-overlay-patches/:
# a couple of small diffs against upstream's portfile.cmake and vcpkg.json,
# and our own .patch files that those diffs wire in. This script extracts
# the pinned upstream port (at vcpkg.json's builtin-baseline commit, out of
# the vcpkg clone set up by setup_vcpkg.sh) and layers our changes on top.

set -e

baseline=$(grep -o '"builtin-baseline"[[:space:]]*:[[:space:]]*"[0-9a-f]*"' vcpkg.json | grep -o '[0-9a-f]\{40\}')

if [ -z "$baseline" ]; then
    echo "Could not find builtin-baseline in vcpkg.json" >&2
    exit 1
fi

echo "Generating ports/qtbase from upstream vcpkg qtbase port at $baseline..."

rm -rf ports/qtbase
git -C vcpkg archive "$baseline" -- ports/qtbase | tar -x -C .

patch -p1 -d ports/qtbase < qtbase-overlay-patches/portfile.cmake.diff
patch -p1 -d ports/qtbase < qtbase-overlay-patches/vcpkg.json.diff

cp qtbase-overlay-patches/fix-yieldcpu-arm-acle.patch ports/qtbase/
cp qtbase-overlay-patches/fix-missing-agl-framework.patch ports/qtbase/
cp qtbase-overlay-patches/fix-missing-agl-framework-mkspec.patch ports/qtbase/
