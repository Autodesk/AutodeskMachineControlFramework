#!/bin/bash

basepath=$(cd "$(dirname "$0")" && pwd)
echo "$basepath"
cd "$basepath"

if [ -d "../build_client" ]; then
	echo "Error: build_client directory already exists. Please remove it before running this script."
	exit 1
fi

mkdir -p ../build_client/core/common
mkdir -p ../build_client/core/modules
mkdir -p ../build_client/Client/public
mkdir -p ../build_client/Client/src/common
mkdir -p ../build_client/Client/src/modules
mkdir -p ../build_client/Client/src/dialogs
mkdir -p ../build_client/Client/dist

# Shared core JS (framework-agnostic)
cp ../Client/core/common/* ../build_client/core/common/ 2>/dev/null || true
cp ../Client/core/modules/* ../build_client/core/modules/ 2>/dev/null || true

# Vue 2 specific files
cp ../Client/vue2/public/* ../build_client/Client/public/ 2>/dev/null || true
cp ../Client/vue2/src/*.* ../build_client/Client/src/ 2>/dev/null || true
cp ../Client/vue2/src/modules/* ../build_client/Client/src/modules/ 2>/dev/null || true
cp ../Client/vue2/src/dialogs/* ../build_client/Client/src/dialogs/ 2>/dev/null || true
cp ../Client/vue2/vue.config.js ../build_client/Client/ 2>/dev/null || true
cp ../Client/vue2/babel.config.js ../build_client/Client/ 2>/dev/null || true
cp ../Client/vue2/package.json ../build_client/Client/ 2>/dev/null || true

cd ..
git log -n 1 --format="%H" -- "Client" > "build_client/Client/dist/_githash_client_vue2.txt"
git log -n 1 --format="%H" -- "Client" > "Artifacts/clientdist/_githash_client_vue2.txt"
CLIENTDIRHASH=$(cat "build_client/Client/dist/_githash_client_vue2.txt")

cat > build_client/core/common/AMCGitHash.js <<EOF
export function getClientGitHash ()
{
  return "$CLIENTDIRHASH";
}
EOF




cd build_client/Client

TOOLBUILDDIR="../../build_client_dist"
if [ ! -f "$TOOLBUILDDIR/CMakeCache.txt" ]; then
	echo "Configuring client build tools..."
	mkdir -p "$TOOLBUILDDIR"
	cmake -S ../../BuildScripts/ClientDist -B "$TOOLBUILDDIR"
fi
echo "Building client build tools..."
cmake --build "$TOOLBUILDDIR" --config Release


npm install

# Work around node-ipc crashing when os.networkInterfaces fails in sandboxed environments.
NODE_IPC_DEFAULTS="node_modules/@achrinza/node-ipc/entities/Defaults.js"
if [ -f "$NODE_IPC_DEFAULTS" ]; then
  python3 - "$NODE_IPC_DEFAULTS" <<'PY'
import sys

path = sys.argv[1]
with open(path, "r", encoding="utf-8") as handle:
    data = handle.read()
original_data = data

needle = "const networkInterfaces = os.networkInterfaces();"
broken = (
    "let networkInterfaces = null;\\\\n"
    "    try {\\\\n"
    "        networkInterfaces = os.networkInterfaces();\\\\n"
    "    } catch (error) {\\\\n"
    "        networkInterfaces = null;\\\\n"
    "    }"
)
replacement = (
    "let networkInterfaces = null;\n"
    "    try {\n"
    "        networkInterfaces = os.networkInterfaces();\n"
    "    } catch (error) {\n"
    "        networkInterfaces = null;\n"
    "    }"
)
if needle in data:
    data = data.replace(needle, replacement, 1)
elif broken in data:
    data = data.replace(broken, replacement, 1)

if data != original_data:
    with open(path, "w", encoding="utf-8") as handle:
        handle.write(data)
PY
fi

npm run build

cd ../..

cd build_client/Client

"$TOOLBUILDDIR/create_client_dist" dist ../../Artifacts/clientdist/clientpackage_vue2.zip

"$TOOLBUILDDIR/create_client_source" . ../../Artifacts/clientdist/clientsourcepackage_vue2.zip

echo
echo "Created packages in Artifacts/clientdist/:"
ls -l ../../Artifacts/clientdist
echo

exit 0
