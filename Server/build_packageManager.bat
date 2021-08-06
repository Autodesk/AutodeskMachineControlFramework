@echo off

echo "Building DownloadPackage..."
go build -o ../build/DevPackage/packageManager.exe -ldflags="-s -w" ../Server/packageManager.go

echo "done."

pause