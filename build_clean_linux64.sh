#!/bin/bash
set -e

# disable go modules
export GO111MODULE="off" 

basepath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
builddir="$basepath/build"
outputdir="$builddir/Output"

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
dirs_to_make[13]="$builddir/Framework"
dirs_to_make[14]="$builddir/Framework/HeadersDev"
dirs_to_make[15]="$builddir/Framework/HeadersDev/CppDynamic"
dirs_to_make[16]="$builddir/Framework/HeadersDriver"
dirs_to_make[17]="$builddir/Framework/HeadersDriver/CppDynamic"
dirs_to_make[18]="$builddir/Framework/InterfacesDev"
dirs_to_make[19]="$builddir/Framework/PluginCpp"
dirs_to_make[20]="$builddir/Client"
dirs_to_make[21]="$builddir/Client/public"
dirs_to_make[22]="$builddir/Client/src"
dirs_to_make[23]="$builddir/Client/src/plugins"
dirs_to_make[24]="$builddir/Client/dist"


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

cd "$basepath"

echo "Building Resource builder (Win64)..."
set GOARCH=amd64
set GOOS=windows
go build -o "$builddir/DevPackage/Framework/buildresources.exe" -ldflags="-s -w" "$basepath/Server/buildResources.go"

echo "Building Resource builder (Linux64)..."
set GOARCH=amd64
set GOOS=linux
go build -o "$builddir/DevPackage/Framework/buildresources.linux" -ldflags="-s -w" "$basepath/Server/buildResources.go"

echo "Building Resource builder (LinuxARM)..."
set GOARCH=arm
set GOOS=linux
set GOARM=5
go build -o "$builddir/DevPackage/Framework/buildresources.arm" -ldflags="-s -w" "$basepath/Server/buildResources.go"

echo "Building Go Server..."
go get "github.com/gorilla/handlers"
go build -o "$builddir/Output/amc_server" -ldflags="-s -w" "$basepath/Server/mcserver.go"


cp "$basepath/Client/public/"*.* "$builddir/Client/public"
cp "$basepath/Client/src/"*.* "$builddir/Client/src"
cp "$basepath/Client/src/plugins/"*.* "$builddir/Client/src/plugins"
cp "$basepath/Client/"*.js "$builddir/Client"
cp "$basepath/Client/"*.json "$builddir/Client"

cd "$builddir/Client"

npm install
npm run build

go run ../../Server/createDist.go ../Output $GITHASH

cd "$builddir"

echo "Building Core Modules"
cmake ..
cmake --build . --config Release

echo "Building Core Resources"
go run ../Server/buildResources.go ../Plugins/Resources "$outputdir/${GITHASH}_core.data"

echo "Building Developer Package"
cd "$builddir/DevPackage"
cp ../githash.txt Framework/Dist/disthash.txt
cp ../Output/amc_server Framework/Dist/
cp ../Output/amc_server.xml Framework/Dist/
cp ../Output/${GITHASH}_core_libmc.so Framework/Dist/
cp ../Output/${GITHASH}_core_lib3mf.so Framework/Dist/
cp ../Output/${GITHASH}_core_libmcdata.so Framework/Dist/
cp ../Output/${GITHASH}_*.data Framework/Dist/
cp ../Output/${GITHASH}_*.client Framework/Dist/
cp ../Output/${GITHASH}_package.xml Framework/Dist/
cp ../Output/${GITHASH}_driver_*.so Framework/Dist/
cp ../Output/lib3mf.so Framework/Dist/${GITHASH}_core_lib3mf.so
cp ../../Templates/libmcconfig.xml ./configuration.xml
cp ../../Framework/HeadersDev/CppDynamic/*.* Framework/HeadersDev/CppDynamic
cp ../../Framework/InterfacesDev/*.* Framework/InterfacesDev
cp ../../Framework/PluginCpp/*.* Framework/PluginCpp
cp ../../Framework/PluginPython/*.* Framework/PluginPython

go run ../../Server/createDevPackage.go $builddir/DevPackage/Framework $builddir/DevPackage ${GITHASH}

echo "Build done!"