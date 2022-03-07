#!/bin/bash
set -e


basepath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "$basepath"
mkdir ../_build_lib3mf_linux64
cd ../_build_lib3mf_linux64

[ -e ../Artifacts/lib3mf/lib3mf_linux64.so ] && rm ../Artifacts/lib3mf/lib3mf_linux64.so
[ -e ../Artifacts/lib3mf/_githash_linux64_lib3mf.txt ] && rm ../Artifacts/lib3mf/_githash_linux64_lib3mf.txt

cmake ../submodules/lib3mf
cmake --build . --config Release
cp -L lib3mf.so ../Artifacts/lib3mf/lib3mf_linux64.so

git rev-parse --verify HEAD >"../Artifacts/lib3mf/_githash_linux64_lib3mf.txt"

cd ..
