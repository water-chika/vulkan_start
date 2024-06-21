#include <stdexcept>
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
            while (wl_display_dispatch(parent::get_display()) != -1) {
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

using app =
    run_event_loop<
    add_event_loop<
    add_surface<
    add_registry_listener<
    cache_registry<
    add_registry_listener_callbacks<
    add_display<
    set_default_display_name<
    none_t
    >>>>>>>>
;

int main() {
    app app{};
    return 0;
}
