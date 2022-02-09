@echo off

echo "Building DownloadPackage..."
go build -o ../build_win64/DevPackage/packageManager.exe -ldflags="-s -w" ../Server/packageManager.go

echo "done."

pause