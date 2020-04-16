#!/bin/bash
build_path=$1
version=$2
app_name="Strategr"
dmg_path="$build_path/$app_name.v$version.dmg"
dmg_template_path="$build_path/../../../deployment/package.dmg"
#entitlements_path="$build_path/../../../deployment/Strategr.entitlements"
dmg_source_path="./DMGContainer"
app_bundle_path="$build_path/$app_name.app"

rm -rf "$dmg_path" || true

echo "Deploying macOS application"

echo "Signing with certificate: $DEVELOPER_CERTIFACATE_ID"

~/Qt/5.14.1/clang_64/bin/macdeployqt "$app_bundle_path"

install_name_tool -delete_rpath /Library/Frameworks "$app_bundle_path/Contents/MacOS/Strategr"
install_name_tool -rpath "$app_bundle_path/Contents/Frameworks" "@executable_path/Contents/Frameworks" "$app_bundle_path/Contents/MacOS/Strategr"

codesign --deep --force --verify --verbose -s "$DEVELOPER_CERTIFACATE_ID" \
        --options runtime \
				--keychain "$HOME/Library/Keychains/login.keychain" \
				"$app_bundle_path/Contents/Frameworks/Sparkle.framework"

codesign --deep --force --verify --verbose -s "$DEVELOPER_CERTIFACATE_ID" \
        --options runtime \
				--keychain "$HOME/Library/Keychains/login.keychain" \
				"$app_bundle_path/Contents/Frameworks/Sparkle.framework/Versions/A/Resources/Autoupdate.app"

codesign --deep --force --verify --verbose -s "$DEVELOPER_CERTIFACATE_ID" \
        --options runtime \
				--keychain "$HOME/Library/Keychains/login.keychain" \
				"$app_bundle_path"

echo "App bundle created"

echo "Creating .dmg"

mkdir "$dmg_source_path"
cp -a "$app_bundle_path" "$dmg_source_path/$app_name.app"
ln -s  "/Applications" "$dmg_source_path/Applications"

hdiutil create "$dmg_template_path" -ov \
				-volname "$app_name" \
				-fs HFS+ \
				-srcfolder "$dmg_source_path"

hdiutil convert "$dmg_template_path" -format UDZO -o "$dmg_path"

codesign -s "$DEVELOPER_CERTIFACATE_ID" \
				--keychain "$HOME/Library/Keychains/login.keychain" "$dmg_path"

codesign --verify --verbose=4 "$dmg_path"

rm -rf "$dmg_source_path"
rm  "$dmg_template_path"

echo ".dmg created"