echo  "Updating remote appcast..."

git checkout gh-pages
git add _include/ appcast.xml
git commit  --amend --no-edit
git push -f origin gh-pages
git checkout master


echo  "Remote appcast updated."

