mkdir build
cd build

copy ..\Framework\Dist\disthash.txt .\githash.txt 
SET /p GITHASH=<githash.txt

call cmake ..
call cmake --build . --config Release

cd Output
copy template_config.xml %GITHASH%_config.xml

pause