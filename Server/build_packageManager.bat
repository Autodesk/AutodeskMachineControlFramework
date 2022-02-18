@echo off

echo "Building DownloadPackage..."
set GOARCH=amd64
set GOOS=windows
go build -o ../build_win64/DevPackage/packageManager.exe -ldflags="-s -w" ../Server/packageManager.go

set GOARCH=arm
set GOOS=linux
set GOARM=5
go build -o ../build_win64/DevPackage/packageManager.arm -ldflags="-s -w" ../Server/packageManager.go

set GOARCH=amd64
set GOOS=linux
go build -o ../build_win64/DevPackage/packageManager.linux64 -ldflags="-s -w" ../Server/packageManager.go

echo "done."

pause