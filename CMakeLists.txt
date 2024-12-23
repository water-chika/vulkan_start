cmake_minimum_required(VERSION 3.20)

project(vulkan_start)

find_package(Vulkan REQUIRED)
add_subdirectory(vulkan_helper)

if(WIN32)

add_executable(triangle
    triangle.cpp
    shaders/vert.spv
    shaders/test.vert
    shaders/frag.spv
    shaders/test.frag
)
target_link_libraries(triangle PUBLIC vulkan_helper)
set_target_properties(triangle PROPERTIES CXX_STANDARD 23)

add_executable(cube
    cube_windows.cpp
    cube.hpp
    shaders/cube.vert
    shaders/cube_vert.spv
    shaders/cube.frag
    shaders/cube_frag.spv
)
target_link_libraries(cube PUBLIC vulkan_helper)
set_target_properties(cube PROPERTIES CXX_STANDARD 23)

add_executable(ray_tracing
    ray_tracing.cpp
)
target_link_libraries(ray_tracing PUBLIC vulkan_helper)
set_target_properties(ray_tracing PROPERTIES CXX_STANDARD 23)

else()
find_library(wayland-client wayland-client)
add_executable(cube
    cube_linux.cpp
    wayland/xdg-shell-protocol.c
    cube.hpp
    shaders/cube.vert
    shaders/cube_vert.spv
    shaders/cube.frag
    shaders/cube_frag.spv
)
target_link_libraries(cube PUBLIC vulkan_helper wayland-client)
target_include_directories(cube PUBLIC wayland)
set_target_properties(cube PROPERTIES CXX_STANDARD 23)

add_executable(mesh_test
    mesh.cpp
    wayland/xdg-shell-protocol.c
    cube.hpp
    shaders/mesh.glsl
    shaders/mesh.spv
    shaders/cube.frag
    shaders/cube_frag.spv
)
target_link_libraries(mesh_test PUBLIC vulkan_helper wayland-client)
target_include_directories(mesh_test PUBLIC wayland)
set_target_properties(mesh_test PROPERTIES CXX_STANDARD 23)

add_executable(cube_display
    cube_display.cpp
    cube.hpp
    shaders/cube.vert
    shaders/cube_vert.spv
    shaders/cube.frag
    shaders/cube_frag.spv
)
target_link_libraries(cube_display PUBLIC vulkan_helper)
target_include_directories(cube_display PUBLIC wayland)
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
