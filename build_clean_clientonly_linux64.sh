#!/bin/bash
set -e

# disable go modules
export GO111MODULE="off" 

basepath="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
builddir="$basepath/build_client"
outputdir="$builddir/Output"

dirs_to_make[0]="$builddir"
dirs_to_make[1]="$outputdir"
dirs_to_make[2]="$builddir/Client"
dirs_to_make[3]="$builddir/Client/public"
dirs_to_make[4]="$builddir/Client/src"
dirs_to_make[5]="$builddir/Client/src/plugins"
dirs_to_make[6]="$builddir/Client/dist"
dirs_to_make[7]="$builddir/Artifacts"

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

git rev-parse --verify HEAD > "$builddir/longgithash.txt"
LONGGITHASH=$(<"$builddir/longgithash.txt")
echo "long git hash: $LONGGITHASH"

cp "$basepath/Client/public/"*.* "$builddir/Client/public"
cp "$basepath/Client/src/"*.* "$builddir/Client/src"
cp "$basepath/Client/src/plugins/"*.* "$builddir/Client/src/plugins"
cp "$basepath/Client/"*.js "$builddir/Client"
cp "$basepath/Client/"*.json "$builddir/Client"

cd "$builddir/Client"

npm install
npm run build

go run ../../Server/createDist.go ../Output $GITHASH linux64

echo "Build done!"

