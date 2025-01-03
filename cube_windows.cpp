#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include "cube.hpp"

using draw_cube_app =
	vulkan_start::run_on_platform<vulkan_start::platform::win32,
      vulkan_start::use_platform_add_cube_physical_device_and_device_and_draw<vulkan_start::platform::win32>::
        add_cube_physical_device_and_device_and_draw
	>
	;

int main() {
  try {
    auto app = draw_cube_app{};
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
