#pragma once

#include <wayland_helper.hpp>

#include <xkb_helper.hpp>
#include <posix.hpp>

namespace vulkan_start {

template<>
class use_platform<platform::wayland> {
public:

template <class T> class add_vulkan_surface : public T {
public:
  using parent = T;
  add_vulkan_surface(const configure auto& conf) : parent{conf} {
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
    register_size_change_callback(const configure auto& conf) : parent{conf} {
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
class register_key_callback : public T {
public:
    using parent = T;
    using this_type = register_key_callback<T>;
    register_key_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::key_event_processable T>
class register_key_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_key_callback<T>;
    register_key_callback(const configure auto& conf) : parent{conf} {
        parent::set_key_callback(key_callback, this);
    }
    static void key_callback(int key, int state, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_key_event(key, state);
    }
};
template<class T>
class register_keymap_callback : public T {
public:
    using parent = T;
    register_keymap_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::keymap_processable T>
class register_keymap_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_keymap_callback<T>;
    register_keymap_callback(const configure auto& conf) : parent{conf} {
        parent::set_keymap_callback(keymap_callback, this);
    }
    static void keymap_callback(int fd, int size, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_keymap(fd, size);
    }
};
template<class T>
class register_keyboard_modifiers_callback : public T {
public:
    using parent = T;
    register_keyboard_modifiers_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::keyboard_modifiers_event_processable T>
class register_keyboard_modifiers_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_keyboard_modifiers_callback<T>;
    register_keyboard_modifiers_callback(const configure auto& conf) : parent{conf} {
        parent::set_keyboard_modifiers_callback(keyboard_modifiers_callback, this);
    }
    static void keyboard_modifiers_callback(uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_keyboard_modifiers(mods_depressed, mods_latched, mods_locked, group);
    }
};
template<class T>
class register_pointer_motion_callback : public T {
public:
    using parent = T;
    using this_type = register_pointer_motion_callback<T>;
    register_pointer_motion_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::pointer_motion_event_processable T>
class register_pointer_motion_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_pointer_motion_callback<T>;
    register_pointer_motion_callback(const configure auto& conf) : parent{conf} {
        parent::set_pointer_motion_callback(pointer_motion_callback, this);
    }
    static void pointer_motion_callback(uint32_t x, uint32_t y, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_pointer_motion_event(x, y);
    }
};
template<class T>
class register_pointer_button_callback : public T {
public:
    using parent = T;
    using this_type = register_pointer_button_callback<T>;
    register_pointer_button_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::pointer_button_event_processable T>
class register_pointer_button_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_pointer_button_callback<T>;
    register_pointer_button_callback(const configure auto& conf) : parent{conf} {
        parent::set_pointer_button_callback(pointer_button_callback, this);
    }
    static void pointer_button_callback(int button, int button_state, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_pointer_button_event(button, button_state);
    }
};

template<class T>
class register_pointer_axis_callback : public T {
public:
    using parent = T;
    using this_type = register_pointer_axis_callback<T>;
    register_pointer_axis_callback(const configure auto& conf) : parent{conf} {
    }
};
template<xkb_helper::pointer_axis_event_processable T>
class register_pointer_axis_callback<T> : public T {
public:
    using parent = T;
    using this_type = register_pointer_axis_callback<T>;
    register_pointer_axis_callback(const configure auto& conf) : parent{conf} {
        parent::set_pointer_axis_callback(pointer_axis_callback, this);
    }
    static void pointer_axis_callback(uint32_t axis, int value, void* data) {
        auto th = reinterpret_cast<this_type*>(data);
        th->process_pointer_axis_event(axis, value);
    }
};

template<class T>
using add_event_loop_parent =
      wayland_helper::run_wayland_event_loop<
      wayland_helper::add_wayland_event_loop<
      register_pointer_axis_callback<
      register_pointer_button_callback<
      register_pointer_motion_callback<
      register_key_callback<
      register_keyboard_modifiers_callback<
      register_keymap_callback<
      register_size_change_callback<
      xkb_helper::add_process_key_event<
      xkb_helper::add_process_keyboard_modifiers<
      xkb_helper::add_process_keymap<
      xkb_helper::add_state<
      xkb_helper::add_keymap<
      xkb_helper::add_context<
      T>>>>>>>>>>>>>>>
;

template<class T>
class add_event_loop
    : public
      add_event_loop_parent<T>
{
public:
    using parent = add_event_loop_parent<T>;
    add_event_loop(const configure auto& conf) : parent{conf} {
    }
}; // class add_event_loop

template<typename T>
using add_pollfds_loop =
      wayland_helper::run_wayland_event_loop<
      wayland_helper::add_wayland_pollfds_loop<
      posix::add_poll_events<
      wayland_helper::add_wayland_pollfd<
      register_pointer_axis_callback<
      register_pointer_button_callback<
      register_pointer_motion_callback<
      register_key_callback<
      register_keyboard_modifiers_callback<
      register_keymap_callback<
      register_size_change_callback<
      xkb_helper::add_process_key_event<
      xkb_helper::add_process_keyboard_modifiers<
      xkb_helper::add_process_keymap<
      xkb_helper::add_state<
      xkb_helper::add_keymap<
      xkb_helper::add_context<
      T>>>>>>>>>>>>>>>>>
; // template add_pollfds

template<class T>
class add_window : public wayland_helper::add_wayland_surface<T>
{
public:
    using parent = wayland_helper::add_wayland_surface<T>;
    add_window(const configure auto& conf) : parent{conf} {}
}; // class add_window

}; // use_platform<platform::wayland>


template<>
class use_platform_add_swapchain_image_extent<platform::wayland> {
public:
template<class T>
class add_swapchain_image_extent
    : public add_swapchain_image_extent_equal_surface_resolution<T> {
public:
    using parent = add_swapchain_image_extent_equal_surface_resolution<T>;
    add_swapchain_image_extent(const configure auto& conf) : parent{conf} {
    }
};
};

} // namespace vulkan_start
