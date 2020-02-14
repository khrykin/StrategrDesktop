exit_on_error() {
    exit_code=$1
    # shellcheck disable=SC2124
    last_command=${@:2}
    if [ "$exit_code" -ne 0 ]; then
        >&2 echo "\"${last_command}\" command failed with exit code ${exit_code}."
        exit "$exit_code"
    fi
}

echo  "Updating remote appcast..."

git checkout gh-pages
exit_on_error $? !!
git add _include/ appcast.xml
git commit  --amend --no-edit
git push -f origin gh-pages
git checkout master


echo  "Remote appcast updated."

