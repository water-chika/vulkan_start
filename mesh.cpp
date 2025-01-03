#define VK_USE_PLATFORM_WAYLAND_KHR
#include "cube.hpp"

using app =
  vulkan_start::run_on_platform<vulkan_start::platform::wayland,
      vulkan_start::use_platform_add_mesh_physical_device_and_device_and_draw<vulkan_start::platform::wayland>::
        add_mesh_physical_device_and_device_and_draw
>
;

int main() {
  app app{};
  return 0;
}
