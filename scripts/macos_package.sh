#!/bin/bash
build_path=$1
version=$(git describe --abbrev=0)
app_name="Strategr"

echo "Creating updater archive"
cd "$build_path" || exit
zip -r --symlinks "$app_name.macOS.$version.zip" "$app_name.app"
echo "Updater archive created"