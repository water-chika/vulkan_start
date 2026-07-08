cmake_minimum_required(VERSION 3.20)

project(vulkan_start)

find_package(Vulkan REQUIRED)

add_library(vulkan_start
    vulkan_start.hpp
    vulkan_start.cpp
)

target_include_directories(vulkan_start PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vulkan_start PUBLIC
    vulkan_helper
    win32_helper
    wayland_helper
    xkb_helper
    posix_cpp
)
set_target_properties(vulkan_start PROPERTIES CXX_STANDARD 20)

add_executable(demo
    cube.cpp
    cube.hpp
    shaders/cube.vert
    shaders/cube_vert.spv
    shaders/cube.frag
    shaders/cube_frag.spv
    shaders/mesh.glsl
    shaders/mesh.spv
    shaders/task.glsl
    shaders/task.spv
)
target_link_libraries(demo PUBLIC vulkan_start)
set_target_properties(demo PROPERTIES CXX_STANDARD 23)

if(NOT WIN32)
add_executable(cube_display
    cube_display.cpp
    cube.hpp
    shaders/cube.vert
    shaders/cube_vert.spv
    shaders/cube.frag
    shaders/cube_frag.spv
)
target_link_libraries(cube_display PUBLIC
    vulkan_start
)
set_target_properties(cube_display PROPERTIES CXX_STANDARD 23)

endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_command(OUTPUT shaders/vert.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert Vulkan::glslangValidator)

add_custom_command(OUTPUT shaders/cube_vert.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/cube_vert.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert Vulkan::glslangValidator)

add_custom_command(OUTPUT shaders/mesh.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/mesh.glsl
	      -S mesh
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/mesh.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/mesh.glsl
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/mesh.glsl Vulkan::glslangValidator)

add_custom_command(OUTPUT shaders/task.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/task.glsl
	      -S task
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/task.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/task.glsl
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/task.glsl Vulkan::glslangValidator)

add_custom_command(OUTPUT shaders/frag.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag Vulkan::glslangValidator)

add_custom_command(OUTPUT shaders/cube_frag.spv
  COMMAND Vulkan::glslangValidator --target-env vulkan1.3
              ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag
	      -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/cube_frag.spv
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag Vulkan::glslangValidator)
