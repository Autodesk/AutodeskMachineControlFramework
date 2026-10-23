#!/bin/bash
set -e

basepath=$(cd "$(dirname "$0")" && pwd)
echo "$basepath"
cd "$basepath"

if [ -d "../build_client_svelte" ]; then
	echo "Error: build_client_svelte directory already exists. Please remove it before running this script."
	exit 1
fi

mkdir -p ../build_client_svelte
mkdir -p clientdist

# Copy the entire Svelte project (source only, no node_modules or dist)
rsync -a --exclude='node_modules' --exclude='dist' --exclude='.svelte-kit' ../Client/svelte/ ../build_client_svelte/Client/

# Shared core JS (common, modules, theme)
cp -r ../Client/core ../build_client_svelte/core

cd ..
git log -n 1 --format="%H" -- "Client/core" "Client/svelte" > "build_client_svelte/Client/_githash_client_svelte.txt"
git log -n 1 --format="%H" -- "Client/core" "Client/svelte" > "Artifacts/clientdist/_githash_client_svelte.txt"
CLIENTDIRHASH=$(cat "build_client_svelte/Client/_githash_client_svelte.txt")

cat > build_client_svelte/core/common/AMCGitHash.js <<HASHEOF
export function getClientGitHash ()
{
  return "${CLIENTDIRHASH}";
}
HASHEOF

cd build_client_svelte/Client

npm install
npm run build

cd ../..

# Build create_client_dist in its own lightweight build directory
CLIENTDIST_BUILDDIR="build_client_dist"
if [ ! -f "$CLIENTDIST_BUILDDIR/CMakeCache.txt" ]; then
	echo "Configuring create_client_dist tool..."
	mkdir -p "$CLIENTDIST_BUILDDIR"
	cmake -S BuildScripts/ClientDist -B "$CLIENTDIST_BUILDDIR"
fi
echo "Building create_client_dist tool..."
cmake --build "$CLIENTDIST_BUILDDIR" --config Release

CLIENTDIST_EXE="$CLIENTDIST_BUILDDIR/create_client_dist"

# Package the build output into a client ZIP
"$CLIENTDIST_EXE" build_client_svelte/Client/dist Artifacts/clientdist/clientpackage_svelte.zip

echo
echo "Created svelte package in Artifacts/clientdist/:"
ls -l Artifacts/clientdist/clientpackage_svelte.zip
echo

exit 0
