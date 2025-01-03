#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#else
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include "cube.hpp"

#ifdef WIN32
constexpr auto PLATFORM = vulkan_start::platform::win32;
#else
constexpr auto PLATFORM = vulkan_start::platform::wayland;
#endif

using draw_cube_app =
	vulkan_start::run_on_platform<PLATFORM,
      vulkan_start::use_platform_add_cube_physical_device_and_device_and_draw<PLATFORM>::
        add_cube_physical_device_and_device_and_draw
	>
	;
using draw_mesh_app =
	vulkan_start::run_on_platform<PLATFORM,
      vulkan_start::use_platform_add_mesh_physical_device_and_device_and_draw<PLATFORM>::
        add_mesh_physical_device_and_device_and_draw
	>
	;

using namespace std::literals;

int main(int argc, const char* argv[]) {
  try {
    if (argc < 2 || "cube"s == argv[1])
    {
      auto app = draw_cube_app{};
    }
    else
    {
      auto app = draw_mesh_app{};
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
