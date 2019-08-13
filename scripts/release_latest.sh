#!/usr/bin/env bash
remote_origin_url=$(git config --get remote.origin.url)
repo_name="${remote_origin_url##*.com/}"
repo_name="${repo_name%.git}"

scripts/release.sh "$repo_name" "$(git describe)" -- "./cmake-build-release/Strategr.dmg"