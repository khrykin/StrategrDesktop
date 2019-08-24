#!/usr/bin/env bash

scripts/release.sh "$(scripts/repo_name.sh)" "$(git describe)" -- "./cmake-build-release/Strategr.dmg" "./cmake-build-release/macOS_update.zip"