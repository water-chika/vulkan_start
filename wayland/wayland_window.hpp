#pragma once

#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <source_location>

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include <wayland-client.h>
//#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "xdg-shell-client-protocol.h"


static void randname(char *buf) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long r = ts.tv_nsec;
  for (int i = 0; i < 6; ++i) {
    buf[i] = 'A' + (r & 15) + (r & 16) * 2;
    r >>= 5;
  }
}
static int create_shm_file(void) {
  int retries = 100;
  do {
    char name[] = "/wl_shm-XXXXXX";
    randname(name + sizeof(name) - 7);
    --retries;
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd >= 0) {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);
  return -1;
}

int allocate_shm_file(size_t size) {
  int fd = create_shm_file();
  if (fd < 0)
    return -1;
  int ret;
  do {
    ret = ftruncate(fd, size);
  } while (ret < 0 && errno == EINTR);
  if (ret < 0) {
    close(fd);
    return -1;
  }
  return fd;
}

#define PROJECT_NAME "test_wayland"

struct our_state {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_shm *shm;
  struct xdg_wm_base *wm_base;

  struct wl_seat *seat;
  struct wl_keyboard *keyboard;
  std::vector<uint32_t> keymap;
  void (*key_callback)(int, int, void*);
  void* key_callback_user_data;

  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;
  struct xdg_toplevel *toplevel;

  float offset;
  uint32_t last_frame;
  int width, height;
  bool closed;
  bool size_changed;
  void (*size_changed_callback)(int, int, void*);
  void* size_changed_callback_user_data;
};

static void buffer_release(void *data, struct wl_buffer *buffer) {
  wl_buffer_destroy(buffer);
}
static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release,
};

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  struct our_state *state = (struct our_state *)data;
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void wl_keyboard_keymap(void* data, struct wl_keyboard* keyboard,
        uint32_t keymap_format, int fd, uint32_t size) {
    std::clog << "wayland: " << std::source_location::current().function_name() <<
        std::format("{}, {}, {}", keymap_format, fd, size)
        << std::endl;
    struct our_state *state = (struct our_state *)data;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));
    std::cout << std::endl;
    int count = size/sizeof(*ptr);
    state->keymap.resize(count);
    std::copy(ptr, ptr+count, state->keymap.begin());
    munmap(ptr, size);
}

static void wl_keyboard_enter(void* data, struct wl_keyboard* keyboard,
        uint32_t serial, wl_surface* surface, wl_array* keys) {
}

static void wl_keyboard_leave(void* data, struct wl_keyboard* keyboard,
        uint32_t serial, wl_surface* surface) {
}

static void wl_keyboard_key(void* data, struct wl_keyboard* keyboard,
        uint32_t serial, uint32_t time, uint32_t key, uint32_t key_state) {
    std::clog << "wayland: keyboard: key: " <<
        std::format("{}, {}, {:#x}, {}", serial, time, key, key_state)
        << std::endl;
    struct our_state *state = (struct our_state *)data;
    if (state->keymap.size() > key) {
        std::cout << std::format("{:#x}", state->keymap[key+8]) << std::endl;
    }
    if (state->key_callback) {
        state->key_callback(key, key_state, state->key_callback_user_data);
    }
}

static void wl_keyboard_modifiers(void* data, struct wl_keyboard* keyboard,
        uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
}

static void wl_keyboard_repeat_info(void* data, struct wl_keyboard* keyboard,
        int rate, int delay) {
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};

static void wl_seat_capabilities(void* data, struct wl_seat* seat, uint32_t capability) {
    std::clog << "wayland: seat: capabilities: " << capability << std::endl;
    struct our_state *state = (struct our_state *)data;
    if (capability & WL_SEAT_CAPABILITY_KEYBOARD){
        if (state->keyboard == NULL) {
            state->keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(state->keyboard, &wl_keyboard_listener, state);
        }
    }
    else {
        if (state->keyboard != NULL) {
            wl_keyboard_release(state->keyboard);
            state->keyboard = NULL;
        }
    }
}

static void wl_seat_name(void* data, struct wl_seat* seat, const char* name) {
    std::clog << "wayland: seat: name: " << name << std::endl;
}

static const struct wl_seat_listener wl_seat_listener = {
    .capabilities = wl_seat_capabilities,
    .name = wl_seat_name,
};

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface,
                                   uint32_t version) {
  struct our_state *state = (struct our_state *)data;
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    state->compositor = (wl_compositor *)wl_registry_bind(
        registry, name, &wl_compositor_interface, 4);
  }
  else if (strcmp(interface, wl_shm_interface.name) == 0) {
    state->shm =
        (wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 1);
  }
  else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    state->wm_base = (xdg_wm_base *)wl_registry_bind(registry, name,
                                                     &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(state->wm_base, &xdg_wm_base_listener, state);
  }
  else if (strcmp(interface, wl_seat_interface.name) == 0) {
    state->seat = (wl_seat *)wl_registry_bind(registry, name,
            &wl_seat_interface, 9);
    wl_seat_add_listener(state->seat, &wl_seat_listener, state);
  }
  else {
    std::clog << "wayland: not handled interface: " << interface << "(v" << version << ")" << std::endl;
  }
}
static void registry_handle_global_remove(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

static void xdg_toplevel_configure(void *data,
                                   struct xdg_toplevel *xdg_toplevel,
                                   int32_t width, int32_t height,
                                   struct wl_array *states) {
  struct our_state *state = (struct our_state *)data;
  if (width == 0 || height == 0) {
    return;
  }
  if (width != state->width || height != state->height) {
      state->width = width;
      state->height = height;
      state->size_changed = true;
      state->size_changed_callback(width, height,
              state->size_changed_callback_user_data);
  }
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *toplevel) {
  struct our_state *state = (struct our_state *)data;
  state->closed = true;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};

#include <wayland-client.h>

namespace wl_helper {
inline wl_display *display_connect(const char *name) {
  return wl_display_connect(name);
}
template <class T> class set_default_display_name : public T {
public:
  using parent = T;
  set_default_display_name(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
  }
  auto get_display_name() { return nullptr; }
};
template <class T> class add_display : public T {
public:
  using parent = T;
  add_display(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
    m_display = display_connect(parent::get_display_name());
    if (m_display == nullptr) {
      throw std::runtime_error{"wayland: failed to connect display"};
    }
  }
  ~add_display() {
    std::clog << "wayland: display disconnect" << std::endl;
    wl_display_disconnect(m_display);
  }
  auto get_display() { return m_display; }

private:
  wl_display *m_display;
};

template <class T> class add_registry_listener : public T {
public:
  using parent = T;
  add_registry_listener(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
    wl_registry *registry = parent::get_registry();
    wl_registry_listener listener = {
        .global = parent::registry_handle_global,
        .global_remove = parent::registry_handle_global_remove,
    };
    // listener is referenced but does not used immediately, so I use
    // display_roundtrip to use it.
    wl_registry_add_listener(registry, &listener,
                             parent::get_registry_listener_user_data());
    auto display = parent::get_display();
    wl_display_roundtrip(display);
  }
};
template <class T> class cache_registry : public T {
public:
  using parent = T;
  cache_registry(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
    auto display = parent::get_display();
    m_registry = wl_display_get_registry(display);
  }
  auto get_registry() { return m_registry; }

private:
  wl_registry *m_registry;
};
template <class T> class add_registry_listener_callbacks : public T {
public:
  using parent = T;
  using this_type = add_registry_listener_callbacks<T>;
  add_registry_listener_callbacks(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
  }
  void registry_handle_global(wl_registry *registry, uint32_t name,
                              const char *interface, uint32_t version) {
    std::vector<std::tuple<void *, const wl_interface *, int>> binds{
        {&m_compositor, &wl_compositor_interface, 4},
        {&m_shm, &wl_shm_interface, 1},
    };
    std::map<std::string, std::tuple<void *, const wl_interface *, int>>
        bind_map;
    for (auto &[state_ptr, state_interface, version] : binds) {
      bind_map.emplace(state_interface->name,
                       std::tuple{state_ptr, state_interface, version});
    }
    if (bind_map.contains(interface)) {
      auto &[state_ptr, state_interface, version] = bind_map[interface];
      *reinterpret_cast<void **>(state_ptr) =
          wl_registry_bind(registry, name, state_interface, version);
    }
  }
  static void registry_handle_global(void *data, wl_registry *registry,
                                     uint32_t name, const char *interface,
                                     uint32_t version) {
    reinterpret_cast<this_type *>(data)->registry_handle_global(
        registry, name, interface, version);
  }
  static void registry_handle_global_remove(void *data, wl_registry *registry,
                                            uint32_t name) {}
  void *get_registry_listener_user_data() { return this; }
  auto get_compositor() { return m_compositor; }
  auto get_shm() { return m_shm; }

private:
  wl_compositor *m_compositor;
  wl_shm *m_shm;
};
template <typename T> class add_surface : public T {
public:
  using parent = T;
  add_surface(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
    auto compositor = parent::get_compositor();
    m_surface = wl_compositor_create_surface(compositor);
  }

private:
  wl_surface *m_surface;
};
} // namespace wl_helper


using namespace wl_helper;



void water_chika_set_size_changed_callback(
        our_state& state,
        void (*callback)(int, int, void*), void* user_data) {
      if (state.size_changed) {
          callback(state.width, state.height, user_data);
          state.size_changed = false;
      }
      state.size_changed_callback = callback;
      state.size_changed_callback_user_data = user_data;
}

#endif

template <class T> class add_wayland_surface : public T {
public:
    using parent = T;
    static void dummy_size_changed_callback(int, int, void*) {
    }
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
  add_wayland_surface(const vulkan_hpp_helper::configure auto& conf) : parent{conf}, state{} {
      state.size_changed_callback = dummy_size_changed_callback;
    state.width = 640, state.height = 480;
    state.display = wl_display_connect(NULL);
    if (!state.display) {
        throw std::runtime_error{"Failed to connect to Wayland display"};
    }
    fprintf(stderr, "Connection established!\n");

    state.registry = wl_display_get_registry(state.display);
    wl_registry_add_listener(state.registry, &registry_listener, &state);
    wl_display_roundtrip(state.display);

    state.surface = wl_compositor_create_surface(state.compositor);
    state.xdg_surface =
        xdg_wm_base_get_xdg_surface(state.wm_base, state.surface);
    xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
    state.toplevel = xdg_surface_get_toplevel(state.xdg_surface);
    xdg_toplevel_set_title(state.toplevel, "Example client");
    xdg_toplevel_add_listener(state.toplevel, &xdg_toplevel_listener, &state);
    xdg_toplevel_set_title(state.toplevel, "Example client");
    wl_surface_commit(state.surface);

    wl_display_roundtrip(state.display);
  }
  auto get_wayland_display() { return state.display; }
  auto get_wayland_surface() { return state.surface; }
  auto get_surface_resolution() { return std::pair{state.width, state.height}; }
  auto get_event_loop_should_exit() { return state.closed; }
#endif
  void set_size_changed_callback(void (*callback)(int, int, void*), void* user_data) {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
      water_chika_set_size_changed_callback(state, callback, user_data);
#endif
  }
  void set_key_callback(void (*callback)(int, int, void*), void* user_data) {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
      state.key_callback = callback;
      state.key_callback_user_data = user_data;
#endif
  }

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
private:
  struct our_state state;
#endif
};

template <class T> class add_wayland_event_loop : public T {
public:
  using parent = T;
  add_wayland_event_loop(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
  }
  void event_loop() {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    while (!parent::get_event_loop_should_exit()) {
        // Dispath without blocking
        wl_display_dispatch_pending(parent::get_wayland_display());

        parent::draw();
    }
#endif
  }
};
template <class T> class run_wayland_event_loop : public T {
public:
  using parent = T;
  run_wayland_event_loop(const vulkan_hpp_helper::configure auto& conf) : parent{conf} {
      parent::event_loop();
  }
};

#include <poll.h>
template<typename T>
class add_wayland_pollfd : public T {
public:
    using parent = T;
    static constexpr int FDS_INDEX = parent::FDS_SIZE;
    static constexpr int FDS_SIZE = parent::FDS_SIZE+1;
    std::array<struct pollfd, FDS_SIZE> get_fds() {
        std::array<pollfd, FDS_SIZE> res{};
        auto fds = parent::get_fds();
        std::copy(fds.begin(), fds.end(), res.begin());
        res.back() = pollfd{
            .fd = wl_display_get_fd(parent::get_wayland_display()),
            .events = POLLIN,
        };
        return res;
    }
    void process_events(auto& fds) {
        if (fds[FDS_INDEX].revents & POLLIN) {
            auto display = parent::get_wayland_display();
            wl_display_read_events(display);
            wl_display_dispatch_pending(display);
        }
        parent::process_events(fds);
    }
};
