@echo off

set basepath=%~dp0
echo %basepath%
cd %basepath%

mkdir ..\build_apidocs

copy ..\OpenAPI\swagger*.* ..\build_apidocs
copy ..\OpenAPI\index.css ..\build_apidocs
copy ..\OpenAPI\index.html ..\build_apidocs
copy ..\OpenAPI\favicon.png ..\build_apidocs
copy ..\OpenAPI\oauth2-redirect.html ..\build_apidocs
copy ..\OpenAPI\AMCF_OpenAPI.json ..\build_apidocs

cd ..

git log -n 1 --format="%%H" -- "OpenAPI" >"build_apidocs\_githash_apidocs.txt"
git log -n 1 --format="%%H" -- "OpenAPI" >"Artifacts\apidocsdist\_githash_apidocs.txt"

set TOOLBUILDDIR=build_clientdist_tools
if not exist "%TOOLBUILDDIR%" (mkdir "%TOOLBUILDDIR%")
git rev-parse --verify --short HEAD >"%TOOLBUILDDIR%\githash.txt"
git log -n 1 --format="%%H" -- "OpenAPI" >"%TOOLBUILDDIR%\clientdirhash.txt"
cmake -S . -B "%TOOLBUILDDIR%"
cmake --build "%TOOLBUILDDIR%" --target create_client_dist --config Release
"%TOOLBUILDDIR%\DevPackage\Framework\create_client_dist.exe" build_apidocs Artifacts\apidocsdist\apidocspackage.zip 

if "%1" neq "NOPAUSE" (
	pause
)

exit 0
