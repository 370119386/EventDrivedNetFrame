# CMakeList.txt: EventDrivedNetFrame 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_BUILE_TYPE DEBUG)

set(CMAKE_CXX_STANDARD 11)
if(!CMAKE_HOST_WIN32)
add_definitions(-std=c++11)
endif()

if(CMAKE_HOST_WIN32)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_HOST_WIN32)
set(PROJECT_LINK_DIR ${PROJECT_SOURCE_DIR}/libs/windows)
elseif(CMAKE_HOST_APPLE)
set(PROJECT_LINK_DIR ${PROJECT_SOURCE_DIR}/libs/ios)
else()
set(PROJECT_LINK_DIR ${PROJECT_SOURCE_DIR}/libs/unix)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
#= /DEBUG /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib
if(CMAKE_HOST_WIN32)
set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib)
endif()

set(CMAKE_DEBUG_POSTFIX "_d")

message("[PROJECT_SOURCE_DIR] = ${PROJECT_SOURCE_DIR}")
message("[PROJECT_BINARY_DIR] = ${PROJECT_BINARY_DIR}")
message("[PROJECT_LINK_DIR] = ${PROJECT_LINK_DIR}")
message("[PROJECT_LIBS_OUT_DIR] = ${PROJECT_LIBS_OUT_DIR}")
message("[CMAKE_LINK_LIBRARY_SUFFIX] = ${CMAKE_LINK_LIBRARY_SUFFIX}")
message("[CMAKE_SHARED_LIBRARY_SUFFIX] = ${CMAKE_SHARED_LIBRARY_SUFFIX}")
message("[CMAKE_DEBUG_POSTFIX] = ${CMAKE_DEBUG_POSTFIX}")
message("[CMAKE_ARCHIVE_OUTPUT_DIRECTORY] = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message("[CMAKE_LIBRARY_OUTPUT_DIRECTORY] = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("[CMAKE_RUNTIME_OUTPUT_DIRECTORY] = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("[CMAKE_EXE_LINKER_FLAGS] = ${CMAKE_EXE_LINKER_FLAGS}")


if(CMAKE_HOST_WIN32)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "." "./Dependencies/libevent/include" "./Dependencies/libevent/WIN32-Code/nmake")
else()
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "." "./Dependencies/libevent/include")
endif()
message("[CMAKE_INCLUDE_PATH] = ${CMAKE_INCLUDE_PATH}")

include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${PROJECT_LINK_DIR})

# 将源代码添加到此项目的可执行文件。
add_subdirectory("DDNFCommon/DDNFNetPlugin")

file(GLOB EventDrivedNetFrameH *.h)
file(GLOB EventDrivedNetFrameCC *.cc)

add_executable (EventDrivedNetFrame ${EventDrivedNetFrameH} ${EventDrivedNetFrameCC})

# TODO: 如有需要，请添加测试并安装目标。