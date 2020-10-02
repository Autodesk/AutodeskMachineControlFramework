REM echo off

set basepath=%~dp0
echo %basepath%
set builddir=%basepath%\build
set outputdir=%builddir%\Output

if not exist "%builddir%" (mkdir "%builddir%")
if not exist "%outputdir%" (mkdir "%outputdir%")
if not exist "%builddir%\DevPackage" (mkdir "%builddir%\DevPackage")
if not exist "%builddir%\DevPackage\Plugins" (mkdir "%builddir%\DevPackage\Plugins")
if not exist "%builddir%\DevPackage\Framework" (mkdir "%builddir%\DevPackage\Framework")
if not exist "%builddir%\DevPackage\Framework\HeadersDev" (mkdir "%builddir%\DevPackage\Framework\HeadersDev")
if not exist "%builddir%\DevPackage\Framework\HeadersDev\CppDynamic" (mkdir "%builddir%\DevPackage\Framework\HeadersDev\CppDynamic")
if not exist "%builddir%\DevPackage\Framework\HeadersDriver" (mkdir "%builddir%\DevPackage\Framework\HeadersDriver")
if not exist "%builddir%\DevPackage\Framework\HeadersDriver\CppDynamic" (mkdir "%builddir%\DevPackage\Framework\HeadersDriver\CppDynamic")
if not exist "%builddir%\DevPackage\Framework\InterfacesDev" (mkdir "%builddir%\DevPackage\Framework\InterfacesDev")
if not exist "%builddir%\DevPackage\Framework\PluginCpp" (mkdir "%builddir%\DevPackage\Framework\PluginCpp")
if not exist "%builddir%\DevPackage\Framework\PluginPython" (mkdir "%builddir%\DevPackage\Framework\PluginPython")
if not exist "%builddir%\DevPackage\Framework\Dist" (mkdir "%builddir%\DevPackage\Framework\Dist")

git rev-parse --verify --short HEAD >"%builddir%\githash.txt"
SET /p GITHASH=<"%builddir%\githash.txt"
echo git hash: %GITHASH%

cd /d "%basepath%"

echo "Building Resource builder..."
go build -o "%builddir%/DevPackage/Framework/BuildResources.exe" "%basepath%/Server/buildresources.go"


echo "Building Server..."
go get github.com/gorilla/handlers
go build -o "%builddir%/Output/amc_server.exe" -ldflags="-s -w" "%basepath%/Server/mcserver.go"

echo "Building Client"
cd "%basepath%\Client"
call build_client_clean.bat
cd "%builddir%\Client"
go run ../../Server/createDist.go ../Output %GITHASH%

cd "%builddir%"

echo "Building Core Modules"
call cmake ..
call cmake --build . --config Release

echo "Building Core Resources"
go run ../Server/buildresources.go ../Plugins/Resources "%outputdir%/%GITHASH%_core_resources.zip"

echo "Building Developer Package"
cd "%builddir%\DevPackage"
copy ..\githash.txt Framework\Dist\disthash.txt
copy ..\Output\amc_server.exe Framework\Dist\
copy ..\Output\amc_server.xml Framework\Dist\
copy ..\Output\%GITHASH%_core_libmc.dll Framework\Dist\
copy ..\Output\%GITHASH%_core_lib3mf.dll Framework\Dist\
copy ..\Output\%GITHASH%_core_libmcdata.dll Framework\Dist\
copy ..\Output\%GITHASH%_core_client.zip Framework\Dist\
copy ..\Output\%GITHASH%_package.xml Framework\Dist\
copy ..\Output\%GITHASH%_driver_*.dll Framework\Dist\
copy ..\Output\lib3mf.dll Framework\Dist\%GITHASH%_core_lib3mf.dll
copy ..\..\Templates\CMakeLists_DeveloperPackage.txt CMakeLists.txt
copy ..\..\Templates\libmcconfig.xml .\configuration.xml
copy ..\..\Templates\build_devpackage_clean.bat .\build_devpackage_clean.bat
copy ..\..\Framework\HeadersDev\CppDynamic\*.* Framework\HeadersDev\CppDynamic
copy ..\..\Framework\InterfacesDev\*.* Framework\InterfacesDev
copy ..\..\Framework\PluginCpp\*.* Framework\PluginCpp
copy ..\..\Framework\PluginPython\*.* Framework\PluginPython

echo "Build done!"

if "%1" neq "NOPAUSE" (
	pause
)
