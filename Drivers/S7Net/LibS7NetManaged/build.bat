dotnet restore
dotnet build libs7net_managed.sln /t:Build /p:Configuration=Release /p:TargetFramework=netcoreapp3.0
copy ..\build\dotnet\bin\x64\Release\netcoreapp3.0\*.dll ..\Dependencies
copy ..\build\dotnet\bin\x64\Release\netcoreapp3.0\*.json ..\Dependencies

pause