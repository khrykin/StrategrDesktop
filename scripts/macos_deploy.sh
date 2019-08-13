#!/bin/bash
build_path=$1
codesign_id=$(cat "$build_path/../scripts/cert_id.txt")
dmg_path="$build_path/Strategr.dmg"
rm -rf "$dmg_path" || true
echo "Deploying macOS application"
~/Qt/5.13.0/clang_64/bin/macdeployqt "$build_path/Strategr.app" -dmg -codesign="$codesign_id"
echo "App bundle and .dmg created"
echo "Creating updater archive"
cd "$build_path" || exit
zip -r "$build_path/macOS_update.zip" "Strategr.app"
echo "Updater archive created"

