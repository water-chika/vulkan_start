#pragma once

#include "wayland/wayland_window.hpp"

namespace vulkan_start {

template<>
class use_platform<platform::wayland> {
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
  void create()
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
  {
    auto instance = parent::get_instance();
    auto display = parent::get_wayland_display();
    auto surface = parent::get_wayland_surface();

    m_surface = instance.createWaylandSurfaceKHR(
        vk::WaylandSurfaceCreateInfoKHR{}.setDisplay(display).setSurface(
            surface));
  }
#else
  {
    throw std::runtime_error{"wayland surface is not supported"};
  };
#endif
  void destroy()
  {
    auto instance = parent::get_instance();
    instance.destroySurfaceKHR(m_surface);
  }
  auto get_surface() { return m_surface; }

private:
  vk::SurfaceKHR m_surface;
}; // add_vulkan_surface
}; // use_platform<platform::wayland>


template<>
class use_platform_add_swapchain_image_extent<platform::wayland> {
public:
template<class T>
class add_swapchain_image_extent
    : public add_swapchain_image_extent_equal_surface_resolution<T> {
};
};

} // namespace vulkan_start
