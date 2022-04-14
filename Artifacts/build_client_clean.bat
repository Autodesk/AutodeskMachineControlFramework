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
git log -n 1 --format="%%H" -- "Client" >"build_client\Client\dist\_githash_client.txt"
git log -n 1 --format="%%H" -- "Client" >"Artifacts\clientdist\_githash_client.txt"
SET /p CLIENTDIRHASH=<"build_client\Client\dist\_githash_client.txt"

echo export function getClientGitHash ()> build_client\Client\src\AMCGitHash.js
echo {>> build_client\Client\src\AMCGitHash.js
echo   return "%CLIENTDIRHASH%";>> build_client\Client\src\AMCGitHash.js
echo }>> build_client\Client\src\AMCGitHash.js


cd build_client\Client

go run ..\..\BuildScripts\createClientDist.go dist ..\..\Artifacts\clientdist\clientpackage.zip

if "%1" neq "NOPAUSE" (
	pause
)

exit 0
