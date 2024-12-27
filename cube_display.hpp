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
        throw std::runtime_error{"unsupported platform"};
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
      create_surface();
  }
  ~add_vulkan_surface() {
      destroy_surface();
  }
  void create_surface() {
      auto physical_device = parent::get_physical_device();
      auto display_properties = physical_device.getDisplayPropertiesKHR();
      if (display_properties.size() == 0) {
          throw std::runtime_error{"vulkan physical device get displayPropertiesKHR: display properties count is 0"};
      }
      auto display = display_properties.at(0).display;
      auto plane_properties = physical_device.getDisplayPlanePropertiesKHR();
      if (plane_properties.size() == 0) {
          throw std::runtime_error{"vulkan physical device get displayPlanePropertiesKHR: display plane properties count is 0"};
      }
      auto plane_index = 0;
      auto plane_current_display = plane_properties.at(plane_index).currentDisplay;
      auto plane_current_stack_index = plane_properties.at(plane_index).currentStackIndex;
      auto mode_properties = physical_device.getDisplayModePropertiesKHR(display);
      if (mode_properties.size() == 0) {
          throw std::runtime_error{"vulkan physical device get displayModePropertiesKHR: display mode properties count is 0"};
      }
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
  void destroy_surface() {
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

template<app APP>
class set_app_and_platform<APP, platform::display> {
public:
template<class T>
class add_physical_device_and_surface
    : public
    add_recreate_surface<
    use_platform<platform::display>::add_vulkan_surface<
    typename use_app<APP>::template add_physical_device<
    T
    >>>
{};
};


} //namespace vulkan_start

