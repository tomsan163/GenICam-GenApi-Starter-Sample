# GenICam GenApi Starter Sample (C++)
# Copyright (c) 2026 tomsan163. All Rights Reserved.
cmake_minimum_required(VERSION 3.10.0)
set(sProjectName GenICam)

project(${sProjectName} VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

link_directories(../../lib)
add_executable(${sProjectName} main.cpp)
target_include_directories("${sProjectName}" PRIVATE ../../include)
if(NOT WIN32)
    target_link_libraries(${sProjectName} GCBase_gcc48_v3_4 GenApi_gcc48_v3_4)
endif()
