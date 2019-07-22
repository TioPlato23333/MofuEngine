cmake_minimum_required(VERSION 3.6)

project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEBUG)
  set(BUILDCFG "debug")
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb")
  add_compile_definitions(DEBUG)
else()
  set(BUILDCFG "release")
  set(CMAKE_BUILD_TYPE "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
endif()

if(GL_ES)
  add_compile_definitions(GL_ES)
endif()

# FFmpeg ==================================================================
find_path(AVCODEC_INCLUDE_DIR libavcodec)
find_path(AVFORMAT_INCLUDE_DIR libavformat)
find_path(AVUTIL_INCLUDE_DIR libavutil)
find_path(AVSWRESAMPLE_INCLUDE_DIR libswresample)
find_path(AVSWSCALE_INCLUDE_DIR libswscale)

find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY avutil)
find_library(AVSWRESAMPLE_LIBRARY swresample)
find_library(AVSWSCALE_LIBRARY swscale)

set(FFMPEG_INCLUDE_DIRS
  ${AVCODEC_INCLUDE_DIR}
  ${AVFORMAT_INCLUDE_DIR}
  ${AVUTIL_INCLUDE_DIR}
  ${AVSWRESAMPLE_INCLUDE_DIR}
  ${AVSWSCALE_INCLUDE_DIR}
)

set(FFMPEG_LIBRARIES
  ${AVCODEC_LIBRARY}
  ${AVFORMAT_LIBRARY}
  ${AVUTIL_LIBRARY}
  ${AVSWRESAMPLE_LIBRARY}
  ${AVSWSCALE_LIBRARY}
)
# =========================================================================

# SDL2 ====================================================================
find_package(SDL2 REQUIRED)
# =========================================================================

include_directories("${FFMPEG_INCLUDE_DIRS}")
include_directories("${SDL2_INCLUDE_DIRS}")

set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/${BUILDCFG}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/${BUILDCFG}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/${BUILDCFG}/bin")

execute_process(COMMAND bash -c "${CMAKE_SOURCE_DIR}/tool/format.sh")
execute_process(COMMAND bash -c "${CMAKE_SOURCE_DIR}/tool/copy_resource.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_subdirectory(MofuEngine)

add_executable(main
  "${CMAKE_SOURCE_DIR}/demo/main.cpp"
)

target_include_directories(main PRIVATE
  "${CMAKE_SOURCE_DIR}/MofuEngine"
)

target_link_libraries(main
  MofuEngine
)
