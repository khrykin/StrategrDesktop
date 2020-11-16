#!/bin/bash

set -e

APP_BUNDLE="$1"
OUTPUT_DIR="$2"

mkdir -p "$OUTPUT_DIR"

create-dmg "$APP_BUNDLE" \
            --identity="$CODE_SIGN_IDENTITY" \
            --overwrite \
            "$OUTPUT_DIR"
