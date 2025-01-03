#pragma once

#include <win32_helper.hpp>

namespace vulkan_start {

template <class T> class add_window_process : public T {
public:
  static LRESULT CALLBACK window_process(HWND hwnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) {
    static std::map<HWND, add_window_process<T> *> hwnd_this{};
    switch (uMsg) {
    case WM_CREATE: {
      auto create_struct = reinterpret_cast<CREATESTRUCT *>(lParam);
      hwnd_this.emplace(hwnd, reinterpret_cast<add_window_process<T> *>(
                                  create_struct->lpCreateParams));
      break;
    }
    case WM_SIZE: {
      uint16_t width = lParam;
      uint16_t height = lParam >> 16;
      hwnd_this[hwnd]->set_size(width, height);
      break;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
  }
  void *get_lparam() { return this; }
  void set_size(uint16_t width, uint16_t height) {
    m_width = width;
    m_height = height;
  }
  bool is_window_minimized() { return m_width == 0 || m_height == 0; }

private:
  static add_window_process<T> *m_this;
  uint16_t m_width;
  uint16_t m_height;
};
template <class T> class add_window_class : public T {
public:
  using parent = T;
  add_window_class() {
    const char *window_class_name = "draw_pixels";
    WNDCLASS window_class{};
    window_class.hInstance = GetModuleHandle(nullptr);
    window_class.lpszClassName = window_class_name;
    window_class.lpfnWndProc = parent::window_process;
    m_window_class = RegisterClass(&window_class);
  }
  ~add_window_class() {
    UnregisterClass(reinterpret_cast<LPCSTR>(m_window_class), GetModuleHandle(nullptr));
  }
  auto get_window_class() { return m_window_class; }

private:
  ATOM m_window_class;
};
template <int Width, int Height, class T>
class set_window_resolution : public T {
public:
  auto get_window_width() { return Width; }
  auto get_window_height() { return Height; }
};
template <int WindowStyle, class T> class set_window_style : public T {
public:
  auto get_window_style() { return WindowStyle; }
};
template <class T> class adjust_window_resolution : public T {
public:
  using parent = T;
  adjust_window_resolution() {
    auto width = parent::get_window_width();
    auto height = parent::get_window_height();
    auto window_style = parent::get_window_style();
    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, window_style, false);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;
  }
  auto get_window_width() { return m_width; }
  auto get_window_height() { return m_height; }

private:
  int m_width;
  int m_height;
};
template <class T> class add_window : public T {
public:
  using parent = T;
  add_window() {
    int width = parent::get_window_width();
    int height = parent::get_window_height();
    int window_style = parent::get_window_style();
    m_window =
        CreateWindowA(reinterpret_cast<LPCSTR>(parent::get_window_class()), "draw_pixels",
                      window_style, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                      nullptr, nullptr, GetModuleHandle(nullptr), parent::get_lparam());
    if (m_window == nullptr) {
      throw std::runtime_error("failed to create window");
    }
    ShowWindow(m_window, SW_SHOWNORMAL);
  }
  auto get_window() { return m_window; }

private:
  HWND m_window;
};



template<>
class use_platform<platform::win32> {
public:

template <class T> class add_vulkan_surface : public T {
public:
  using parent = T;
  add_vulkan_surface() { create_surface(); }
  ~add_vulkan_surface() { destroy_surface(); }
  void create_surface() {
#ifdef VULKAN_HPP_USING_PLATFORM_WIN32_KHR
    vk::Instance instance = parent::get_instance();
    m_surface =
        instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR{}
                                           .setHinstance(GetModuleHandle(nullptr))
                                           .setHwnd(parent::get_window()));
#else
    throw std::runtime_error{"Win32 surface is not supported"};
#endif
  }
  void destroy_surface() {
    vk::Instance instance = parent::get_instance();
    instance.destroySurfaceKHR(m_surface);
  }
  auto get_surface() { return m_surface; }

private:
  vk::SurfaceKHR m_surface;
}; //class add_vulkan_surface

template<class T>
class add_platform_needed_extensions : public add_win32_surface_extension<T>
{
};
template <class T> class add_event_loop : public jump_draw_if_window_minimized<T> {
public:
  using parent = T;
  add_event_loop() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } else {
        parent::draw();
      }
    }
    throw std::runtime_error{"Windows event loop is not supported"};
  }
}; // class add_event_loop
template <class T> class add_window
    : public T
{
public:
    auto get_window() {
      return m_window.get_window();
    }
private:
    vulkan_start::add_window<
	  adjust_window_resolution<
	  set_window_resolution<151, 151,
	  set_window_style<WS_OVERLAPPEDWINDOW,
	  add_window_class<
	  add_window_process<
    empty_class>>>>>> m_window;
}; // class add_window
}; // class use_platform<platform::win32>
} // namespace vulkan_start
