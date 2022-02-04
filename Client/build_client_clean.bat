@echo off
mkdir ..\build_win64\Client
mkdir ..\build_win64\Client\public
mkdir ..\build_win64\Client\src
mkdir ..\build_win64\Client\src\plugins

copy public\*.* ..\build_win64\Client\public
copy src\*.* ..\build_win64\Client\src
copy src\plugins\*.* ..\build_win64\Client\src\plugins
copy *.js ..\build_win64\Client
copy *.json ..\build_win64\Client

cd ..\build_win64\Client

call npm install
call npm run build

cd ..\Client