#define VK_USE_PLATFORM_WAYLAND_KHR
#include "cube.hpp"
#include "wayland_window.hpp"
#include <chrono>

using namespace std::literals;

using app =
  run_on_wayland_platform<
      vulkan_start::use_platform_add_cube_physical_device_and_device_and_draw<vulkan_start::platform::wayland>::
        add_cube_physical_device_and_device_and_draw
>
;

int main() {
  app app{};
  return 0;
}
