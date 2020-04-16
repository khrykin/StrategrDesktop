#!/bin/sh -u

# (c) @rednoah
# https://github.com/rednoah/notarize-app/

#ASC_PROVIDER="$1"
ASC_USERNAME="$APPLE_ID_EMAIL"
ASC_PASSWORD="$APPLE_NOTARIZATION_PASSWORD"

BUNDLE_ID="com.khrykin.strategr.macos"
BUNDLE_PKG="$1"

# create temporary files
NOTARIZE_APP_LOG=$(mktemp -t notarize-app)
NOTARIZE_INFO_LOG=$(mktemp -t notarize-info)

# delete temporary files on exit
function finish {
	rm "$NOTARIZE_APP_LOG" "$NOTARIZE_INFO_LOG"
}
trap finish EXIT


# submit app for notarization

# we don't use asc provider currently
# --asc-provider "$ASC_PROVIDER"

if xcrun altool --notarize-app --primary-bundle-id "$BUNDLE_ID"  --username "$ASC_USERNAME" --password "$ASC_PASSWORD" -f "$BUNDLE_PKG" > "$NOTARIZE_APP_LOG" 2>&1; then
	cat "$NOTARIZE_APP_LOG"
	RequestUUID=$(awk -F ' = ' '/RequestUUID/ {print $2}' "$NOTARIZE_APP_LOG")

	# check status periodically
	while sleep 60 && date; do
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