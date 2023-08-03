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
DEVPACKAGESHA=$(<"./devpackage_sha256_rpi.txt")

echo "Verifying package ${DEVPACKAGE}"
DevTools/packageManager.arm download "DevPackages/amcf_rpi_${DEVPACKAGE}.zip" "http://cicd.aleos.io:8865/" "rpi_${DEVPACKAGE}" "${DEVPACKAGESHA}"

echo "Using developer package rpi_${DEVPACKAGE}"

mkdir -p build_rpi
mkdir -p build_rpi/Framework
mkdir -p build_rpi/Output
mkdir -p build_rpi/Output/data

echo "Extracting DevPackages/amcf_rpi_${DEVPACKAGE}.zip"

cd build_rpi/Framework
unzip -o ../../DevPackages/amcf_rpi_${DEVPACKAGE}.zip
chmod +x Dist/*.so
chmod +x Dist/amc_server
chmod +x buildresources.arm
cd ../..

cp build_rpi/Framework/Dist/disthash.txt build_rpi/githash.txt 

cd build_rpi
cmake  ..
cmake --build . --config Release

cd Output

