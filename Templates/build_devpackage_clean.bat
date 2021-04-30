mkdir build
cd build

mkdir Output\
mkdir Output\data

copy ..\Framework\Dist\disthash.txt .\githash.txt 
SET /p GITHASH=<githash.txt

call cmake ..
call cmake --build . --config Release

cd Output

pause