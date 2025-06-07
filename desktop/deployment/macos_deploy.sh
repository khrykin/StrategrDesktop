#!/usr/bin/env bash

set -e

APP_BUNDLE_PATH="$1"
EXECUTABLE_PATH="$APP_BUNDLE_PATH/Contents/MacOS/Strategr"

echo "App bundle: $APP_BUNDLE_PATH"

# Somehow @rpath/Sparkle is set on the executable which is incorrecc. Fix it
# manually:
install_name_tool -change @rpath/Sparkle @rpath/Sparkle.framework/Sparkle "$EXECUTABLE_PATH"

codesign --deep --force --sign - "$APP_BUNDLE_PATH"
