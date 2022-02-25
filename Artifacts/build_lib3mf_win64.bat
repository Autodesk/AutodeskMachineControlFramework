@echo off

set basepath=%~dp0
echo %basepath%
cd %basepath%

mkdir ..\_build_lib3mf_win64
cd ..\_build_lib3mf_win64

del ..\..\Artifacts\lib3mf\lib3mf_win64.dll
del ..\..\Artifacts\lib3mf\_githash_win64_lib3mf.txt

cmake ..\submodules\lib3mf
cmake --build . --config Release
copy Release\lib3mf.dll ..\..\Libraries\lib3mf\lib3mf_win64.dll /Y

git rev-parse --verify HEAD >"..\..\Artifacts\lib3mf\_githash_win64_lib3mf.txt"

cd ..

pause