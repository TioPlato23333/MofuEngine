cmake_minimum_required(VERSION 3.6)

project(MofuEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEBUG)
  set(BUILDCFG "Debug")
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb")
  add_compile_definitions(DEBUG)
else()
  set(BUILDCFG "Release")
  set(CMAKE_BUILD_TYPE "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
endif()

if(GL_ES)
  add_compile_definitions(GL_ES)
endif()

# FFmpeg ==================================================================
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY avutil)
find_library(AVSWRESAMPLE_LIBRARY swresample)
find_library(AVSWSCALE_LIBRARY swscale)

set(FFMPEG_LIBRARIES
  ${AVCODEC_LIBRARY}
  ${AVFORMAT_LIBRARY}
  ${AVUTIL_LIBRARY}
  ${AVSWRESAMPLE_LIBRARY}
  ${AVSWSCALE_LIBRARY}
)
# =========================================================================

include_directories(${CMAKE_SOURCE_DIR}/MofuEngine)

set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

add_library(MofuEngine OBJECT
  ${CMAKE_SOURCE_DIR}/MofuEngine/core/core.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/core/error.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/core/log.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/video/video.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/audio/audio.cpp
  ${CMAKE_SOURCE_DIR}/MofuEngine/util/renderer.cpp
)

target_link_libraries(MofuEngine
  ${FFMPEG_LIBRARIES}
)
