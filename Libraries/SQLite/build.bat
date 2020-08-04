del sqlite3.lib /Y

mkdir build
cd build

cmake ..
cmake --build . --config Release
copy Release\sqlite3.lib ..

pause