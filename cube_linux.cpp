#include <stdexcept>
#include <iostream>

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
        static
            void registry_handle_global(
                void* data,
                wl_registry* registry,
                uint32_t name,
                const char* interface,
                uint32_t version
                ) {
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
    };
}

struct none_t{};

using namespace wl_helper;

using app =
    run_event_loop<
    add_event_loop<
    add_registry_listener<
    cache_registry<
    add_registry_listener_callbacks<
    add_display<
    set_default_display_name<
    none_t
    >>>>>>>
;

int main() {
    app app{};
    return 0;
}
