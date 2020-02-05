#!/bin/bash
build_path=$1
version=$2
app_name="Strategr"
dmg_path="$build_path/$app_name v$version.dmg"
dmg_template_path="$build_path/../../../deployment/package.dmg"
#entitlements_path="$build_path/../../../deployment/Strategr.entitlements"
dmg_source_path="./DMGContainer"
#
rm -rf "$dmg_path" || true

echo "Deploying macOS application"

echo "Signing with certificate: $DEVELOPER_CERTIFACATE_ID"

~/Qt/5.13.0/clang_64/bin/macdeployqt "$build_path/$app_name.app" \
				-codesign="$DEVELOPER_CERTIFACATE_ID"

echo "App bundle created"

echo "Creating .dmg"

mkdir "$dmg_source_path"
cp -r "$build_path/$app_name.app" "$dmg_source_path/$app_name.app"

hdiutil create "$dmg_template_path" -ov \
				-volname "$app_name" \
				-fs HFS+ \
				-srcfolder "$dmg_source_path"

hdiutil convert "$dmg_template_path" -format UDZO -o "$dmg_path"

codesign -s "$DEVELOPER_CERTIFACATE_ID" \
				--keychain "$HOME/Library/Keychains/login.keychain" "$dmg_path"

rm -rf "$dmg_source_path"

echo ".dmg created"

echo "Creating updater archive"
cd "$build_path" || exit
zip -r "$build_path/macOS_update.zip" "Strategr.app"
echo "Updater archive created"

