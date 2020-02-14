#!/bin/bash
build_path=$1
version=$2
app_name="Strategr"
dmg_path="$build_path/$app_name.v$version.dmg"
dmg_template_path="$build_path/../../../deployment/package.dmg"
#entitlements_path="$build_path/../../../deployment/Strategr.entitlements"
dmg_source_path="./DMGContainer"
#
rm -rf "$dmg_path" || true

echo "Deploying macOS application"

echo "Signing with certificate: $DEVELOPER_CERTIFACATE_ID"

codesign -s "$DEVELOPER_CERTIFACATE_ID" \
				--keychain "$HOME/Library/Keychains/login.keychain" \
				"$build_path/$app_name.app/Contents/Frameworks/Sparkle.framework"

~/Qt/5.14.1/clang_64/bin/macdeployqt "$build_path/$app_name.app" \
				-codesign="$DEVELOPER_CERTIFACATE_ID"

echo "App bundle created"

echo "Creating .dmg"

mkdir "$dmg_source_path"
cp -a "$build_path/$app_name.app" "$dmg_source_path/$app_name.app"
ln -s  "/Applications" "$dmg_source_path/Applications"

hdiutil create "$dmg_template_path" -ov \
				-volname "$app_name" \
				-fs HFS+ \
				-srcfolder "$dmg_source_path"

hdiutil convert "$dmg_template_path" -format UDZO -o "$dmg_path"

codesign -s "$DEVELOPER_CERTIFACATE_ID" \
				--keychain "$HOME/Library/Keychains/login.keychain" "$dmg_path"

rm -rf "$dmg_source_path"
rm  "$dmg_template_path"

echo ".dmg created"

echo "Creating updater archive"
cd "$build_path" || exit
zip -r --symlinks "Strategr.macOS.v$version.zip" "Strategr.app"
echo "Updater archive created"

