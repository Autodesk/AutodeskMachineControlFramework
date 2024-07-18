#!/bin/bash
set -e


basepath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "$basepath"
mkdir ../_build_lib3mf_rpi
cd ../_build_lib3mf_rpi

[ -e ../Artifacts/lib3mf/lib3mf_rpi.so ] && rm ../Artifacts/lib3mf/lib3mf_rpi.so
[ -e ../Artifacts/lib3mf/_githash_rpi_lib3mf.txt ] && rm ../Artifacts/lib3mf/_githash_rpi_lib3mf.txt

cmake ../submodules/lib3mf -DLIB3MF_TESTS=OFF 
cmake --build . --config Release
cp -L lib3mf.so ../Artifacts/lib3mf/lib3mf_rpi.so

cd ../submodules/lib3mf
git rev-parse --verify HEAD >"../Artifacts/lib3mf/_githash_rpi_lib3mf.txt"

cd ../..
