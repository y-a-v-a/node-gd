cmake_minimum_required(VERSION 3.10)

project (node_gd)
include_directories(${CMAKE_JS_INC})

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: default is Release")
endif()

message(STATUS "CMAKE_VERSION=${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(FindPackageHandleStandardArgs)

FIND_PATH(LIBGD_INCLUDE_DIR gd.h
/usr/local/include
/usr/include
/opt/local/include
)

find_library(LIBGD_LIBRARY gd)

#MARK_AS_ADVANCED(GD_INCLUDE_DIR GD_LIBRARY)

find_package_handle_standard_args(
    libgd
    DEFAULT_MSG
    LIBGD_LIBRARY
    LIBGD_INCLUDE_DIR
)

message(STATUS "LIBGD_INCLUDE_DIR='${LIBGD_INCLUDE_DIR}'")
message(STATUS "LIBGD_LIBRARY=${LIBGD_LIBRARY}")
include_directories(${LIBGD_INCLUDE_DIR})

file(GLOB SOURCE_FILES "cpp/addon.cc")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_link_libraries(${PROJECT_NAME} ${LIBGD_LIBRARY})
