#!/bin/bash

set -e

status() {
	[ -n "$2" ] && color=$2 || color=4

	bold=$(tput -T ansi bold && tput -T ansi setaf $color)
	normal=$(tput -T ansi sgr0)

	printf "[deployment] %s%s%s\n" "$bold" "$1" "$normal"
}

Qt5_ROOT="$1"
APP_BUNDLE_PATH="$2"

echo "Specified Qt5 installation: $Qt5_ROOT"
echo "App bundle: $APP_BUNDLE_PATH"

codesign --deep \
         --force \
         --verify \
         --timestamp \
         --verbose -s "$CODE_SIGN_IDENTITY" \
         --options runtime \
				 --keychain "$CODE_SIGN_KEYCHAIN" \
				 "$APP_BUNDLE_PATH/Contents/Frameworks/Sparkle.framework/Versions/A/Resources/Autoupdate.app"
