#
# REST client application
#
#
project (REST_Client)

get_restclient_source(sources *.h *.cc)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(-DRAPIDJSON_MAXFLOAT=3.40282e+38 -DRAPIDJSON_MAXUINT64=4294967295 -DRAPIDJSON_MAXINT64=2147483647)

if (NOT ${os_name} STREQUAL "win")
  # enable pthread for gcc compiler
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
else()
    add_definitions(-DCURL_STATICLIB)
endif()

# Dependencies
include_directories("${root_directory}/Libraries/curl")
include_directories("${root_directory}/Libraries/rapidjson")
include_directories("${root_directory}/Libraries")

# Create a static library
ADD_LIBRARY(REST_Client ${sources})

if (NOT ${os_name} STREQUAL "win")
  target_link_libraries(REST_Client curl)
endif()
