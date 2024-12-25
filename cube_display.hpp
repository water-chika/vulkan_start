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
}; // class add_vulkan_surface


template<class T>
class add_platform_needed_extensions : public T {
public:
  auto get_extensions() {
    auto ext = T::get_extensions();
    ext.push_back(vk::KHRDisplayExtensionName);
    return ext;
  }
}; // class add_platform_needed_extensions


template<class T>
class add_event_loop
    : public
    add_run_loop<
    T
    >
{
}; // class add_event_loop

}; // class use_platform<platform::display>

template<>
class use_platform_add_physical_device_and_surface<platform::display> {
public:
template<class T>
class add_physical_device_and_surface
    : public
    add_dummy_recreate_surface<
    use_platform<platform::display>::add_vulkan_surface<
    add_physical_device<
    T
    >>>
{};
};


} //namespace vulkan_start

