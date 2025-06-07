#!/usr/bin/env bash

set -e

APP_BUNDLE_PATH="$1"
EXECUTABLE_PATH="$APP_BUNDLE_PATH/Contents/MacOS/Strategr"

echo "App bundle: $APP_BUNDLE_PATH"

codesign --deep --force --sign - "$APP_BUNDLE_PATH"
