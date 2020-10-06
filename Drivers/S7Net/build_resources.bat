del Resources/*.dll
del Resources/*.runtimeconfig.json
del Resources/*.runtimeconfig.dev.json

mkdir build

cd LibS7NetManaged

dotnet restore
dotnet build libs7net_managed.sln /t:Build /p:Configuration=Release /p:TargetFramework=netcoreapp3.0
copy ..\build\dotnet\bin\x64\Release\netcoreapp3.0\*.dll ..\Resources
copy ..\build\dotnet\bin\x64\Release\netcoreapp3.0\*.runtimeconfig.json ..\Resources
copy ..\build\dotnet\bin\x64\Release\netcoreapp3.0\*.runtimeconfig.dev.json ..\Resources

cd ..\build

cmake ..\LibS7Net
cmake --build . --config Release

pause