#define VK_USE_PLATFORM_WAYLAND_KHR
#include "cube.hpp"
#include "wayland_window.hpp"

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
template <class T> class add_wayland_surface_extension : public T {
public:
  auto get_extensions() {
    auto ext = T::get_extensions();
    ext.push_back(vk::KHRWaylandSurfaceExtensionName);
    return ext;
  }
};

template <class T> class add_dummy_recreate_surface : public T {
public:
  void recreate_surface() {}
};

template <class T>
class add_swapchain_image_extent_equal_surface_resolution : public T {
public:
  using parent = T;
  auto get_swapchain_image_extent() {
    auto [width, height] = parent::get_surface_resolution();
    return vk::Extent2D{static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)};
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

using app =
    run_event_loop<
    add_event_loop<
    add_dynamic_draw <
    add_acquire_next_image_semaphores <
    add_acquire_next_image_semaphore_fences <
    add_draw_semaphores <
    register_size_change_callback<
    add_recreate_surface_for<
    record_swapchain_command_buffers_cube <
    add_get_format_clear_color_value_type <
    add_recreate_surface_for<
    add_swapchain_command_buffers <
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
    add_recreate_surface_for<
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
    add_recreate_surface_for<
    add_framebuffers_cube <
    add_render_pass_cube <
    add_subpasses <
    add_subpass_dependency <
    add_empty_subpass_dependencies <
    add_depth_attachment<
    add_attachment <
    add_empty_attachments <
    add_pipeline_viewport_state <
    add_scissor_equal_swapchain_extent<
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
    add_recreate_surface_for<
    add_depth_images_views_cube<
    add_recreate_surface_for<
    barrier_depth_image_layout<
    add_recreate_surface_for<
    add_images_memories<
    add_image_memory_property<vk::MemoryPropertyFlagBits::eDeviceLocal,
    add_empty_image_memory_properties<
    add_recreate_surface_for<
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
    add_recreate_surface_for<
    add_swapchain_images_views <
    add_recreate_surface_for<
    add_swapchain_images <
    add_recreate_surface_for<
    add_swapchain <
    add_swapchain_image_extent_equal_surface_resolution<
    add_swapchain_image_format <
    add_command_pool <
    add_queue <
    add_device <
    add_swapchain_extension <
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
    none_t
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>
;

int main() {
  app app{};
  return 0;
}
