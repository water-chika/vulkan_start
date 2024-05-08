cmake_minimum_required(VERSION 3.20)

project(vulkan_start)

find_package(Vulkan REQUIRED)
add_subdirectory(vulkan_helper)

add_executable(test
    main.cpp)
target_link_libraries(test PUBLIC vulkan_helper)
set_target_properties(test PROPERTIES CXX_STANDARD 23)
