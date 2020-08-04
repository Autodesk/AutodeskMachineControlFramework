@echo off
mkdir ..\build\Client
mkdir ..\build\Client\public
mkdir ..\build\Client\src
mkdir ..\build\Client\src\plugins

copy public\*.* ..\build\Client\public
copy src\*.* ..\build\Client\src
copy src\plugins\*.* ..\build\Client\src\plugins
copy *.js ..\build\Client
copy *.json ..\build\Client

cd ..\build\Client

call npm install
call npm run build

cd ..\Client