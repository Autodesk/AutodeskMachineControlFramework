#!/bin/bash
echo "Building Go Server..."
go build -o ../build/Output/amc_server -ldflags="-s -w" ../Server/mcserver.go

echo "done."
