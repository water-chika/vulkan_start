#define VK_USE_PLATFORM_DISPLAY_KHR
#include "cube.hpp"

#include <poll.h>

template <class T> class add_vulkan_surface : public T {
public:
  using parent = T;
  add_vulkan_surface() {
      create();
  }
  ~add_vulkan_surface() {
      destroy();
  }
  void create() {
      auto physical_device = parent::get_physical_device();
      auto display_properties = physical_device.getDisplayPropertiesKHR();
      auto display = display_properties.at(0).display;
      auto plane_properties = physical_device.getDisplayPlanePropertiesKHR();
      auto plane_index = 0;
      auto plane_current_display = plane_properties.at(plane_index).currentDisplay;
      auto plane_current_stack_index = plane_properties.at(plane_index).currentStackIndex;
      auto mode_properties = physical_device.getDisplayModePropertiesKHR(display);
      auto mode = mode_properties.at(0).displayMode;
      auto plane_capabilities = physical_device.getDisplayPlaneCapabilitiesKHR(mode, plane_index);
      m_surface_extent = plane_capabilities.maxDstExtent;
      auto create_info = vk::DisplaySurfaceCreateInfoKHR{}
        .setDisplayMode(mode)
        .setPlaneIndex(plane_index)
        .setImageExtent(m_surface_extent);
      auto instance = parent::get_instance();
      m_surface = instance.createDisplayPlaneSurfaceKHR(create_info);
  }
  void destroy() {
  }
  auto get_surface() { return m_surface; }
  auto get_surface_resolution() { return m_surface_extent; }

private:
  vk::SurfaceKHR m_surface;
  vk::Extent2D m_surface_extent;
};

using namespace vulkan_hpp_helper;
template <class T> class add_surface_needed_extension : public T {
public:
  auto get_extensions() {
    auto ext = T::get_extensions();
    ext.push_back(vk::KHRDisplayExtensionName);
    return ext;
  }
};



template<class T>
class add_run_loop : public T {
public:
    using parent = T;
    add_run_loop() {
        while (true) {
            parent::draw();

            pollfd fds[1];
            fds[0].fd = STDIN_FILENO;
            fds[0].events = POLLRDNORM;
            int timeout_ms = 0;
            int poll_ret = poll(fds, 1, timeout_ms);
            if (poll_ret > 0) {
                break;
            }
        }
    }
};

using namespace std::literals;

using app =
    add_run_loop<
    add_cube_resources_and_draw<
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return "shaders/cube_vert.spv"s;}), vk::ShaderStageFlagBits::eVertex,
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return "shaders/cube_frag.spv"s;}), vk::ShaderStageFlagBits::eFragment,
    set_shader_entry_name_with_main <
    add_empty_pipeline_stages <
    add_cube_swapchain_and_pipeline_layout<
    add_swapchain_image_extent_equal_surface_resolution<
    add_command_pool <
    add_queue <
    add_device <
    add_swapchain_extension <
    add_empty_extensions <
    add_find_properties <
    cache_physical_device_memory_properties<
    test_physical_device_support_surface<
    add_queue_family_index <
    cache_surface_capabilities<
    add_vulkan_surface<
    add_physical_device<
    add_dummy_recreate_surface<
    add_instance<
    add_surface_needed_extension<
    add_surface_extension<
    add_empty_extensions<
    empty_class
    >>>>>>>>>>>>>>>>>>>>>>>>>
;

int main() {
  app app{};
  return 0;
}
