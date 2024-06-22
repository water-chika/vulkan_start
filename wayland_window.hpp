#include <stdexcept>
#include <stdio.h>
#include <wayland-client.h>
#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "xdg-shell-client-protocol.h"

static
void randname(char *buf) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A' + (r&15)+(r&16)*2;
        r >>= 5;
    }
}
static
int create_shm_file(void) {
    int retries = 100;
    do{
        char name[] = "/wl_shm-XXXXXX";
        randname(name+sizeof(name) - 7);
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
    do{
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
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_shm* shm;
    struct xdg_wm_base* wm_base;

    struct wl_surface* surface;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* toplevel;

    float offset;
    uint32_t last_frame;
    int width, height;
    bool closed;
};

static
void buffer_release(void* data, struct wl_buffer* buffer) {
    wl_buffer_destroy(buffer);
}
static
const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release,
};
static
struct wl_buffer* draw_frame(struct our_state* state) {
    const int width = 640, height = 480;
    int stride = width * 4;
    int size = stride * height;
    int fd = allocate_shm_file(size);
    if (fd == -1) {
        return NULL;
    }

    uint32_t *data = (uint32_t*)mmap(NULL, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(state->shm, fd, size);
    struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0,
            width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    int offset = (int)state->offset % 8;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (((x+offset) + (y+offset) / 8 * 8) % 16 < 8)
                data[y * width + x] = 0xFF666666;
            else
                data[y * width + x] = 0xFFFFFFFF;
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &buffer_listener, NULL);
    return buffer;
}

static
void xdg_surface_configure(
        void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    struct our_state* state = (struct our_state*)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    struct wl_buffer* buffer = draw_frame(state);
    wl_surface_attach(state->surface, buffer, 0, 0);
    wl_surface_commit(state->surface);
}

static
const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static
void xdg_wm_base_ping(
        void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static
const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static
void registry_handle_global(
        void* data, struct wl_registry* registry,
        uint32_t name, const char* interface,
        uint32_t version) {
    struct our_state* state = (struct our_state*)data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->compositor = (wl_compositor*)wl_registry_bind(
                registry, name, &wl_compositor_interface, 4);
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->shm = (wl_shm*)wl_registry_bind(
                registry, name, &wl_shm_interface, 1);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        state->wm_base = (xdg_wm_base*)wl_registry_bind(
                registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->wm_base,
                &xdg_wm_base_listener, state);
    }
}
static
void registry_handle_global_remove(
        void* data, struct wl_registry* registry,
        uint32_t name) {
}

static
const struct wl_registry_listener
registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

static
void xdg_toplevel_configure(
        void* data,
        struct xdg_toplevel* xdg_toplevel,
        int32_t width, int32_t height,
        struct wl_array* states) {
    struct our_state* state = (struct our_state*)data;
    if (width == 0 || height == 0) {
        return;
    }
    state->width = width;
    state->height = height;
}

static
void xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    struct our_state* state = (struct our_state*)data;
    state->closed = true;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};


static
void wl_surface_frame_done(
        void* data, struct wl_callback* cb, uint32_t time
        ) {
    wl_callback_destroy(cb);

    struct our_state* state = (struct our_state*)data;
    //cb = wl_surface_frame(state->surface);
    //wl_callback_add_listener(cb, &wl_surface_frame_listener, state);

    if (state->last_frame != 0) {
        int elapsed = time - state->last_frame;
        state->offset += elapsed / 1000.0 * 24;
    }

    struct wl_buffer* buffer = draw_frame(state);
    wl_surface_attach(state->surface, buffer, 0, 0);
    wl_surface_damage_buffer(state->surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(state->surface);

    state->last_frame = time;
}

static
const struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};


#include <iostream>
#include <map>
#include <vector>

#include <wayland-client.h>

namespace wl_helper{
    inline wl_display* display_connect(const char* name) {
        return wl_display_connect(name);
    }
    template<class T>
    class set_default_display_name : public T{
    public:
        auto get_display_name() {
            return nullptr;
        }
    };
    template<class T>
    class add_display : public T{
    public:
        using parent = T;
        add_display() {
            m_display = display_connect(parent::get_display_name());
            if (m_display == nullptr) {
                throw std::runtime_error{"wayland: failed to connect display"};
            }
        }
        ~add_display() {
            std::clog << "display disconnect" << std::endl;
            wl_display_disconnect(m_display);
        }
        auto get_display() {
            return m_display;
        }
    private:
        wl_display* m_display;
    };
    template<class T>
    class add_event_loop : public T{
    public:
        using parent = T;
        void event_loop() {
            while (wl_display_dispatch(parent::get_wayland_display()) != -1) {
                parent::draw();
            }
        }
    };
    template<class T>
    class run_event_loop : public T{
    public:
        using parent = T;
        run_event_loop() {
            parent::event_loop();
        }
    };
    template<class T>
    class add_registry_listener : public T{
    public:
        using parent = T;
        add_registry_listener() {
            wl_registry* registry = parent::get_registry();
            wl_registry_listener listener = {
                .global = parent::registry_handle_global,
                .global_remove = parent::registry_handle_global_remove,
            };
            // listener is referenced but does not used immediately, so I use display_roundtrip to use it.
            wl_registry_add_listener(registry, &listener, parent::get_registry_listener_user_data());
            auto display = parent::get_display();
            wl_display_roundtrip(display);
        }
    };
    template<class T>
    class cache_registry : public T{
    public:
        using parent = T;
        cache_registry() {
            auto display = parent::get_display();
            m_registry = wl_display_get_registry(display);
        }
        auto get_registry() {
            return m_registry;
        }
    private:
        wl_registry* m_registry;
    };
    template<class T>
    class add_registry_listener_callbacks : public T{
    public:
        using this_type = add_registry_listener_callbacks<T>;
        void registry_handle_global(
            wl_registry* registry,
            uint32_t name,
            const char* interface,
            uint32_t version
            ) {
                std::vector<std::tuple<void*, const wl_interface*, int>> binds{
                    {&m_compositor, &wl_compositor_interface, 4},
                    {&m_shm, &wl_shm_interface, 1},
                };
                std::map<std::string, std::tuple<void*, const wl_interface*, int>> bind_map;
                for (auto& [state_ptr, state_interface, version] : binds) {
                    bind_map.emplace(state_interface->name, std::tuple{state_ptr, state_interface, version});
                }
                if (bind_map.contains(interface)) {
                    auto& [state_ptr, state_interface, version] = bind_map[interface];
                    *reinterpret_cast<void**>(state_ptr) = wl_registry_bind(registry, name, state_interface, version);
                }
            }
        static
        void registry_handle_global(
            void* data,
            wl_registry* registry,
            uint32_t name,
            const char* interface,
            uint32_t version
                ) {
                reinterpret_cast<this_type*>(data)->registry_handle_global(registry, name, interface, version);
        }
        static
            void registry_handle_global_remove(
                    void* data,
                    wl_registry* registry,
                    uint32_t name
                    ) {
        }
        void* get_registry_listener_user_data() {
            return this;
        }
        auto get_compositor() {
            return m_compositor;
        }
        auto get_shm() {
            return m_shm;
        }
    private:
        wl_compositor* m_compositor;
        wl_shm* m_shm;
    };
    template<typename T>
    class add_surface : public T{
    public:
        using parent = T;
        add_surface() {
            auto compositor = parent::get_compositor();
            m_surface = wl_compositor_create_surface(compositor);
        }
    private:
        wl_surface* m_surface;
    };
}


struct none_t{};

using namespace wl_helper;

template<class T>
class add_wayland_surface : public T{
public:
    add_wayland_surface() : state{}{
        state.display = wl_display_connect(NULL);
        if (!state.display) {
            fprintf(stderr, "Failed to connect to Wayland display.\n");
        }
        fprintf(stderr, "Connection established!\n");

        state.registry = wl_display_get_registry(state.display);
        wl_registry_add_listener(state.registry, &registry_listener, &state);
        wl_display_roundtrip(state.display);

        state.surface = wl_compositor_create_surface(state.compositor);
        state.xdg_surface = xdg_wm_base_get_xdg_surface(
                state.wm_base, state.surface);
        xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
        state.toplevel = xdg_surface_get_toplevel(state.xdg_surface);
        xdg_toplevel_set_title(state.toplevel, "Example client");
        xdg_toplevel_add_listener(state.toplevel,
                &xdg_toplevel_listener, &state);
        xdg_toplevel_set_title(state.toplevel, "Example client");
        wl_surface_commit(state.surface);

        wl_display_roundtrip(state.display);
        wl_display_roundtrip(state.display);
        wl_display_roundtrip(state.display);
        wl_display_roundtrip(state.display);

        //struct wl_callback* cb = wl_surface_frame(state.surface);
        //wl_callback_add_listener(cb, &wl_surface_frame_listener, &state);


        //while (!state.closed && wl_display_dispatch(state.display) != -1) {
        //}

        //wl_display_disconnect(state.display);
        //return 0;
    }
    auto get_wayland_display() {
        return state.display;
    }
    auto get_wayland_surface() {
        return state.surface;
    }
    auto get_surface_resolution() {
        return std::pair{640, 480};
    }
private:
        struct our_state state;
};
