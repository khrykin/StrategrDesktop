#!/bin/bash
build_path=$1
version=$2
app_name="Strategr"

echo "Creating updater archive"
cd "$build_path" || exit
zip -r --symlinks "$app_name.macOS.v$version.zip" "$app_name.app"
echo "Updater archive created"