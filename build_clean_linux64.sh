#!/bin/bash
set -e

# disable go modules
export GO111MODULE="off" 

basepath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

PLATFORMNAME="linux64"

for var in "$@"
do
echo "command line parameter: $var"
if test $var = "--buildrpi"
then
	PLATFORMNAME="rpi"	
fi	
if test $var = "--buildwin64"
then
	PLATFORMNAME="win64"	
fi	
done

builddir="$basepath/build_${PLATFORMNAME}"
outputdir="$builddir/Output"


echo "Building for platform: ${PLATFORMNAME}..."

dirs_to_make[0]="$builddir"
dirs_to_make[1]="$outputdir"
dirs_to_make[2]="$builddir/DevPackage"
dirs_to_make[3]="$builddir/DevPackage/Plugins"
dirs_to_make[4]="$builddir/DevPackage/Framework"
dirs_to_make[5]="$builddir/DevPackage/Framework/HeadersDev"
dirs_to_make[6]="$builddir/DevPackage/Framework/HeadersDev/CppDynamic"
dirs_to_make[7]="$builddir/DevPackage/Framework/HeadersDriver"
dirs_to_make[8]="$builddir/DevPackage/Framework/HeadersDriver/CppDynamic"
dirs_to_make[9]="$builddir/DevPackage/Framework/InterfacesDev"
dirs_to_make[10]="$builddir/DevPackage/Framework/PluginCpp"
dirs_to_make[11]="$builddir/DevPackage/Framework/PluginPython"
dirs_to_make[12]="$builddir/DevPackage/Framework/Dist"
dirs_to_make[13]="$builddir/DevPackage/Framework/ClientSource"
dirs_to_make[14]="$builddir/Framework"
dirs_to_make[15]="$builddir/Framework/HeadersDev"
dirs_to_make[16]="$builddir/Framework/HeadersDev/CppDynamic"
dirs_to_make[17]="$builddir/Framework/HeadersDriver"
dirs_to_make[18]="$builddir/Framework/HeadersDriver/CppDynamic"
dirs_to_make[19]="$builddir/Framework/InterfacesDev"
dirs_to_make[20]="$builddir/Framework/PluginCpp"
dirs_to_make[21]="$builddir/Framework/ClientSource"
dirs_to_make[22]="$builddir/Client"
dirs_to_make[23]="$builddir/Client/public"
dirs_to_make[24]="$builddir/Client/src"
dirs_to_make[25]="$builddir/Client/src/plugins"
dirs_to_make[26]="$builddir/Client/dist"
dirs_to_make[27]="$builddir/Artifacts"
dirs_to_make[28]="$outputdir/data"

for dir in "${dirs_to_make[@]}"
do
	if [ ! -d "$dir" ]; then
		echo "Creating directory $dir..."
		mkdir "$dir"
	else
		echo "Directory $dir already exists"
	fi
done

cp "$basepath/Framework/PluginCpp/"* "$builddir/Framework/PluginCpp"
cp "$basepath/Framework/InterfacesDev/"* "$builddir/Framework/InterfacesDev"

git rev-parse --verify --short HEAD > "$builddir/githash.txt"
GITHASH=$(<"$builddir/githash.txt")
echo "git hash: $GITHASH"

git rev-parse --verify HEAD > "$builddir/longgithash.txt"
LONGGITHASH=$(<"$builddir/longgithash.txt")
echo "long git hash: $LONGGITHASH"

git log -n 1 --format="%H" -- "$basepath/Client" > "$builddir/clientdirhash.txt"
CLIENTDIRHASH=$(<"$builddir/clientdirhash.txt")
echo "client dir hash: $CLIENTDIRHASH"

CLIENTDISTHASH=$(<"$basepath/Artifacts/clientdist/_githash_client.txt")
echo "client dist hash: $CLIENTDISTHASH"

if test $CLIENTDISTHASH != $CLIENTDIRHASH
then
	echo "Invalid client hash! Please rebuild client!"
	exit 1
fi

cd "$basepath"

if test $PLATFORMNAME = "rpi"
then

	echo "Building Resource builder (LinuxARM)..."
	export GOARCH=arm
	export GOOS=linux
	export GOARM=5
	go build -o "$builddir/DevPackage/Framework/buildresources.arm" -ldflags="-s -w" "$basepath/BuildScripts/buildResources.go"

else

if test $PLATFORMNAME = "win64"
then

	echo "Building Resource builder (Win64)..."
	export GOARCH=amd64
	export GOOS=windows
	go build -o "$builddir/DevPackage/Framework/buildresources.exe" -ldflags="-s -w" "$basepath/BuildScripts/buildResources.go"

	echo "Building Resource builder (Linux64)..."
	export GOARCH=amd64
	export GOOS=linux
	go build -o "$builddir/DevPackage/Framework/buildresources.linux64" -ldflags="-s -w" "$basepath/BuildScripts/buildResources.go"
	
else

	echo "Building Resource builder (Linux64)..."
	export GOARCH=amd64
	export GOOS=linux
	go build -o "$builddir/DevPackage/Framework/buildresources.linux64" -ldflags="-s -w" "$basepath/BuildScripts/buildResources.go"

fi	
fi

cp "$basepath/Artifacts/clientdist/clientpackage.zip" "$builddir/Output/${GITHASH}_core.client"

cd "$builddir"


echo "Building Core Modules"
if test $PLATFORMNAME = "win64"
then
cmake -DOVERRIDE_BUILDRESOURCES=linux64 -DCMAKE_TOOLCHAIN_FILE=$basepath/BuildScripts/CrossCompile_Win32FromDebian.txt ..
else
cmake ..
fi

cmake --build . --config Release

echo "Building Core Resources"
go run ../BuildScripts/buildResources.go ../Plugins/Resources "$outputdir/${GITHASH}_core.data"

echo "Building Developer Package"
cd "$builddir/DevPackage"
cp ../githash.txt Framework/Dist/disthash.txt

if test $PLATFORMNAME = "win64"
then
cp ../Output/amc_server.exe Framework/Dist/
DLLEXT=dll
else
cp ../Output/amc_server Framework/Dist/
DLLEXT=so
fi

cp ../Output/amc_server.xml Framework/Dist/
cp ../Output/${GITHASH}_core_libmc.${DLLEXT} Framework/Dist/
cp ../Output/${GITHASH}_core_lib3mf.${DLLEXT} Framework/Dist/
cp ../Output/${GITHASH}_core_libmcdata.${DLLEXT} Framework/Dist/
cp ../Output/${GITHASH}_*.data Framework/Dist/
cp ../Output/${GITHASH}_core.client Framework/Dist/
cp ../Output/${GITHASH}_package.xml Framework/Dist/
cp ../Output/${GITHASH}_driver_*.${DLLEXT} Framework/Dist/
cp ../../Framework/HeadersDev/CppDynamic/*.* Framework/HeadersDev/CppDynamic
cp ../../Framework/InterfacesDev/*.* Framework/InterfacesDev
cp ../../Framework/PluginCpp/*.* Framework/PluginCpp
rm Framework/Dist/${GITHASH}_core.data

cd $builddir
echo "Building Package XML"

"$builddir/DevPackage/Framework/create_package_xml" --config "$builddir/Output/${GITHASH}_config.xml" --devpackage ${GITHASH} --output "$builddir/Output/${GITHASH}_package.xml" --serveroutput "$builddir/Output/amc_server.xml"

go run "$basepath/BuildScripts/createDevPackage.go" ./DevPackage/Framework ./DevPackage ${LONGGITHASH} $PLATFORMNAME

cp "$builddir/DevPackage/amcf_${PLATFORMNAME}_${LONGGITHASH}.zip" "$builddir/Artifacts/devpackage_${PLATFORMNAME}.zip"

if [[ -z "${AMCF_PACKAGE_REPOSITORY}" ]]; then
  echo "No AMCF package repository given!"

else
  echo "AMCF package repository: ${AMCF_PACKAGE_REPOSITORY}"
  if [[ -z "${AMCF_PACKAGE_REPOSITORY_KEYFILE}" ]]; then
    echo "No AMCF package repository key file given!"

  else
    echo "AMCF package key file: ${AMCF_PACKAGE_REPOSITORY_KEYFILE}"
    echo "Uploading package to server..."
    scp -i "${AMCF_PACKAGE_REPOSITORY_KEYFILE}" "$builddir/DevPackage/amcf_${PLATFORMNAME}_${LONGGITHASH}.zip" "${AMCF_PACKAGE_REPOSITORY}"
    echo "Upload finished."
  fi
fi

echo "Build done!"
