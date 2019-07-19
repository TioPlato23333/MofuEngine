cmake_minimum_required(VERSION 3.6)

project(demo)

set(CMAKE_CXX_STANDARD 11)

execute_process(COMMAND bash -c ${CMAKE_SOURCE_DIR}/tool/format.sh)

add_subdirectory(MofuEngine)

add_executable(main
  ${CMAKE_SOURCE_DIR}/demo/main.cpp
)

target_include_directories(main PRIVATE
  ${CMAKE_SOURCE_DIR}/MofuEngine
)

target_link_libraries(main
  MofuEngine
)
