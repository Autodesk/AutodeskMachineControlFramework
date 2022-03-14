@echo off

set basepath=%~dp0
echo %basepath%
cd %basepath%

mkdir ..\build_client\Client
mkdir ..\build_client\Client\public
mkdir ..\build_client\Client\src
mkdir ..\build_client\Client\src\plugins
mkdir ..\build_client\Client\dist

copy ..\Client\public\*.* ..\build_client\Client\public
copy ..\Client\src\*.* ..\build_client\Client\src
copy ..\Client\src\plugins\*.* ..\build_client\Client\src\plugins
copy ..\Client\*.js ..\build_client\Client
copy ..\Client\*.json ..\build_client\Client

cd ..\build_client\Client

call npm install
call npm run build

cd ..\..\
git rev-parse --verify HEAD >"build_client\Client\dist\_githash_client.txt"
git rev-parse --verify HEAD >"Artifacts\clientdist\_githash_client.txt"
cd build_client\Client

go run ..\..\BuildScripts\createClientDist.go dist ..\..\Artifacts\clientdist\clientpackage.zip

if "%1" neq "NOPAUSE" (
	pause
)

exit 0
