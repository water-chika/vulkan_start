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

template<class T>
class add_surface : public T{
public:
    add_surface() : state{}{
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
private:
        struct our_state state;
};

#define VK_USE_PLATFORM_WAYLAND_KHR
#include "cube.hpp"

template<class T>
class add_vulkan_surface : public T{
public:
    using parent = T;
    add_vulkan_surface() {
        auto instance = parent::get_instance();
        auto display = parent::get_wayland_display();
        auto surface = parent::get_wayland_surface();

        m_surface = instance.createWaylandSurfaceKHR(
                vk::WaylandSurfaceCreateInfoKHR{}
                .setDisplay(display)
                .setSurface(surface)
                );
    }
    auto get_surface() {
        return m_surface;
    }
private:
    vk::SurfaceKHR m_surface;
};

using namespace vulkan_hpp_helper;
template<class T>
class add_wayland_surface_extension : public T {
public:
    auto get_extensions() {
        auto ext = T::get_extensions();
        ext.push_back(vk::KHRWaylandSurfaceExtensionName);
        return ext;
    }
};

using app =
    run_event_loop<
    add_event_loop<
	add_dynamic_draw <
	add_acquire_next_image_semaphores <
	add_acquire_next_image_semaphore_fences <
	add_draw_semaphores <
	record_swapchain_command_buffers_cube <
	add_get_format_clear_color_value_type <
	add_swapchain_command_buffers <
	add_command_pool <
	add_queue <
	write_descriptor_set<
	add_nonfree_descriptor_set<
	add_descriptor_pool<
	add_buffer_memory_with_data_copy<
	rename_buffer_to_index_buffer<
	add_buffer_as_member<
	set_buffer_usage<vk::BufferUsageFlagBits::eIndexBuffer,
	add_cube_index_buffer_data<
	rename_buffer_vector_to_uniform_upload_buffer_vector <
	rename_buffer_memory_vector_to_uniform_upload_buffer_memory_vector<
	rename_buffer_memory_ptr_vector_to_uniform_upload_buffer_memory_ptr_vector<
	map_buffer_memory_vector<
	add_buffer_memory_vector<
	set_buffer_memory_properties < vk::MemoryPropertyFlagBits::eHostVisible,
	add_buffer_vector<
	set_vector_size_to_swapchain_image_count<
	set_buffer_usage<vk::BufferUsageFlagBits::eTransferSrc,
	rename_buffer_vector_to_uniform_buffer_vector<
	add_buffer_memory_vector<
	set_buffer_memory_properties<vk::MemoryPropertyFlagBits::eDeviceLocal,
	add_buffer_vector<
	set_vector_size_to_swapchain_image_count <
	add_buffer_usage<vk::BufferUsageFlagBits::eTransferDst,
	add_buffer_usage<vk::BufferUsageFlagBits::eUniformBuffer,
	empty_buffer_usage<
	set_buffer_size<sizeof(uint64_t),
	add_buffer_memory_with_data_copy <
	rename_buffer_to_vertex_buffer<
	add_buffer_as_member <
	set_buffer_usage<vk::BufferUsageFlagBits::eVertexBuffer,
	add_cube_vertex_buffer_data <
	add_graphics_pipeline <
	add_pipeline_vertex_input_state <
	add_vertex_binding_description <
	add_empty_binding_descriptions <
	add_vertex_attribute_description <
	set_vertex_input_attribute_format<vk::Format::eR32G32B32Sfloat,
	add_empty_vertex_attribute_descriptions <
	set_binding < 0,
	set_stride < sizeof(float) * 3,
	set_input_rate < vk::VertexInputRate::eVertex,
	set_subpass < 0,
	add_framebuffers_cube <
	add_render_pass_cube <
	add_subpasses <
	add_subpass_dependency <
	add_empty_subpass_dependencies <
	add_depth_attachment<
	add_attachment <
	add_empty_attachments <
	add_pipeline_viewport_state <
	add_scissor_equal_surface_rect <
	add_empty_scissors <
	add_viewport_equal_swapchain_image_rect <
	add_empty_viewports <
	set_tessellation_patch_control_point_count < 1,
	add_pipeline_stage_to_stages <
	add_pipeline_stage <
	set_shader_stage < vk::ShaderStageFlagBits::eVertex,
	add_shader_module <
	add_spirv_code <
	adapte_map_file_to_spirv_code <
	map_file_mapping <
	cache_file_size <
	add_file_mapping <
	add_file <
	add_cube_vertex_shader_path <
	add_pipeline_stage_to_stages <
	add_pipeline_stage <
	set_shader_stage < vk::ShaderStageFlagBits::eFragment,
	set_shader_entry_name_with_main <
	add_shader_module <
	add_spirv_code <
	adapte_map_file_to_spirv_code <
	map_file_mapping <
	cache_file_size <
	add_file_mapping <
	add_file <
	add_cube_fragment_shader_path <
	add_empty_pipeline_stages <
	add_pipeline_layout <
	add_single_descriptor_set_layout<
	add_descriptor_set_layout<
	add_cube_descriptor_set_layout_binding<
	set_pipeline_rasterization_polygon_mode < vk::PolygonMode::eFill,
	disable_pipeline_multisample <
	set_pipeline_input_topology < vk::PrimitiveTopology::eTriangleList,
	disable_pipeline_dynamic <
	enable_pipeline_depth_test <
	add_pipeline_color_blend_state_create_info <
	disable_pipeline_attachment_color_blend < 0, // disable index 0 attachment
	add_pipeline_color_blend_attachment_states < 1, // 1 attachment
	rename_images_views_to_depth_images_views<
	add_depth_images_views_cube<
	add_images_memories<
	add_image_memory_property<vk::MemoryPropertyFlagBits::eDeviceLocal,
	add_empty_image_memory_properties<
	add_images<
	add_image_type<vk::ImageType::e2D,
	set_image_tiling<vk::ImageTiling::eOptimal,
	set_image_samples<vk::SampleCountFlagBits::e1,
	add_image_extent_equal_swapchain_image_extent<
	add_image_usage<vk::ImageUsageFlagBits::eDepthStencilAttachment,
	add_empty_image_usages<
	rename_image_format_to_depth_image_format<
	add_image_format<vk::Format::eD32Sfloat,
	add_image_count_equal_swapchain_image_count<
	add_swapchain_images_views <
	add_swapchain_images <
	add_swapchain <
	add_swapchain_image_extent_equal_surface_current_extent <
	add_swapchain_image_format <
	add_device <
	add_swapchain_extension <
	add_empty_extensions <
	add_find_properties <
	cache_physical_device_memory_properties<
	cache_surface_capabilities<
	test_physical_device_support_surface<
	add_queue_family_index <
	add_physical_device<
	add_vulkan_surface<
	add_instance<
    add_wayland_surface_extension<
	add_surface_extension<
	add_empty_extensions<
    add_surface<
	empty_class
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>
;

int main() {
    app app{};
    return 0;
}
