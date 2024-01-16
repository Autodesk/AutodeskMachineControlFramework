#!/bin/bash
set -e

# disable go modules
export GO111MODULE="off" 

scriptpath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "script path: $scriptpath"
cd "$scriptpath"
cd ..

mkdir -p DevPackages

DEVPACKAGE=$(<"./devpackage_version.txt")
DEVPACKAGESHA=$(<"./devpackage_sha256_linux64.txt")

echo "Verifying package ${DEVPACKAGE}"
DevTools/packageManager.arm download "DevPackages/amcf_linux64_${DEVPACKAGE}.zip" "https://ssl.aleos.io/" "linux64_${DEVPACKAGE}" "${DEVPACKAGESHA}"

echo "Using developer package linux64_${DEVPACKAGE}"

mkdir -p build_linux64
mkdir -p build_linux64/Framework
mkdir -p build_linux64/Output
mkdir -p build_linux64/Output/data

echo "Extracting DevPackages/amcf_linux64_${DEVPACKAGE}.zip"

cd build_linux64/Framework
unzip -o ../../DevPackages/amcf_linux64_${DEVPACKAGE}.zip
chmod +x Dist/*.so
chmod +x Dist/amc_server
chmod +x buildresources.arm
cd ../..

cp build_linux64/Framework/Dist/disthash.txt build_linux64/githash.txt 

cd build_linux64
cmake  ..
cmake --build . --config Release

cd Output

