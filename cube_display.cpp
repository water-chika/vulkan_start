#define VK_USE_PLATFORM_DISPLAY_KHR
#include "cube.hpp"

using app =
  vulkan_start::run_on_display_platform<
      vulkan_start::use_platform_add_cube_physical_device_and_device_and_draw<vulkan_start::platform::display>::
        add_cube_physical_device_and_device_and_draw
  >
;

int main() {
  app app{};
  return 0;
}
