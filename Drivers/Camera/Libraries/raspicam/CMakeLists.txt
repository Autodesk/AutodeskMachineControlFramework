#--------------------------------------------------------------------------
#   General Config
#--------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
project(raspicam)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
#   Creating C Library version of original userland/host_applications/linux/apps/raspicam
#--------------------------------------------------------------------------
file(GLOB raspicamc_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/raspicam_clib/*.c)
file(GLOB raspicamc_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/raspicam_clib/*.h)

add_library(raspicamc STATIC ${raspicamc_SOURCES})

target_include_directories(raspicamc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/raspicam_clib/include>)   

#--------------------------------------------------------------------------
#   Dependencies
#--------------------------------------------------------------------------
target_include_directories(raspicamc PRIVATE /opt/vc/include /opt/vc/include/interface)
target_link_directories(raspicamc PUBLIC /opt/vc/lib)
set(MMAL_LIBS mmal_core mmal_util mmal_vc_client)
target_link_libraries(raspicamc ${MMAL_LIBS} vcos bcm_host m dl pthread)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
#   C++ Library and Compile Settings
#   This is a lightweight C++ wrapper over the C library
#--------------------------------------------------------------------------
add_library(raspicam RaspiStill.cpp)
set_property(TARGET raspicam PROPERTY CXX_STANDARD 11)

target_include_directories(raspicam PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})   
#--------------------------------------------------------------------------
#   Dependencies
#--------------------------------------------------------------------------
target_include_directories(raspicam PUBLIC /opt/vc/include /opt/vc/include/interface)
target_link_directories(raspicam PUBLIC /opt/vc/lib)
target_link_libraries(raspicam raspicamc)