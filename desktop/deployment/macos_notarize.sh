#!/usr/bin/env bash

set -e

#ASC_PROVIDER="$1"
ASC_USERNAME="$APPLE_ID_EMAIL"
ASC_PASSWORD="$APPLE_NOTARIZATION_PASSWORD"

BUNDLE_ID="$1"

# Path to .app application bundle (or .dmg)
BUNDLE_PKG="$2"

# create temporary files
NOTARIZE_APP_LOG=$(mktemp -t notarize-app)
NOTARIZE_INFO_LOG=$(mktemp -t notarize-info)

status() {
	[ -n "$2" ] && color=$2 || color=4

	bold=$(tput -T ansi bold && tput -T ansi setaf $color)
	normal=$(tput -T ansi sgr0)

	printf "[notarize] %s%s%s\n" "$bold" "$1" "$normal"
}

# delete temporary files on exit
function finish {
	rm "$NOTARIZE_APP_LOG" "$NOTARIZE_INFO_LOG"
}

trap finish EXIT

# submit app for notarization

# we don't use asc provider currently
# --asc-provider "$ASC_PROVIDER"

status "Notarizing $BUNDLE_PKG..."

if xcrun altool --notarize-app --primary-bundle-id "$BUNDLE_ID"  --username "$ASC_USERNAME" --password "$ASC_PASSWORD" -f "$BUNDLE_PKG" > "$NOTARIZE_APP_LOG" 2>&1; then
	cat "$NOTARIZE_APP_LOG"
	RequestUUID=$(awk -F ' = ' '/RequestUUID/ {print $2}' "$NOTARIZE_APP_LOG")

	# check status periodically
	while sleep 30 && date; do

		# check notarization status
		if xcrun altool --notarization-info "$RequestUUID" --username "$ASC_USERNAME" --password "$ASC_PASSWORD" > "$NOTARIZE_INFO_LOG" 2>&1; then
			cat "$NOTARIZE_INFO_LOG"

			# once notarization is complete, run stapler and exit
			if ! grep -q "Status: in progress" "$NOTARIZE_INFO_LOG"; then
				xcrun stapler staple "$BUNDLE_PKG"
				exit $?
			fi
		else
			cat "$NOTARIZE_INFO_LOG" 1>&2
			exit 1
		fi
	done
else
	cat "$NOTARIZE_APP_LOG" 1>&2
	exit 1
fi