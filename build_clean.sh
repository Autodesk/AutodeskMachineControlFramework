#!/bin/bash

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

for dir in "${dirs_to_make[@]}"
do
	if [ ! -d "$dir" ]; then
		echo "Creating directory $dir..."
		mkdir "$dir"
	else
		echo "Directory $dir already exists"
	fi
done

git rev-parse --verify --short HEAD > "$builddir/githash.txt"
GITHASH=$(<"$builddir/githash.txt")
echo "git hash: $GITHASH"

cd "$basepath"

echo "Building Resource builder (Win32)..."
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


#echo "Building Client"
mkdir "$builddir/Client"
mkdir "$builddir/Client/public"
mkdir "$builddir/Client/src"
mkdir "$builddir/Client/src/plugins"
mkdir "$builddir/Client/dist"

copy "$basepath/Client/public/*" "$builddir/Client/public"
copy "$basepath/Client/src/*" "$builddir/Client/src"
copy "$basepath/Client/src/plugins/*" "$builddir/Client/src/plugins"
copy "$basepath/Client/*.js" "$builddir/Client"
copy "$basepath/Client/*.json" "$builddir/Client"

npm install
npm run build

cd ..\Client

cd "$builddir/Client"
go run ../../Server/createDist.go ../Output $GITHASH 

cd "$builddir"

echo "Building Core Modules"
cmake ..
cmake --build . --config Release

echo "Building Core Resources"
go run ../Server/buildResources.go ../Plugins/Resources "$outputdir/${GITHASH}_core.data"

#echo "Building Developer Package"
# TODO: Copy files to builddir

