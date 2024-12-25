#define VK_USE_PLATFORM_WAYLAND_KHR
#include "cube.hpp"
#include "wayland_window.hpp"
#include <chrono>

template <class T> class add_vulkan_surface : public T {
public:
  using parent = T;
  add_vulkan_surface() {
      create();
  }
  ~add_vulkan_surface() {
      destroy();
  }
  void create() {
    auto instance = parent::get_instance();
    auto display = parent::get_wayland_display();
    auto surface = parent::get_wayland_surface();

    m_surface = instance.createWaylandSurfaceKHR(
        vk::WaylandSurfaceCreateInfoKHR{}.setDisplay(display).setSurface(
            surface));
  }
  void destroy() {
    auto instance = parent::get_instance();
    instance.destroySurfaceKHR(m_surface);
  }
  auto get_surface() { return m_surface; }

private:
  vk::SurfaceKHR m_surface;
};

using namespace vulkan_hpp_helper;
template <std::invocable CALL, class T> class add_extension : public T {
public:
  auto get_extensions() {
    auto ext = T::get_extensions();
    ext.push_back(CALL::operator()());
    return ext;
  }
};

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

using namespace std::literals;

using app =
    run_event_loop<
    add_event_loop<
    register_size_change_callback<
    add_mesh_resources_and_draw<
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return "shaders/task.spv"s;}), vk::ShaderStageFlagBits::eTaskEXT,
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return "shaders/mesh.spv"s;}), vk::ShaderStageFlagBits::eMeshEXT,
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return "shaders/cube_frag.spv"s;}), vk::ShaderStageFlagBits::eFragment,
    set_shader_entry_name_with_main <
    add_empty_pipeline_stages <
    add_cube_swapchain_and_pipeline_layout<
    add_swapchain_image_extent_equal_surface_resolution<
    add_command_pool <
    add_queue <
    add_device <
    add_swapchain_extension <
    add_extension<typeof([]() static { return vk::EXTMeshShaderExtensionName; }),
    add_empty_extensions <
    add_find_properties <
    cache_physical_device_memory_properties<
    add_recreate_surface_for<
    cache_surface_capabilities<
    add_recreate_surface_for<
    test_physical_device_support_surface<
    add_queue_family_index <
    add_physical_device<
    add_recreate_surface_for<
    add_vulkan_surface<
    add_dummy_recreate_surface<
    add_instance<
    add_wayland_surface_extension<
    add_surface_extension<
    add_empty_extensions<
    add_wayland_surface<
    empty_class
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;

int main() {
  app app{};
  return 0;
}
