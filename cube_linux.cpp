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
}

struct none_t{};

using namespace wl_helper;

using app =
    run_event_loop<
    add_event_loop<
    add_display<
    set_default_display_name<
    none_t
    >>>>
;

int main() {
    app app{};
    return 0;
}
