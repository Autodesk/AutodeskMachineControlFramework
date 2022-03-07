@echo off

echo "Building Go Server..."
go build -o ../build/Output/amc_server.exe -ldflags="-s -w" ../Server/mcserver.go

echo "done."

pause