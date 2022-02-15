@echo off

echo "----------------------------------"
echo "IMPORTING cppcodec"
echo "----------------------------------"
if not exist "..\Libraries\cppcodec\" (mkdir "..\Libraries\cppcodec\")
if not exist "..\Libraries\cppcodec\data" (mkdir "..\Libraries\cppcodec\data")
if not exist "..\Libraries\cppcodec\detail" (mkdir "..\Libraries\cppcodec\detail")
copy cppcodec\cppcodec\*.hpp "..\Libraries\cppcodec\" /Y
copy cppcodec\cppcodec\data\*.hpp "..\Libraries\cppcodec\data\" /Y
copy cppcodec\cppcodec\detail\*.hpp "..\Libraries\cppcodec\detail\" /Y
copy cppcodec\LICENSE "..\Libraries\cppcodec\" /Y
copy cppcodec\README.md "..\Libraries\cppcodec\" /Y

echo "saving git hash"
cd cppcodec
git rev-parse --verify HEAD >"..\..\Libraries\cppcodec\_githash_cppcodec.txt"
cd ..

echo .
echo .
echo "----------------------------------"
echo "IMPORTING crossguid"
echo "----------------------------------"
if not exist "..\Libraries\crossguid\" (mkdir "..\Libraries\crossguid\")
copy crossguid\include\crossguid\guid.hpp "..\Libraries\crossguid\" /Y
copy crossguid\src\guid.cpp "..\Libraries\crossguid\" /Y
copy crossguid\LICENSE "..\Libraries\crossguid\" /Y
copy crossguid\README.md "..\Libraries\crossguid\" /Y

echo "Patching header definitions..."
type _patches\crossguid_patch.txt >> ..\Libraries\crossguid\guid.hpp

echo "saving git hash"
cd crossguid
git rev-parse --verify HEAD >"..\..\Libraries\crossguid\_githash_crossguid.txt"
cd ..

echo .
echo .
echo "----------------------------------"
echo "IMPORTING lodePNG"
echo "----------------------------------"
if not exist "..\Libraries\LodePNG\" (mkdir "..\Libraries\LodePNG\")
copy lodepng\lodepng.cpp "..\Libraries\LodePNG\" /Y
copy lodepng\lodepng.h "..\Libraries\LodePNG\" /Y
copy lodepng\LICENSE "..\Libraries\LodePNG\" /Y
copy lodepng\README.md "..\Libraries\LodePNG\" /Y
echo "saving git hash"
cd lodepng
git rev-parse --verify HEAD >"..\..\Libraries\LodePNG\_githash_lodepng.txt"
cd ..


echo .
echo .
echo "----------------------------------"
echo "IMPORTING PicoSHA2"
echo "----------------------------------"
if not exist "..\Libraries\PicoSHA2\" (mkdir "..\Libraries\PicoSHA2\")
copy PicoSHA2\picosha2.h "..\Libraries\PicoSHA2\" /Y
copy PicoSHA2\LICENSE "..\Libraries\PicoSHA2\" /Y
copy PicoSHA2\README.md "..\Libraries\PicoSHA2\" /Y
echo "saving git hash"
cd PicoSHA2
git rev-parse --verify HEAD >"..\..\Libraries\PicoSHA2\_githash_PicoSHA2.txt"
cd ..


echo .
echo .
echo "----------------------------------"
echo "IMPORTING PugiXML"
echo "----------------------------------"
if not exist "..\Libraries\PugiXML\" (mkdir "..\Libraries\PugiXML\")
copy pugixml\src\pugixml.cpp "..\Libraries\PugiXML\" /Y
copy pugixml\src\pugixml.hpp "..\Libraries\PugiXML\" /Y
copy pugixml\LICENSE.md "..\Libraries\PugiXML\" /Y
copy pugixml\README.md "..\Libraries\PugiXML\" /Y
copy _patches\pugiconfig.hpp "..\Libraries\PugiXML\" /Y
echo "saving git hash"
cd pugixml
git rev-parse --verify HEAD >"..\..\Libraries\PugiXML\_githash_pugixml.txt"
cd ..


echo .
echo .
echo "----------------------------------"
echo "IMPORTING RapidJSON"
echo "----------------------------------"
if not exist "..\Libraries\RapidJSON\" (mkdir "..\Libraries\RapidJSON\")
if not exist "..\Libraries\RapidJSON\error\" (mkdir "..\Libraries\RapidJSON\error\")
if not exist "..\Libraries\RapidJSON\internal\" (mkdir "..\Libraries\RapidJSON\internal\")
if not exist "..\Libraries\RapidJSON\msinttypes\" (mkdir "..\Libraries\RapidJSON\msinttypes\")
copy rapidjson\include\rapidjson\*.h "..\Libraries\RapidJSON\" /Y
copy rapidjson\include\rapidjson\error\*.h "..\Libraries\RapidJSON\error\" /Y
copy rapidjson\include\rapidjson\internal\*.h "..\Libraries\RapidJSON\internal\" /Y
copy rapidjson\include\rapidjson\msinttypes\*.h "..\Libraries\RapidJSON\msinttypes\" /Y
copy rapidjson\readme.md "..\Libraries\RapidJSON\" /Y
copy rapidjson\license.txt "..\Libraries\RapidJSON\" /Y
echo "saving git hash"
cd rapidjson
git rev-parse --verify HEAD >"..\..\Libraries\RapidJSON\_githash_rapidjson.txt"
cd ..


pause