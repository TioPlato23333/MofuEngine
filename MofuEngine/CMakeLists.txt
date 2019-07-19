cmake_minimum_required(VERSION 3.6)

project(MofuEngine)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/MofuEngine)

set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

add_library(MofuEngine OBJECT
  ${CMAKE_SOURCE_DIR}/MofuEngine/core/core.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/video/video.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/audio/audio.cpp
)
