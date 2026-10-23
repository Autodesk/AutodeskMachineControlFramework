#!/bin/bash
set -e

basepath=$(cd "$(dirname "$0")" && pwd)
echo "$basepath"
cd "$basepath"

mkdir -p ../build_apidocs

cp ../OpenAPI/swagger*.* ../build_apidocs 2>/dev/null || true
cp ../OpenAPI/index.css ../build_apidocs 2>/dev/null || true
cp ../OpenAPI/index.html ../build_apidocs 2>/dev/null || true
cp ../OpenAPI/favicon.png ../build_apidocs 2>/dev/null || true
cp ../OpenAPI/oauth2-redirect.html ../build_apidocs 2>/dev/null || true
cp ../OpenAPI/AMCF_OpenAPI.json ../build_apidocs 2>/dev/null || true

cd ..

git log -n 1 --format="%H" -- "OpenAPI" > "build_apidocs/_githash_apidocs.txt"
git log -n 1 --format="%H" -- "OpenAPI" > "Artifacts/apidocsdist/_githash_apidocs.txt"

TOOLBUILDDIR="build_clientdist_tools"
mkdir -p "$TOOLBUILDDIR"
git rev-parse --verify --short HEAD > "$TOOLBUILDDIR/githash.txt"
git log -n 1 --format="%H" -- "OpenAPI" > "$TOOLBUILDDIR/clientdirhash.txt"
cmake -S . -B "$TOOLBUILDDIR"
cmake --build "$TOOLBUILDDIR" --target create_client_dist --config Release
"$TOOLBUILDDIR/DevPackage/Framework/create_client_dist" build_apidocs Artifacts/apidocsdist/apidocspackage.zip

echo
echo "Created packages in Artifacts/apidocsdist/:"
ls -l Artifacts/apidocsdist
echo
