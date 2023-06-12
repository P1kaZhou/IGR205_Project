
cmake_minimum_required(VERSION 3.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_BUILD_TYPE "debug")
add_definitions(-D_MY_OPENGL_IS_33_)

project(sketchy)
add_executable(${PROJECT_NAME} main.cpp)

# CGAL
# find_package(CGAL QUIET COMPONENTS Core )
# include( ${CGAL_USE_FILE} )
# include_directories(${CGAL_INCLUDE_DIR})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

# GLAD
target_sources(${PROJECT_NAME} PRIVATE dep/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE dep/glad/include/)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dep/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# CDT
add_subdirectory(dep/CDT CDT)
target_link_libraries(${PROJECT_NAME} PRIVATE CDT)

# GLM
add_subdirectory(dep/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

# IMGUI
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

  base/modeling/medial-axis.cpp
  base/modeling/medial-axis-generator.cpp
  base/modeling/delaunay.cpp
  base/modeling/cylindrical-douglas-peucker.cpp
  base/modeling/cylinder-generator.cpp
  base/modeling/rigging.cpp
  base/modeling/skeleton-generator.cpp
  base/modeling/skining-generator.cpp
  base/modeling/mesh-generator.cpp

  dep/imgui/imgui.cpp
  dep/imgui/imgui_draw.cpp
  dep/imgui/imgui_tables.cpp
  dep/imgui/imgui_widgets.cpp
  dep/imgui/backends/imgui_impl_glfw.cpp
  dep/imgui/backends/imgui_impl_opengl3.cpp
  )
target_include_directories(
  ${PROJECT_NAME} PRIVATE
  base/
  dep/)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})
