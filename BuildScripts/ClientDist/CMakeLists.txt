cmake_minimum_required(VERSION 3.10)
project(client_build_tools)

set(CMAKE_CXX_STANDARD 17)

set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

file(GLOB DEP_ZLIB ${REPO_ROOT}/Libraries/zlib/*.c)
file(GLOB DEP_PUGIXML ${REPO_ROOT}/Libraries/PugiXML/pugixml.cpp)

set(COMMON_SOURCES
	${REPO_ROOT}/Implementation/Common/common_utils.cpp
	${REPO_ROOT}/Implementation/Common/common_importstream_native.cpp
	${REPO_ROOT}/Implementation/Common/common_exportstream_native.cpp
	${REPO_ROOT}/Implementation/Common/common_exportstream_zip.cpp
	${REPO_ROOT}/Implementation/Common/common_portablezipwriter.cpp
	${REPO_ROOT}/Implementation/Common/common_portablezipwriterentry.cpp
	${REPO_ROOT}/Libraries/crossguid/guid.cpp
	${DEP_ZLIB}
)

set(COMMON_INCLUDES
	${REPO_ROOT}/Implementation/Common
	${REPO_ROOT}/Libraries
	${REPO_ROOT}/Libraries/PugiXML
	${REPO_ROOT}/
)

if(UNIX)
	find_library(LIBUUID_PATH uuid)
	if(NOT LIBUUID_PATH)
		message(FATAL_ERROR "libuuid not found")
	endif()
endif()

# create_client_dist
add_executable(create_client_dist
	${REPO_ROOT}/BuildScripts/createClientDist.cpp
	${COMMON_SOURCES}
	${DEP_PUGIXML}
)
target_include_directories(create_client_dist PRIVATE ${COMMON_INCLUDES})

# create_client_source
add_executable(create_client_source
	${REPO_ROOT}/BuildScripts/createClientSource.cpp
	${COMMON_SOURCES}
)
target_include_directories(create_client_source PRIVATE ${COMMON_INCLUDES})

if(WIN32)
	target_link_libraries(create_client_dist shlwapi.lib ws2_32.lib)
	target_link_libraries(create_client_source shlwapi.lib ws2_32.lib)
endif()

if(UNIX)
	target_link_libraries(create_client_dist ${LIBUUID_PATH})
	target_link_libraries(create_client_source ${LIBUUID_PATH})
endif()
