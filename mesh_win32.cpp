#define VK_USE_PLATFORM_WIN32_KHR
#include "cube.hpp"

using app =
  vulkan_start::run_on_windows_platform<
      vulkan_start::use_platform_add_mesh_physical_device_and_device_and_draw<vulkan_start::platform::win32>::
        add_mesh_physical_device_and_device_and_draw
>
;

int main() {
  try{
    app app{};
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
