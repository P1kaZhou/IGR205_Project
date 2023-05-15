
cmake_minimum_required(VERSION 3.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_BUILD_TYPE "debug")
add_definitions(-D_MY_OPENGL_IS_33_)

project(sketchy)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE dep/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE dep/glad/include/)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dep/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(dep/glm)
target_link_libraries(${PROJECT_NAME} glm)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} PRIVATE dep/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE dep/imgui/backends)

target_sources(
  ${PROJECT_NAME} PRIVATE
  base/base.cpp
  base/mesh.cpp
  base/camera.cpp
  base/camera-controller.cpp
  base/light.cpp
  base/mesh-geometry.cpp
  base/mesh-material.cpp
  base/texture.cpp
  base/program.cpp
  base/animation.cpp
  base/frame-buffer.cpp

  base/geometry/geometry.cpp

  dep/imgui/imgui.cpp
  dep/imgui/imgui_draw.cpp
  dep/imgui/imgui_tables.cpp
  dep/imgui/imgui_widgets.cpp
  dep/imgui/backends/imgui_impl_glfw.cpp
  dep/imgui/backends/imgui_impl_opengl3.cpp
  )
target_include_directories(
  ${PROJECT_NAME} PRIVATE
  base/)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})
