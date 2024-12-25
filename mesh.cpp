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
    register_size_change_callback<
    add_cube_resources_and_draw<
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
    none_t
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;

int main() {
  app app{};
  return 0;
}
