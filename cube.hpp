#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vulkan_helper.hpp>

namespace vulkan_start {
enum class platform {
    win32,
    wayland,
    display,
};


} // namespace vulkan_start


template <class T> class rename_images : public T {
public:
  using parent = T;
  auto get_intermediate_images() { return parent::get_images(); }
};

namespace vulkan_start {
enum class app {
    fast_debug,
    cube,
    mesh_test,
};

template <app APP>
class use_app {
public:

template <class T>
class record_swapchain_command_buffers : public T {
public:
    record_swapchain_command_buffers() = delete;
};

};

template<>
class use_app<app::cube> {
public:

template <class T> class record_swapchain_command_buffers : public T {
public:
  using parent = T;
  record_swapchain_command_buffers() { create(); }
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
      vk::Buffer vertex_buffer = parent::get_vertex_buffer();
      cmd.bindVertexBuffers(0, vertex_buffer, vk::DeviceSize{0});
      vk::Buffer index_buffer = parent::get_index_buffer();
      cmd.bindIndexBuffer(index_buffer, 0, vk::IndexType::eUint16);

      vk::PipelineLayout pipeline_layout = parent::get_pipeline_layout();
      vk::DescriptorSet descriptor_set = descriptor_sets[index];
      cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout,
                             0, descriptor_set, {});
      cmd.drawIndexed(3 * 2 * 3 * 2, 1, 0, 0, 0);
      cmd.endRenderPass();
      cmd.end();
    }
  }
  void destroy() {}
}; // class record_swapchain_command_buffers in use_app<app::cube>
}; // class use_app<app::cube>

template<>
class use_app<app::mesh_test> {
public:


template <class T> class record_swapchain_command_buffers : public T {
public:
  using parent = T;
  record_swapchain_command_buffers() { create(); }
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
}; // class record_swapchain_command_buffers in use_app<app::mesh_test>
}; // class use_app<app::mesh_test>

}; // namespace vulkan_start

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

template <class T>
class record_swapchain_command_buffers_fast_debug : public T {
public:
  using parent = T;
  record_swapchain_command_buffers_fast_debug() { create(); }
  void create() {
    auto buffers = parent::get_swapchain_command_buffers();
    auto swapchain_images = parent::get_swapchain_images();
    auto queue_family_index = parent::get_queue_family_index();
    auto render_images = parent::get_intermediate_images();
    auto unsampled_images = parent::get_images();
    auto image_extent = parent::get_image_extent();

    auto clear_color_value_type =
        parent::get_format_clear_color_value_type(parent::get_image_format());
    using value_type = decltype(clear_color_value_type);
    std::map<value_type, vk::ClearColorValue> clear_color_values{
        {value_type::eFloat32,
         vk::ClearColorValue{}.setFloat32({0.1f, 0.0f, 0.0f, 0.0f})},
        {value_type::eUint32, vk::ClearColorValue{}.setUint32({0, 0, 0, 0})},
    };
    if (!clear_color_values.contains(clear_color_value_type)) {
      throw std::runtime_error{"unsupported clear color value type"};
    }
    vk::ClearColorValue clear_color_value{
        clear_color_values[clear_color_value_type]};

    if (buffers.size() != swapchain_images.size()) {
      throw std::runtime_error{
          "swapchain images count != command buffers count"};
    }
    uint32_t index = 0;
    for (uint32_t index = 0; index < buffers.size(); index++) {
      vk::Image swapchain_image = swapchain_images[index];
      vk::Image render_image = render_images[index];
      vk::Image unsampled_image = unsampled_images[index];
      vk::CommandBuffer buffer = buffers[index];

      buffer.begin(vk::CommandBufferBeginInfo{});
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(render_image)
              .setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      buffer.clearColorImage(render_image, vk::ImageLayout::eTransferDstOptimal,
                             clear_color_value,
                             vk::ImageSubresourceRange{}
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                 .setLayerCount(1)
                                 .setLevelCount(1));
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(render_image)
              .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
              .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(unsampled_image)
              .setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      buffer.resolveImage(
          render_image, vk::ImageLayout::eTransferSrcOptimal, unsampled_image,
          vk::ImageLayout::eTransferDstOptimal,
          vk::ImageResolve{}
              .setSrcSubresource(
                  vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(
                      vk::ImageAspectFlagBits::eColor))
              .setDstSubresource(
                  vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(
                      vk::ImageAspectFlagBits::eColor))
              .setExtent(image_extent));
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(unsampled_image)
              .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
              .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(swapchain_image)
              .setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      vk::Offset3D image_end{static_cast<int32_t>(image_extent.width),
                             static_cast<int32_t>(image_extent.height),
                             static_cast<int32_t>(image_extent.depth)};
      buffer.blitImage(
          unsampled_image, vk::ImageLayout::eTransferSrcOptimal,
          swapchain_image, vk::ImageLayout::eTransferDstOptimal,
          vk::ImageBlit{}
              .setSrcSubresource(
                  vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(
                      vk::ImageAspectFlagBits::eColor))
              .setDstSubresource(
                  vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(
                      vk::ImageAspectFlagBits::eColor))
              .setSrcOffsets(std::array{vk::Offset3D{0, 0, 0}, image_end})
              .setDstOffsets(std::array{vk::Offset3D{0, 0, 0}, image_end}),
          vk::Filter::eNearest);
      buffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eAllCommands,
          vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
          vk::ImageMemoryBarrier{}
              .setImage(swapchain_image)
              .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
              .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead |
                                vk::AccessFlagBits::eMemoryWrite)
              .setSrcQueueFamilyIndex(queue_family_index)
              .setDstQueueFamilyIndex(queue_family_index)
              .setSubresourceRange(
                  vk::ImageSubresourceRange{}
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setLevelCount(1)));
      buffer.end();
    }
  }
  void destroy() {}
};


template <class T> class record_swapchain_command_buffers : public T {
public:
  using parent = T;
  record_swapchain_command_buffers() { create(); }
  void create() {
    auto buffers = parent::get_swapchain_command_buffers();
    auto swapchain_images = parent::get_swapchain_images();
    auto queue_family_index = parent::get_queue_family_index();
    auto framebuffers = parent::get_framebuffers();

    if (buffers.size() != swapchain_images.size()) {
      throw std::runtime_error{
          "swapchain images count != command buffers count"};
    }
    uint32_t index = 0;
    for (uint32_t index = 0; index < buffers.size(); index++) {
      vk::Image swapchain_image = swapchain_images[index];
      vk::CommandBuffer buffer = buffers[index];

      buffer.begin(vk::CommandBufferBeginInfo{});
      vk::RenderPass render_pass = parent::get_render_pass();

      vk::Extent2D swapchain_image_extent =
          parent::get_swapchain_image_extent();
      auto render_area = vk::Rect2D{}
                             .setOffset(vk::Offset2D{0, 0})
                             .setExtent(swapchain_image_extent);
      vk::Framebuffer framebuffer = framebuffers[index];
      buffer.beginRenderPass(vk::RenderPassBeginInfo{}
                                 .setRenderPass(render_pass)
                                 .setRenderArea(render_area)
                                 .setFramebuffer(framebuffer),
                             vk::SubpassContents::eInline);
      auto clear_color_value_type = parent::get_format_clear_color_value_type(
          parent::get_swapchain_image_format());
      using value_type = decltype(clear_color_value_type);
      std::map<value_type, vk::ClearColorValue> clear_color_values{
          {value_type::eFloat32,
           vk::ClearColorValue{}.setFloat32({1.0f, 0.0f, 0.0f, 0.0f})},
          {value_type::eUint32,
           vk::ClearColorValue{}.setUint32({255, 0, 0, 0})},
      };
      if (!clear_color_values.contains(clear_color_value_type)) {
        throw std::runtime_error{"unsupported clear color value type"};
      }
      vk::ClearColorValue clear_color_value{
          clear_color_values[clear_color_value_type]};
      buffer.clearAttachments(
          vk::ClearAttachment{}
              .setAspectMask(vk::ImageAspectFlagBits::eColor)
              .setClearValue(vk::ClearValue{}.setColor(clear_color_value)),
          vk::ClearRect{}.setLayerCount(1).setRect(
              vk::Rect2D{}.setExtent(swapchain_image_extent)));
      vk::Pipeline pipeline = parent::get_pipeline();
      buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
      parent::command_buffer_draw(buffer);
      buffer.endRenderPass();
      buffer.end();
    }
  }
  void destroy() {}
};

template <class T> class add_compute_pipeline : public T {
public:
  using parent = T;
  add_compute_pipeline() { create(); }
  ~add_compute_pipeline() { destroy(); }
  void create() {
    vk::Device device = parent::get_device();
    vk::PipelineShaderStageCreateInfo stage = parent::get_shader_stage();

    auto res_value = device.createComputePipeline(
        nullptr, vk::ComputePipelineCreateInfo{}.setStage(stage));
    if (res_value.result == vk::Result::eSuccess) {
      m_pipeline = res_value.value;
    } else {
      throw std::runtime_error{"failed to create compute pipeline"};
    }
  }
  void destroy() {
    vk::Device device = parent::get_device();
    device.destroyPipeline(m_pipeline);
  }

private:
  vk::Pipeline m_pipeline;
};

template <class T> class add_triangle_vertex_buffer_data : public T {
public:
  auto get_buffer_size() { return m_data.size() * sizeof(m_data[0]); }
  auto get_buffer_data() { return m_data; }

private:
  static constexpr auto m_data = std::array{
      -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f,

  };
};

template <class T> class add_cube_vertex_buffer_data : public T {
public:
  auto get_buffer_size() { return m_data.size() * sizeof(m_data[0]); }
  auto get_buffer_data() { return m_data; }

private:
  static constexpr auto m_data = std::array{
      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
  };
};
template <class T> class add_cube_index_buffer_data : public T {
public:
  using parent = T;
  auto get_buffer_size() { return m_data.size() * sizeof(m_data[0]); }
  auto get_buffer_data() { return m_data; }

private:
  static constexpr auto m_data = std::array<uint16_t, 3 * 3 * 2 * 2>{
      0, 1, 3, 0, 3, 2, 4, 6, 7, 4, 7, 5, 1, 5, 7, 1, 7, 3,
      3, 7, 6, 3, 6, 2, 2, 6, 4, 2, 4, 0, 0, 4, 5, 0, 5, 1,
  };
};
template <std::invocable<> CALL, class T> class add_file_path : public T {
public:
    auto get_file_path() { return CALL::operator()(); }
};

template <class T> class add_cube_descriptor_set_layout_binding : public T {
public:
  using parent = T;
  add_cube_descriptor_set_layout_binding() {
    vk::ShaderStageFlagBits stages = vk::ShaderStageFlagBits::eVertex;
    m_binding = vk::DescriptorSetLayoutBinding{}
                    .setBinding(0)
                    .setDescriptorCount(1)
                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                    .setStageFlags(stages);
  }
  auto get_descriptor_set_layout_bindings() { return m_binding; }

private:
  vk::DescriptorSetLayoutBinding m_binding;
};
template <class T> class add_descriptor_pool : public T {
public:
  using parent = T;
  add_descriptor_pool() { create(); }
  ~add_descriptor_pool() { destroy(); }
  void create() {
    vk::Device device = parent::get_device();
    uint32_t count = parent::get_swapchain_images().size();
    auto pool_sizes =
        vk::DescriptorPoolSize{}.setDescriptorCount(count).setType(
            vk::DescriptorType::eUniformBuffer);
    m_pool = device.createDescriptorPool(
        vk::DescriptorPoolCreateInfo{}.setMaxSets(count).setPoolSizes(
            pool_sizes));
  }
  void destroy() {
    vk::Device device = parent::get_device();

    device.destroyDescriptorPool(m_pool);
  }
  auto get_descriptor_pool() { return m_pool; }

private:
  vk::DescriptorPool m_pool;
};
template <class T> class add_nonfree_descriptor_set : public T {
public:
  using parent = T;
  add_nonfree_descriptor_set() { create(); }
  ~add_nonfree_descriptor_set() { destroy(); }
  void create() {
    vk::Device device = parent::get_device();
    vk::DescriptorPool pool = parent::get_descriptor_pool();
    vk::DescriptorSetLayout layout = parent::get_descriptor_set_layout();
    uint32_t count = parent::get_swapchain_images().size();
    std::vector<vk::DescriptorSetLayout> layouts(count);
    std::ranges::for_each(layouts, [layout](auto &l) { l = layout; });
    m_set = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo{}
                                              .setDescriptorPool(pool)
                                              .setDescriptorSetCount(count)
                                              .setSetLayouts(layouts));
  }
  void destroy() {}
  auto get_descriptor_set() { return m_set; }

private:
  std::vector<vk::DescriptorSet> m_set;
};
template <class T> class set_vector_size_to_swapchain_image_count : public T {
public:
  using parent = T;
  auto get_vector_size() { return parent::get_swapchain_images().size(); }
};
template <class T> class write_descriptor_set : public T {
public:
  using parent = T;
  write_descriptor_set() { create(); }
  auto create() {
    vk::Device device = parent::get_device();
    std::vector<vk::Buffer> buffers = parent::get_uniform_buffer_vector();
    std::vector<vk::DescriptorSet> sets = parent::get_descriptor_set();

    std::vector<vk::WriteDescriptorSet> writes(sets.size());
    std::vector<vk::DescriptorBufferInfo> buffer_infos(sets.size());
    for (uint32_t i = 0; i < sets.size(); i++) {
      auto &buffer = buffers[i];
      auto &set = sets[i];
      auto &write = writes[i];
      auto &buffer_info = buffer_infos[i];

      buffer_info =
          vk::DescriptorBufferInfo{}.setBuffer(buffer).setRange(vk::WholeSize);

      write = vk::WriteDescriptorSet{}
                  .setDstSet(set)
                  .setDescriptorCount(1)
                  .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                  .setDstBinding(0)
                  .setBufferInfo(buffer_info);
    }
    device.updateDescriptorSets(writes, {});
  }
};
template <class T> class add_framebuffers_cube : public T {
public:
  using parent = T;
  add_framebuffers_cube() { create_framebuffers(); }
  ~add_framebuffers_cube() { destroy_framebuffers(); }
  void create() {
      create_framebuffers();
  }
  void destroy() {
      destroy_framebuffers();
  }
  void create_framebuffers() {
    vk::Device device = parent::get_device();
    vk::RenderPass render_pass = parent::get_render_pass();
    auto extent = parent::get_swapchain_image_extent();
    uint32_t width = extent.width;
    uint32_t height = extent.height;
    auto swapchain_image_views = parent::get_swapchain_image_views();
    auto depth_image_views = parent::get_depth_images_views();
    m_framebuffers.resize(swapchain_image_views.size());
    for (uint32_t i = 0; i < swapchain_image_views.size(); i++) {
      auto depth_image_view = depth_image_views[i];
      auto swapchain_image_view = swapchain_image_views[i];
      auto &framebuffer = m_framebuffers[i];
      auto attachments = std::array{swapchain_image_view, depth_image_view};

      framebuffer = device.createFramebuffer(vk::FramebufferCreateInfo{}
                                                 .setAttachments(attachments)
                                                 .setRenderPass(render_pass)
                                                 .setWidth(width)
                                                 .setHeight(height)
                                                 .setLayers(1));
    }
  }
  void destroy_framebuffers() {
    vk::Device device = parent::get_device();
    std::ranges::for_each(m_framebuffers, [device](auto framebuffer) {
      device.destroyFramebuffer(framebuffer);
    });
  }
  auto get_framebuffers() { return m_framebuffers; }

private:
  std::vector<vk::Framebuffer> m_framebuffers;
};
template <class T> class add_depth_images_views_cube : public T {
public:
  using parent = T;
  add_depth_images_views_cube() { create(); }
  ~add_depth_images_views_cube() { destroy(); }
  void create() {
    vk::Device device = parent::get_device();
    auto images = parent::get_images();
    vk::Format format = parent::get_image_format();

    m_views.resize(images.size());
    std::ranges::transform(
        images, m_views.begin(), [device, format](auto image) {
          return device.createImageView(
              vk::ImageViewCreateInfo{}
                  .setImage(image)
                  .setFormat(format)
                  .setSubresourceRange(
                      vk::ImageSubresourceRange{}
                          .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                          .setLayerCount(1)
                          .setLevelCount(1))
                  .setViewType(vk::ImageViewType::e2D));
        });
  }
  void destroy() {
    vk::Device device = parent::get_device();
    std::ranges::for_each(
        m_views, [device](auto view) { device.destroyImageView(view); });
  }
  auto get_images_views() { return m_views; }

private:
  std::vector<vk::ImageView> m_views;
};
template <class T> class add_render_pass_cube : public T {
public:
  using parent = T;
  add_render_pass_cube() {
    vk::Device device = parent::get_device();
    auto attachments = parent::get_attachments();
    auto dependencies = parent::get_subpass_dependencies();
    auto color_attachment =
        vk::AttachmentReference{}.setAttachment(0).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);
    auto depth_attachment =
        vk::AttachmentReference{}.setAttachment(1).setLayout(
            vk::ImageLayout::eDepthStencilAttachmentOptimal);
    auto subpasses =
        std::array{vk::SubpassDescription{}
                       .setColorAttachments(color_attachment)
                       .setPDepthStencilAttachment(&depth_attachment)};

    m_render_pass = device.createRenderPass(vk::RenderPassCreateInfo{}
                                                .setAttachments(attachments)
                                                .setDependencies(dependencies)
                                                .setSubpasses(subpasses));
  }
  ~add_render_pass_cube() {
    vk::Device device = parent::get_device();
    device.destroyRenderPass(m_render_pass);
  }
  auto get_render_pass() { return m_render_pass; }

private:
  vk::RenderPass m_render_pass;
};

template <class F, class T> class add_process_suboptimal_image : public T {
public:
    using parent = T;
    void process_suboptimal_image() {
        F::operator()(this);
    }
};

template <class T> class add_get_time : public T {
public:
    add_get_time() : m_start_time{std::chrono::steady_clock::now()}{
    }
    auto get_time() {
        return std::chrono::steady_clock::now() - m_start_time;
    }
private:
    std::chrono::steady_clock::time_point m_start_time;
};

template <class T> class add_dynamic_draw : public T {
public:
  using parent = T;
  void draw() {
    vk::Device device = parent::get_device();
    vk::SwapchainKHR swapchain = parent::get_swapchain();
    vk::Queue queue = parent::get_queue();
    vk::Semaphore acquire_image_semaphore =
        parent::get_acquire_next_image_semaphore();
    bool need_recreate_surface = false;

    auto [res, index] =
        device.acquireNextImage2KHR(vk::AcquireNextImageInfoKHR{}
                                        .setSwapchain(swapchain)
                                        .setSemaphore(acquire_image_semaphore)
                                        .setTimeout(UINT64_MAX)
                                        .setDeviceMask(1));
    if (res == vk::Result::eSuboptimalKHR) {
      need_recreate_surface = true;
    } else if (res != vk::Result::eSuccess) {
      throw std::runtime_error{"acquire next image != success"};
    }
    parent::free_acquire_next_image_semaphore(index);

    vk::Fence acquire_next_image_semaphore_fence =
        parent::get_acquire_next_image_semaphore_fence(index);
    {
      vk::Result res = device.waitForFences(acquire_next_image_semaphore_fence,
                                            true, UINT64_MAX);
      if (res != vk::Result::eSuccess) {
        throw std::runtime_error{"failed to wait fences"};
      }
    }
    device.resetFences(acquire_next_image_semaphore_fence);

    auto time = parent::get_time();
    auto time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    uint64_t frame_index = time_in_ms.count();
    std::vector<void *> upload_memory_ptrs =
        parent::get_uniform_upload_buffer_memory_ptr_vector();
    void *upload_ptr = upload_memory_ptrs[index];
    memcpy(upload_ptr, &frame_index, sizeof(frame_index));
    std::vector<vk::DeviceMemory> upload_memory_vector =
        parent::get_uniform_upload_buffer_memory_vector();
    vk::DeviceMemory upload_memory = upload_memory_vector[index];
    device.flushMappedMemoryRanges(vk::MappedMemoryRange{}
                                       .setMemory(upload_memory)
                                       .setOffset(0)
                                       .setSize(vk::WholeSize));

    vk::Semaphore draw_image_semaphore =
        parent::get_draw_image_semaphore(index);
    vk::CommandBuffer buffer = parent::get_swapchain_command_buffer(index);
    vk::PipelineStageFlags wait_stage_mask{
        vk::PipelineStageFlagBits::eTopOfPipe};
    queue.submit(vk::SubmitInfo{}
                     .setCommandBuffers(buffer)
                     .setWaitSemaphores(acquire_image_semaphore)
                     .setWaitDstStageMask(wait_stage_mask)
                     .setSignalSemaphores(draw_image_semaphore),
                 acquire_next_image_semaphore_fence);
    try {
      auto res = queue.presentKHR(vk::PresentInfoKHR{}
                                      .setImageIndices(index)
                                      .setSwapchains(swapchain)
                                      .setWaitSemaphores(draw_image_semaphore));
      if (res == vk::Result::eSuboptimalKHR) {
        need_recreate_surface = true;
      } else if (res != vk::Result::eSuccess) {
        throw std::runtime_error{"present return != success"};
      }
    } catch (vk::OutOfDateKHRError e) {
      need_recreate_surface = true;
    }
    if (need_recreate_surface) {
      queue.waitIdle();
      parent::process_suboptimal_image();
    }
  }
  ~add_dynamic_draw() {
    vk::Device device = parent::get_device();
    vk::Queue queue = parent::get_queue();
    queue.waitIdle();
  }
};


static void water_chika_vulkan_barrier_depth_image_layout(
    vk::Device device, uint32_t queue_family_index, vk::Queue queue,
    vk::CommandPool cmd_pool, std::vector<vk::Image> images) {
  auto cmds = device.allocateCommandBuffers(
      vk::CommandBufferAllocateInfo{}
          .setCommandPool(cmd_pool)
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1));
  auto cmd = cmds[0];

  cmd.begin(vk::CommandBufferBeginInfo{});

  std::vector<vk::ImageMemoryBarrier> depth_image_barriers(images.size());
  std::ranges::transform(
      images, depth_image_barriers.begin(), [queue_family_index](auto &image) {
        return vk::ImageMemoryBarrier{}
            .setSrcAccessMask(vk::AccessFlagBits::eNone)
            .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
            .setSrcQueueFamilyIndex(queue_family_index)
            .setDstQueueFamilyIndex(queue_family_index)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setImage(image)
            .setSubresourceRange(
                vk::ImageSubresourceRange{}
                    .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                    .setLevelCount(1)
                    .setLayerCount(1));
      });

  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                      vk::PipelineStageFlagBits::eAllCommands, {}, {},
                      {}, depth_image_barriers);
  cmd.end();

  queue.submit(vk::SubmitInfo{}.setCommandBuffers(cmd));
  queue.waitIdle();

  device.freeCommandBuffers(cmd_pool, cmd);
}

template <class T> class barrier_depth_image_layout : public T {
public:
  using parent = T;
  barrier_depth_image_layout() {
      create();
  }
  void create() {
    auto device = parent::get_device();
    auto cmd_pool = parent::get_command_pool();
    auto images = parent::get_images();
    auto queue_family_index = parent::get_queue_family_index();
    auto queue = parent::get_queue();
    water_chika_vulkan_barrier_depth_image_layout(device, queue_family_index,
                                                  queue, cmd_pool, images);
  }
  void destroy() {
  }
};

using namespace vulkan_hpp_helper;

template <std::invocable<> CALL, vk::ShaderStageFlagBits STAGE, class T> class add_spirv_file_to_pipeline_stages
    : public
    vulkan_hpp_helper::add_pipeline_stage_to_stages <
    add_pipeline_stage <
    set_shader_stage < STAGE,
    add_shader_module <
    add_spirv_code <
    adapte_map_file_to_spirv_code <
    map_file_mapping <
    cache_file_size <
    add_file_mapping <
    add_file <
    add_file_path <CALL,
    T
    >>>>>>>>>>>
{};

template <class T> class add_cube_swapchain_and_pipeline_layout
  : public
  add_pipeline_layout<
	add_single_descriptor_set_layout<
	add_descriptor_set_layout<
	add_cube_descriptor_set_layout_binding<
	set_pipeline_rasterization_polygon_mode< vk::PolygonMode::eFill,
	disable_pipeline_multisample<
	set_pipeline_input_topology< vk::PrimitiveTopology::eTriangleList,
	disable_pipeline_dynamic<
	enable_pipeline_depth_test<
	add_pipeline_color_blend_state_create_info<
	disable_pipeline_attachment_color_blend< 0, // disable index 0 attachment
	add_pipeline_color_blend_attachment_states< 1, // 1 attachment
	rename_images_views_to_depth_images_views<
	add_recreate_surface_for<
	barrier_depth_image_layout<
	add_recreate_surface_for<
	add_depth_images_views_cube<
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
	add_swapchain_images_views<
	add_recreate_surface_for<
	add_swapchain_images<
	add_recreate_surface_for<
	add_swapchain<
	add_swapchain_image_format<
  T
  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
{};

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

template <class T> class add_cube_resources_and_draw
  : public
    add_frame_time_analyser<
    add_dynamic_draw <
    add_get_time <
    add_process_suboptimal_image<
        typeof([](auto* p) static {p->recreate_surface();std::cout << "recreate surface" << std::endl;}),
    add_acquire_next_image_semaphores <
    add_acquire_next_image_semaphore_fences <
    add_draw_semaphores <
    add_recreate_surface_for<
    vulkan_start::use_app<vulkan_start::app::cube>::record_swapchain_command_buffers<
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
    T
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

{};


template <class T> class add_mesh_resources_and_draw
  : public
    add_frame_time_analyser<
    add_dynamic_draw <
    add_get_time <
    add_process_suboptimal_image<
        typeof([](auto* p) static {p->recreate_surface();std::cout << "recreate surface" << std::endl;}),
    add_acquire_next_image_semaphores <
    add_acquire_next_image_semaphore_fences <
    add_draw_semaphores <
    add_recreate_surface_for<
    vulkan_start::use_app<vulkan_start::app::mesh_test>::record_swapchain_command_buffers<
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
    T
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

{};

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


namespace vulkan_start {

template<platform PLATFORM>
class use_platform {
};

template<platform PLATFORM>
class use_platform_add_swapchain_image_extent {
public:
template<class T>
class add_swapchain_image_extent
    : public add_swapchain_image_extent_equal_surface_current_extent<T> {
};
};

template<platform PLATFORM>
class use_platform_add_cube_physical_device_and_device_and_draw {
public:

template<class T>
class add_cube_physical_device_and_device_and_draw
    : public
    add_cube_resources_and_draw<
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return std::string{"shaders/cube_vert.spv"};}), vk::ShaderStageFlagBits::eVertex,
    add_spirv_file_to_pipeline_stages<
        typeof([]() static {return std::string{"shaders/cube_frag.spv"};}), vk::ShaderStageFlagBits::eFragment,
	set_shader_entry_name_with_main <
	add_empty_pipeline_stages <
	add_cube_swapchain_and_pipeline_layout<
    typename use_platform_add_swapchain_image_extent<PLATFORM>::template add_swapchain_image_extent<
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
  T
  >>>>>>>>>>>>>>>>>>>>
{};
}; // class use_platform_*

} // namespace vulkan_start

#include "cube_wayland.hpp"
#include "cube_windows.hpp"
#include "wayland/wayland_window.hpp"


#if !defined(WS_OVERLAPPEDWINDOW)
#define WS_OVERLAPPEDWINDOW 0
#endif

using namespace windows_helper;
using namespace vulkan_hpp_helper;
using namespace std::literals;
template <template<typename> typename C> class run_on_windows_platform
  : public
  add_window_loop <
	jump_draw_if_window_minimized <
  C<
  add_recreate_surface<
	vulkan_windows_helper::add_windows_surface<
	add_instance<
	add_win32_surface_extension<
	add_surface_extension<
	add_empty_extensions<
	add_window<
	adjust_window_resolution<
	set_window_resolution<151, 151,
	set_window_style<WS_OVERLAPPEDWINDOW,
	add_window_class<
	add_window_process<
  empty_class
  >>>>>>>>>>>>>>>
{};

namespace wayland_platform {
using namespace vulkan_hpp_helper;


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

template <template<typename> typename C> class run_on_wayland_platform
  : public
    run_event_loop<
    add_event_loop<
    register_size_change_callback<
    C<
    add_recreate_surface_for<
    vulkan_start::use_platform<vulkan_start::platform::wayland>::add_vulkan_surface<
    add_dummy_recreate_surface<
    add_instance<
    add_wayland_surface_extension<
    add_surface_extension<
    add_empty_extensions<
    add_wayland_surface<
    empty_class
    >>>>>>>>>>>>
{};
}

using wayland_platform::run_on_wayland_platform;
