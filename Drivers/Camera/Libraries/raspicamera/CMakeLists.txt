cmake_minimum_required(VERSION 3.12)

project("MMALPP" VERSION 0.1 DESCRIPTION "A simple header only library that provide an interface with the broadcom MMAL library on the Raspberry PI to manage Components, Ports, Buffers, Queues, Pools and Connections."
                 HOMEPAGE_URL "https://github.com/s4mu313/MMALPP")

file(GLOB mmalpp_SRC ${CMAKE_CURRENT_SOURCE_DIR}/mmalpp/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/mmalpp/*.h)
add_library(MMALPP STATIC ${mmalpp_SRC})
target_include_directories(MMALPP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mmalpp/include)
target_include_directories(MMALPP PRIVATE /opt/vc/include)
target_link_directories(MMALPP PUBLIC /opt/vc/lib)
set(MMAL_LIBS mmal_core mmal_util mmal_vc_client)
target_link_libraries(MMALPP ${MMAL_LIBS} vcos bcm_host m dl pthread)

file(GLOB raspicamera_SRC ${CMAKE_CURRENT_SOURCE_DIR}/raspicamera/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/raspicamera/*.h)
add_library(raspicamera ${raspicamera_SRC})
target_link_libraries(raspicamera MMALPP)
target_include_directories(raspicamera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mmalpp/include /opt/vc/include)
target_include_directories(raspicamera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/raspicamera)
target_include_directories(raspicamera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
