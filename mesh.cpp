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

using namespace std::chrono;
template<class T>
class add_frame_time_analyser : public T{
public:
    using parent = T;
    add_frame_time_analyser() {
    }
    void draw() {
        const int update_frames_count = 10000;
        if (m_frame_index % update_frames_count == 0) {
            auto now = steady_clock::now();
            m_frame_time = (now - m_last_time_point) / update_frames_count;
            double fps = 1000000000.0/m_frame_time.count();
            std::clog
                << "frame time: "
                << std::setw(10)
                << m_frame_time.count()/1000000.0
                << "ms"
                << "fps: "
                << fps
                << "\t\r";
            m_last_time_point = now;
        }

        parent::draw();

        m_frame_index++;
    }
private:
    nanoseconds m_frame_time;
    uint64_t m_frame_index;
    time_point<steady_clock, nanoseconds> m_last_time_point;
};

template<class T>
class add_vk_cmd_draw_mesh_tasks_ext : public T {
public:
    using parent = T;
    add_vk_cmd_draw_mesh_tasks_ext() {
        m_vk_cmd_draw_mesh_tasks_ext = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetInstanceProcAddr(parent::get_instance(), "vkCmdDrawMeshTasksEXT"));
    }
    static constexpr auto getVkHeaderVersion() {
        return VK_HEADER_VERSION;
    }
    auto vkCmdDrawMeshTasksEXT(VkCommandBuffer cmd, uint32_t x, uint32_t y, uint32_t z) const{
        return m_vk_cmd_draw_mesh_tasks_ext(cmd, x, y, z);
    }

private:
    PFN_vkCmdDrawMeshTasksEXT m_vk_cmd_draw_mesh_tasks_ext;
};

template <class T> class record_swapchain_command_buffers_mesh : public T {
public:
  using parent = T;
  record_swapchain_command_buffers_mesh() { create(); }
  void create() {
    auto buffers = parent::get_swapchain_command_buffers();
    auto swapchain_images = parent::get_swapchain_images();
    auto queue_family_index = parent::get_queue_family_index();
    auto framebuffers = parent::get_framebuffers();
    std::vector<vk::Buffer> uniform_buffers =
        parent::get_uniform_buffer_vector();
    std::vector<vk::Buffer> uniform_upload_buffers =
        parent::get_uniform_upload_buffer_vector();
    std::vector<vk::DescriptorSet> descriptor_sets =
        parent::get_descriptor_set();

    auto clear_color_value_type = parent::get_format_clear_color_value_type(
        parent::get_swapchain_image_format());
    using value_type = decltype(clear_color_value_type);
    std::map<value_type, vk::ClearColorValue> clear_color_values{
        {value_type::eFloat32,
         vk::ClearColorValue{}.setFloat32({0.4f, 0.4f, 0.4f, 0.0f})},
        {value_type::eUint32, vk::ClearColorValue{}.setUint32({50, 50, 50, 0})},
    };
    if (!clear_color_values.contains(clear_color_value_type)) {
      throw std::runtime_error{"unsupported clear color value type"};
    }
    vk::ClearColorValue clear_color_value{
        clear_color_values[clear_color_value_type]};
    auto clear_depth_value = vk::ClearDepthStencilValue{}.setDepth(1.0f);
    auto clear_values =
        std::array{vk::ClearValue{}.setColor(clear_color_value),
                   vk::ClearValue{}.setDepthStencil(clear_depth_value)};

    if (buffers.size() != swapchain_images.size()) {
      throw std::runtime_error{
          "swapchain images count != command buffers count"};
    }
    uint32_t index = 0;
    for (uint32_t index = 0; index < buffers.size(); index++) {
      vk::Image swapchain_image = swapchain_images[index];
      vk::CommandBuffer cmd = buffers[index];

      cmd.begin(vk::CommandBufferBeginInfo{});

      vk::Buffer uniform_buffer = uniform_buffers[index];
      vk::Buffer upload_buffer = uniform_upload_buffers[index];
      cmd.copyBuffer(upload_buffer, uniform_buffer,
                     vk::BufferCopy{}.setSize(sizeof(uint64_t)));
      auto uniform_buffer_memory_barrier =
          vk::BufferMemoryBarrier{}
              .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
              .setDstAccessMask(vk::AccessFlagBits::eUniformRead)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setBuffer(uniform_buffer)
              .setOffset(0)
              .setSize(vk::WholeSize);
      cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                          vk::PipelineStageFlagBits::eVertexShader, {}, {},
                          uniform_buffer_memory_barrier, {});

      vk::RenderPass render_pass = parent::get_render_pass();

      vk::Extent2D swapchain_image_extent =
          parent::get_swapchain_image_extent();
      auto render_area = vk::Rect2D{}
                             .setOffset(vk::Offset2D{0, 0})
                             .setExtent(swapchain_image_extent);
      vk::Framebuffer framebuffer = framebuffers[index];
      cmd.beginRenderPass(vk::RenderPassBeginInfo{}
                              .setRenderPass(render_pass)
                              .setRenderArea(render_area)
                              .setFramebuffer(framebuffer)
                              .setClearValues(clear_values),
                          vk::SubpassContents::eInline);

      vk::Pipeline pipeline = parent::get_pipeline();
      cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

      vk::PipelineLayout pipeline_layout = parent::get_pipeline_layout();
      vk::DescriptorSet descriptor_set = descriptor_sets[index];
      cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout,
                             0, descriptor_set, {});
      cmd.drawMeshTasksEXT(1,1,1, *this);
      cmd.endRenderPass();
      cmd.end();
    }
  }
  void destroy() {}
};

using namespace std::literals;

using app =
    run_event_loop<
    add_event_loop<
    add_frame_time_analyser<
    add_dynamic_draw <
    add_get_time <
    add_process_suboptimal_image<
        typeof([](auto* p) static {p->recreate_surface();std::cout << "recreate surface" << std::endl;}),
    add_acquire_next_image_semaphores <
    add_acquire_next_image_semaphore_fences <
    add_draw_semaphores <
    register_size_change_callback<
    add_recreate_surface_for<
    record_swapchain_command_buffers_mesh <
    add_vk_cmd_draw_mesh_tasks_ext<
    add_get_format_clear_color_value_type <
    add_recreate_surface_for<
    add_swapchain_command_buffers <
    write_descriptor_set<
    add_nonfree_descriptor_set<
    add_descriptor_pool<
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
    set_shader_stage < vk::ShaderStageFlagBits::eMeshEXT,
    add_shader_module <
    add_spirv_code <
    adapte_map_file_to_spirv_code <
    map_file_mapping <
    cache_file_size <
    add_file_mapping <
    add_file <
    add_file_path <typeof([]() static { return std::filesystem::path{"shaders/mesh.spv"}; }),
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
    add_file_path <typeof([]() static { return "shaders/cube_frag.spv"s; }),
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
    none_t
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>>>>>>>>>>>>>>
;

int main() {
  app app{};
  return 0;
}
