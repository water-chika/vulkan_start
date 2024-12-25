#pragma once

#ifdef linux
#include <poll.h>
#endif

namespace vulkan_start{
template<class T>
class add_run_loop : public T {
public:
    using parent = T;
    add_run_loop() {
#ifdef linux
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
#else
        throw std::runtime{"unsupported platform"};
#endif
    }
};

template<>
class use_platform<platform::display> {
public:
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
};

template<>
class use_platform_add_cube_physical_device_and_device_and_draw<platform::display> {
public:
    static constexpr auto PLATFORM = platform::display;
template<class T>
class add_cube_physical_device_and_device_and_draw
    : public
    add_cube_resources_and_draw<
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return std::string{"shaders/cube_vert.spv"};}), vk::ShaderStageFlagBits::eVertex,
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return std::string{"shaders/cube_frag.spv"};}), vk::ShaderStageFlagBits::eFragment,
	set_shader_entry_name_with_main <
	add_empty_pipeline_stages <
	add_cube_swapchain_and_pipeline_layout<
    typename use_platform_add_swapchain_image_extent<PLATFORM>::template add_swapchain_image_extent<
	add_command_pool <
	add_queue <
	add_device <
	add_swapchain_extension <
	add_empty_extensions <
	add_find_properties <
	cache_physical_device_memory_properties<
	add_recreate_surface_for<
	cache_surface_capabilities<
	add_recreate_surface_for<
	test_physical_device_support_surface<
	add_queue_family_index <
    vulkan_start::use_platform<PLATFORM>::add_vulkan_surface<
  add_physical_device<
  T
  >>>>>>>>>>>>>>>>>>>>>
{};
}; // class use_platform_*
} //namespace vulkan_start

