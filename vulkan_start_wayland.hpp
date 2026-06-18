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
      create_surface();
  }
  ~add_vulkan_surface() {
      destroy_surface();
  }
  void create_surface()
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
  }
#endif
  void destroy_surface()
  {
    auto instance = parent::get_instance();
    instance.destroySurfaceKHR(m_surface);
  }
  auto get_surface() { return m_surface; }

private:
  vk::SurfaceKHR m_surface;
}; // add_vulkan_surface

template<class T>
class add_platform_needed_extensions : public add_wayland_surface_extension<T>
{
}; // add_platform_needed_extensions

template<class T>
class register_size_change_callback : public T{
public:
    using parent = T;
    using this_type = register_size_change_callback<T>;
    register_size_change_callback() {
        parent::set_size_changed_callback(size_changed_callback, this);
    }
    static void size_changed_callback(int width, int height, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->size_changed(width, height);
    }
    void size_changed(int width, int height) {
        parent::recreate_surface();
    }
};


template<class T>
class add_event_loop
    : public
      run_wayland_event_loop<
      add_wayland_event_loop<
      register_size_change_callback<
      T>>>
{
}; // class add_event_loop

template<class T>
class add_window : public add_wayland_surface<T>
{}; // class add_window

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
