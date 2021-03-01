REM echo off

set basepath=%~dp0
echo %basepath%
set builddir=%basepath%\build
set outputdir=%builddir%\Output

if not exist "%builddir%" (mkdir "%builddir%")
if not exist "%outputdir%" (mkdir "%outputdir%")
if not exist "%builddir%\DevPackage" (mkdir "%builddir%\DevPackage")
if not exist "%builddir%\DevPackage\Framework" (mkdir "%builddir%\DevPackage\Framework")
if not exist "%builddir%\DevPackage\Framework\HeadersDev" (mkdir "%builddir%\DevPackage\Framework\HeadersDev")
if not exist "%builddir%\DevPackage\Framework\HeadersDev\CppDynamic" (mkdir "%builddir%\DevPackage\Framework\HeadersDev\CppDynamic")
if not exist "%builddir%\DevPackage\Framework\HeadersDriver" (mkdir "%builddir%\DevPackage\Framework\HeadersDriver")
if not exist "%builddir%\DevPackage\Framework\HeadersDriver\CppDynamic" (mkdir "%builddir%\DevPackage\Framework\HeadersDriver\CppDynamic")
if not exist "%builddir%\DevPackage\Framework\InterfacesDev" (mkdir "%builddir%\DevPackage\Framework\InterfacesDev")
if not exist "%builddir%\DevPackage\Framework\PluginCpp" (mkdir "%builddir%\DevPackage\Framework\PluginCpp")
if not exist "%builddir%\DevPackage\Framework\PluginPython" (mkdir "%builddir%\DevPackage\Framework\PluginPython")
if not exist "%builddir%\DevPackage\Framework\Dist" (mkdir "%builddir%\DevPackage\Framework\Dist")
if not exist "%builddir%\Framework" (mkdir "%builddir%\Framework")
if not exist "%builddir%\Framework\HeadersDev" (mkdir "%builddir%\Framework\HeadersDev")
if not exist "%builddir%\Framework\HeadersDev\CppDynamic" (mkdir "%builddir%\Framework\HeadersDev\CppDynamic")
if not exist "%builddir%\Framework\HeadersDriver" (mkdir "%builddir%\Framework\HeadersDriver")
if not exist "%builddir%\Framework\HeadersDriver\CppDynamic" (mkdir "%builddir%\Framework\HeadersDriver\CppDynamic")
if not exist "%builddir%\Framework\InterfacesDev" (mkdir "%builddir%\Framework\InterfacesDev")
if not exist "%builddir%\Framework\PluginCpp" (mkdir "%builddir%\Framework\PluginCpp")

copy "%basepath%\Framework\PluginCpp\*.*" "%builddir%\Framework\PluginCpp"
copy "%basepath%\Framework\InterfacesDev\*.*" "%builddir%\Framework\InterfacesDev"

git rev-parse --verify --short HEAD >"%builddir%\githash.txt"
SET /p GITHASH=<"%builddir%\githash.txt"
echo git hash: %GITHASH%

cd /d "%basepath%"

echo "Building Resource builder (Win32)..."
set GOARCH=amd64
set GOOS=windows
go build -o "%builddir%/DevPackage/Framework/buildresources.exe" -ldflags="-s -w" "%basepath%/Server/buildresources.go"

echo "Building Resource builder (Linux64)..."
set GOARCH=amd64
set GOOS=linux
go build -o "%builddir%/DevPackage/Framework/buildresources.linux" -ldflags="-s -w" "%basepath%/Server/buildresources.go"

echo "Building Resource builder (LinuxARM)..."
set GOARCH=arm
set GOOS=linux
set GOARM=5
go build -o "%builddir%/DevPackage/Framework/buildresources.arm" -ldflags="-s -w" "%basepath%/Server/buildresources.go"

echo "Building Server..."
set GOARCH=amd64
set GOOS=windows
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
REM call cmake -G "MinGW Makefiles" ..
call cmake --build . --config Release


echo "Building Developer Package"
cd "%builddir%\DevPackage"
copy ..\githash.txt Framework\Dist\disthash.txt
copy ..\Output\amc_server.exe Framework\Dist\
copy ..\Output\amc_server.xml Framework\Dist\
copy ..\Output\%GITHASH%_core_libmc.dll Framework\Dist\
copy ..\Output\%GITHASH%_core_lib3mf.dll Framework\Dist\
copy ..\Output\%GITHASH%_core_libmcdata.dll Framework\Dist\
copy ..\Output\%GITHASH%_*.data Framework\Dist\
copy ..\Output\%GITHASH%_*.client Framework\Dist\
copy ..\Output\%GITHASH%_package.xml Framework\Dist\
copy ..\Output\%GITHASH%_driver_*.dll Framework\Dist\
copy ..\Output\lib3mf.dll Framework\Dist\%GITHASH%_core_lib3mf.dll
copy ..\..\Templates\libmcconfig.xml .\configuration.xml
copy ..\..\Framework\HeadersDev\CppDynamic\*.* Framework\HeadersDev\CppDynamic
copy ..\..\Framework\InterfacesDev\*.* Framework\InterfacesDev
copy ..\..\Framework\PluginCpp\*.* Framework\PluginCpp
copy ..\..\Framework\PluginPython\*.* Framework\PluginPython

go run ../../Server/createDevPackage.go %builddir%\DevPackage\Framework %builddir%\DevPackage\ %GITHASH%

echo "Build done!"

if "%1" neq "NOPAUSE" (
	pause
)

exit 0
