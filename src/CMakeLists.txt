cmake_minimum_required (VERSION 2.6)
project (ORCHID)

# The version number.
set(ORCHID_VERSION_MAJOR 0)
set(ORCHID_VERSION_MINOR 1)
set(ORCHID_VERSION_PATCH 0)
set(ORCHID_VERSION ${ORCHID_VERSION_MAJOR}.${ORCHID_VERSION_MINOR}.${ORCHID_VERSION_PATCH})

configure_file("${PROJECT_SOURCE_DIR}/Utility/OrchidConfig.h.in"
               "${PROJECT_BINARY_DIR}/Utility/OrchidConfig.h"
               @ONLY)

include_directories("${PROJECT_BINARY_DIR}/Utility")
include_directories("${PROJECT_BINARY_DIR}")

include(./CMakeFlagDefs.txt)

find_package(Boost REQUIRED)

add_definitions(-m64 -std=c++11 -D_FILE_OFFSET_BITS=64)

if(${BUILD_TYPE} STREQUAL "Release")
    add_definitions(${GCC_OPT_FLAG})
elseif(${BUILD_TYPE} STREQUAL "Debug")
    add_definitions(${GCC_DEB_FLAG})
elseif(${BUILD_TYPE} STREQUAL "OptDebug")
    add_definitions(-g -O2)
elseif(${BUILD_TYPE} STREQUAL "Warn")
    add_definitions(${WARN_FLAGS})
elseif(${BUILD_TYPE} STREQUAL "Warn_Opt")
    add_definitions(${WARN_FLAGS})
    add_definitions(${WARN_OPT_FLAGS})
endif()

include_directories( ${Boost_INCLUDE_DIR} )
# Grab all the source files
file(GLOB_RECURSE SRCS *.cpp)

# Grab all the header files
file(GLOB_RECURSE HDRS *.h)

include_directories(".")

# Set the executable
add_executable(orchid ${SRCS} ${HDRS})

target_link_libraries( orchid ${Boost_LIBRARIES} )
