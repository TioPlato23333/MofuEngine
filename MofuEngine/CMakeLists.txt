cmake_minimum_required(VERSION 3.6)

project(MofuEngine)

include_directories("${CMAKE_SOURCE_DIR}/MofuEngine")

add_library(MofuEngine OBJECT
  "${CMAKE_SOURCE_DIR}/MofuEngine/core/algo.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/core/core.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/core/error.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/core/log.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/video/video.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/audio/audio.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/util/renderer.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/util/shader_program.cpp"
  "${CMAKE_SOURCE_DIR}/MofuEngine/util/util.cpp"
)

target_link_libraries(MofuEngine
  ${FFMPEG_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
)
