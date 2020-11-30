#!/usr/bin/env bash

set -e

status() {
	[ -n "$2" ] && color=$2 || color=4

	bold=$(tput -T ansi bold && tput -T ansi setaf $color)
	normal=$(tput -T ansi sgr0)

	printf "[deployment] %s%s%s\n" "$bold" "$1" "$normal"
}

fix_rpath() {
    # Find rpaths via otool
    otool_output=$(otool -l "$1" | grep RPATH -A2)

    fixed=false

    # Split otool output by lines
    while IFS= read -r line
    do
        regex="path (.+) \(offset.+$"

        if [[ $line =~ $regex ]]
        then
            rpath="${BASH_REMATCH[1]}"

            echo "Found rpath: \"$rpath\""

            if [ "$rpath" == "@executable_path/../Frameworks" ] || [ "$rpath" == "@loader_path/../Frameworks" ]
            then
                fixed=true
            else
              echo "Removing rpath: \"$rpath\""
    	        install_name_tool -delete_rpath "$rpath" "$1"
            fi
        fi
    done <<< "$otool_output"

    if [ $fixed != true ]
    then
        install_name_tool -add_rpath @executable_path/../Frameworks "$1"
    fi
}

APP_BUNDLE_PATH="$1"
EXECUTABLE_PATH="$APP_BUNDLE_PATH/Contents/MacOS/Strategr"

echo "App bundle: $APP_BUNDLE_PATH"

fix_rpath "$EXECUTABLE_PATH"

codesign --deep \
         --force \
         --verify \
         --timestamp \
         --verbose -s "$CODE_SIGN_IDENTITY" \
         --options runtime \
				 --keychain "$CODE_SIGN_KEYCHAIN" \
				 "$APP_BUNDLE_PATH/Contents/Frameworks/Sparkle.framework/Versions/A/Resources/Autoupdate.app"