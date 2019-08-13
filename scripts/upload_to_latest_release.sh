#!/usr/bin/env bash

set -e
[ -z "$DEBUG" ] || set -x;

remote_origin_url=$(git config --get remote.origin.url)
repo_name="${remote_origin_url##*.com/}"
repo_name="${repo_name%.git}"

REPO="$repo_name"

ASSETS="$@"

response=$(
  curl --fail \
       --netrc \
       --silent \
       --location \
       "https://api.github.com/repos/${REPO}/releases"
)

upload_url="$(echo "$response" | jq .[0] | jq -r .upload_url | sed -e "s/{?name,label}//")"

echo "Latest Release Upload URL: $upload_url"

for file in $ASSETS; do
  filename=$(basename "$file")
  extension="${filename##*.}"
  curl --netrc \
       --header "Content-Type:application/$extension" \
       --data-binary "@$file" \
       "$upload_url?name=$filename"
done
