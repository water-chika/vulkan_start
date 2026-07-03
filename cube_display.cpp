#define VK_USE_PLATFORM_DISPLAY_KHR
#include "cube.hpp"

using app =
  vulkan_start::run_on_platform<vulkan_start::platform::display,
      vulkan_start::use_platform_add_cube_physical_device_and_device_and_draw<vulkan_start::platform::display>::
        add_cube_physical_device_and_device_and_draw
  >
;

int main() {
    try{
      //auto conf = cpp_helper::empty_configure{};
        vulkan_start::empty_configure conf{};
        app t{conf};
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
