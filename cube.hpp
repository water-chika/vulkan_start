#include <vulkan_helper.hpp>
#include <iostream>
#include <numeric>
#include <map>
#include <string>

template<class T>
class rename_images : public T {
public:
	using parent = T;
	auto get_intermediate_images() {
		return parent::get_images();
	}
};

template<class T>
class record_swapchain_command_buffers_fast_debug : public T {
public:
	using parent = T;
	record_swapchain_command_buffers_fast_debug() {
		create();
	}
	void create() {
		auto buffers = parent::get_swapchain_command_buffers();
		auto swapchain_images = parent::get_swapchain_images();
		auto queue_family_index = parent::get_queue_family_index();
		auto render_images = parent::get_intermediate_images();
		auto unsampled_images = parent::get_images();
		auto image_extent = parent::get_image_extent();

		auto clear_color_value_type = parent::get_format_clear_color_value_type(parent::get_image_format());
		using value_type = decltype(clear_color_value_type);
		std::map<value_type, vk::ClearColorValue> clear_color_values{
			{value_type::eFloat32, vk::ClearColorValue{}.setFloat32({0.1f,0.0f,0.0f,0.0f})},
			{value_type::eUint32, vk::ClearColorValue{}.setUint32({0,0,0,0})},
		};
		if (!clear_color_values.contains(clear_color_value_type)) {
			throw std::runtime_error{ "unsupported clear color value type" };
		}
		vk::ClearColorValue clear_color_value{ clear_color_values[clear_color_value_type] };

		if (buffers.size() != swapchain_images.size()) {
			throw std::runtime_error{ "swapchain images count != command buffers count" };
		}
		uint32_t index = 0;
		for (uint32_t index = 0; index < buffers.size(); index++) {
			vk::Image swapchain_image = swapchain_images[index];
			vk::Image render_image = render_images[index];
			vk::Image unsampled_image = unsampled_images[index];
			vk::CommandBuffer buffer = buffers[index];

			buffer.begin(
				vk::CommandBufferBeginInfo{}
			);
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{},
				{},
				{},
				vk::ImageMemoryBarrier{}
				.setImage(render_image)
				.setOldLayout(vk::ImageLayout::eUndefined)
				.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			buffer.clearColorImage(
				render_image,
				vk::ImageLayout::eTransferDstOptimal,
				clear_color_value,
				vk::ImageSubresourceRange{}
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setLayerCount(1)
				.setLevelCount(1));
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{}, {}, {},
				vk::ImageMemoryBarrier{}
				.setImage(render_image)
				.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
				.setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{}, {}, {},
				vk::ImageMemoryBarrier{}
				.setImage(unsampled_image)
				.setOldLayout(vk::ImageLayout::eUndefined)
				.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			buffer.resolveImage(render_image, vk::ImageLayout::eTransferSrcOptimal, unsampled_image, vk::ImageLayout::eTransferDstOptimal,
				vk::ImageResolve{}
				.setSrcSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
				.setDstSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
				.setExtent(image_extent)
			);
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{}, {}, {},
				vk::ImageMemoryBarrier{}
				.setImage(unsampled_image)
				.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
				.setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{}, {}, {},
				vk::ImageMemoryBarrier{}
				.setImage(swapchain_image)
				.setOldLayout(vk::ImageLayout::eUndefined)
				.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			vk::Offset3D image_end{ static_cast<int32_t>(image_extent.width), static_cast<int32_t>(image_extent.height), static_cast<int32_t>(image_extent.depth) };
			buffer.blitImage(unsampled_image, vk::ImageLayout::eTransferSrcOptimal, swapchain_image, vk::ImageLayout::eTransferDstOptimal,
				vk::ImageBlit{}
				.setSrcSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
				.setDstSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
				.setSrcOffsets(std::array{ vk::Offset3D{ 0,0,0 }, image_end })
				.setDstOffsets(std::array{ vk::Offset3D{ 0,0,0 }, image_end }),
				vk::Filter::eNearest
			);
			buffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eAllCommands,
				{}, {}, {},
				vk::ImageMemoryBarrier{}
				.setImage(swapchain_image)
				.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
				.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
				.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite)
				.setSrcQueueFamilyIndex(queue_family_index)
				.setDstQueueFamilyIndex(queue_family_index)
				.setSubresourceRange(
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1)
				)
			);
			buffer.end();
		}
	}
	void destroy() {

	}
};

template<class T>
class record_swapchain_command_buffers_cube : public T {
public:
	using parent = T;
	record_swapchain_command_buffers_cube() {
		create();
	}
	void create() {
		auto buffers = parent::get_swapchain_command_buffers();
		auto swapchain_images = parent::get_swapchain_images();
		auto queue_family_index = parent::get_queue_family_index();
		auto framebuffers = parent::get_framebuffers();
		std::vector<vk::Buffer> uniform_buffers = parent::get_uniform_buffer_vector();
		std::vector<vk::Buffer> uniform_upload_buffers = parent::get_uniform_upload_buffer_vector();
		std::vector<vk::DescriptorSet> descriptor_sets = parent::get_descriptor_set();

		auto clear_color_value_type = parent::get_format_clear_color_value_type(parent::get_swapchain_image_format());
		using value_type = decltype(clear_color_value_type);
		std::map<value_type, vk::ClearColorValue> clear_color_values{
			{value_type::eFloat32, vk::ClearColorValue{}.setFloat32({0.4f,0.4f,0.4f,0.0f})},
			{value_type::eUint32, vk::ClearColorValue{}.setUint32({50,50,50,0})},
		};
		if (!clear_color_values.contains(clear_color_value_type)) {
			throw std::runtime_error{ "unsupported clear color value type" };
		}
		vk::ClearColorValue clear_color_value{ clear_color_values[clear_color_value_type] };
		auto clear_depth_value = vk::ClearDepthStencilValue{}.setDepth(1.0f);
		auto clear_values = std::array{
			vk::ClearValue{}.setColor(clear_color_value),
			vk::ClearValue{}.setDepthStencil(clear_depth_value)
		};

		if (buffers.size() != swapchain_images.size()) {
			throw std::runtime_error{ "swapchain images count != command buffers count" };
		}
		uint32_t index = 0;
		for (uint32_t index = 0; index < buffers.size(); index++) {
			vk::Image swapchain_image = swapchain_images[index];
			vk::CommandBuffer cmd = buffers[index];

			cmd.begin(
				vk::CommandBufferBeginInfo{}
			);

			vk::Buffer uniform_buffer = uniform_buffers[index];
			vk::Buffer upload_buffer = uniform_upload_buffers[index];
			cmd.copyBuffer(upload_buffer, uniform_buffer,
				vk::BufferCopy{}.setSize(sizeof(uint64_t)));

			vk::RenderPass render_pass = parent::get_render_pass();

			vk::Extent2D swapchain_image_extent = parent::get_swapchain_image_extent();
			auto render_area = vk::Rect2D{}.setOffset(vk::Offset2D{ 0,0 }).setExtent(swapchain_image_extent);
			vk::Framebuffer framebuffer = framebuffers[index];
			cmd.beginRenderPass(
				vk::RenderPassBeginInfo{}
				.setRenderPass(render_pass)
				.setRenderArea(render_area)
				.setFramebuffer(framebuffer)
				.setClearValues(
					clear_values
				),
				vk::SubpassContents::eInline
			);
			
			vk::Pipeline pipeline = parent::get_pipeline();
			cmd.bindPipeline(
				vk::PipelineBindPoint::eGraphics,
				pipeline
			);
			vk::Buffer vertex_buffer = parent::get_vertex_buffer();
			cmd.bindVertexBuffers(0,
				vertex_buffer, vk::DeviceSize{ 0 });
			vk::Buffer index_buffer = parent::get_index_buffer();
			cmd.bindIndexBuffer(index_buffer, 0, vk::IndexType::eUint16);

			vk::PipelineLayout pipeline_layout = parent::get_pipeline_layout();
			vk::DescriptorSet descriptor_set = descriptor_sets[index];
			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, descriptor_set, {});
			cmd.drawIndexed(3 * 2 * 3 * 2, 1, 0, 0, 0);
			cmd.endRenderPass();
			cmd.end();
		}
	}
	void destroy() {

	}
};
template<class T>
class record_swapchain_command_buffers : public T {
public:
	using parent = T;
	record_swapchain_command_buffers() {
		create();
	}
	void create() {
		auto buffers = parent::get_swapchain_command_buffers();
		auto swapchain_images = parent::get_swapchain_images();
		auto queue_family_index = parent::get_queue_family_index();
		auto framebuffers = parent::get_framebuffers();

		if (buffers.size() != swapchain_images.size()) {
			throw std::runtime_error{ "swapchain images count != command buffers count" };
		}
		uint32_t index = 0;
		for (uint32_t index = 0; index < buffers.size(); index++) {
			vk::Image swapchain_image = swapchain_images[index];
			vk::CommandBuffer buffer = buffers[index];

			buffer.begin(
				vk::CommandBufferBeginInfo{}
			);
			vk::RenderPass render_pass = parent::get_render_pass();

			vk::Extent2D swapchain_image_extent = parent::get_swapchain_image_extent();
			auto render_area = vk::Rect2D{}.setOffset(vk::Offset2D{ 0,0 }).setExtent(swapchain_image_extent);
			vk::Framebuffer framebuffer = framebuffers[index];
			buffer.beginRenderPass(
				vk::RenderPassBeginInfo{}
				.setRenderPass(render_pass)
				.setRenderArea(render_area)
				.setFramebuffer(framebuffer),
				vk::SubpassContents::eInline
			);
			auto clear_color_value_type = parent::get_format_clear_color_value_type(parent::get_swapchain_image_format());
			using value_type = decltype(clear_color_value_type);
			std::map<value_type, vk::ClearColorValue> clear_color_values{
				{value_type::eFloat32, vk::ClearColorValue{}.setFloat32({1.0f,0.0f,0.0f,0.0f})},
				{value_type::eUint32, vk::ClearColorValue{}.setUint32({255,0,0,0})},
			};
			if (!clear_color_values.contains(clear_color_value_type)) {
				throw std::runtime_error{ "unsupported clear color value type" };
			}
			vk::ClearColorValue clear_color_value{ clear_color_values[clear_color_value_type] };
			buffer.clearAttachments(
				vk::ClearAttachment{}.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setClearValue(vk::ClearValue{}.setColor(clear_color_value)),
				vk::ClearRect{}
			.setLayerCount(1)
				.setRect(vk::Rect2D{}
			.setExtent(swapchain_image_extent)));
			vk::Pipeline pipeline = parent::get_pipeline();
			buffer.bindPipeline(
				vk::PipelineBindPoint::eGraphics,
				pipeline
			);
			parent::command_buffer_draw(buffer);
			buffer.endRenderPass();
			buffer.end();
		}
	}
	void destroy() {

	}
};

template<class T>
class add_compute_pipeline : public T {
public:
	using parent = T;
	add_compute_pipeline() {
		create();
	}
	~add_compute_pipeline() {
		destroy();
	}
	void create() {
		vk::Device device = parent::get_device();
		vk::PipelineShaderStageCreateInfo stage = parent::get_shader_stage();

		m_pipeline = device.createComputePipeline(
			nullptr,
			vk::ComputePipelineCreateInfo{}
			.setStage(stage)
		);
	}
	void destroy() {
		vk::Device device = parent::get_device();
		device.destroyPipeline(m_pipeline);
	}
private:
	vk::Pipeline m_pipeline;
};

template<class T>
class add_triangle_vertex_buffer_data : public T {
public:
	auto get_buffer_size() {
		return m_data.size() * sizeof(m_data[0]);
	}
	auto get_buffer_data() {
		return m_data;
	}
private:
	static constexpr auto m_data = std::array{
		-0.5f, -0.5f,
					0.0f, 0.0f,
		0.5f, -0.5f,

	};
};

	template<class T>
	class add_cube_vertex_buffer_data : public T {
	public:
		auto get_buffer_size() {
			return m_data.size() * sizeof(m_data[0]);
		}
		auto get_buffer_data() {
			return m_data;
		}
	private:
		static constexpr auto m_data = std::array{
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
		};
	};
	template<class T>
	class add_cube_index_buffer_data : public T {
	public:
		using parent = T;
		auto get_buffer_size() {
			return m_data.size() * sizeof(m_data[0]);
		}
		auto get_buffer_data() {
			return m_data;
		}
	private:
		static constexpr auto m_data = std::array<uint16_t, 3*3*2*2>{
				0, 1, 3,
				0, 3, 2,
				4, 6, 7,
				4, 7, 5,
				1, 5, 7,
				1, 7, 3,
				3, 7, 6,
				3, 6, 2,
				2, 6, 4,
				2, 4, 0,
				0, 4, 5,
				0, 5, 1,
		};
	};
	template<class T>
	class add_cube_vertex_shader_path : public T {
	public:
		auto get_file_path() {
			return std::filesystem::path{ "cube_vert.spv" };
		}
	};
	template<class T>
	class add_cube_fragment_shader_path : public T {
	public:
		auto get_file_path() {
			return std::filesystem::path{ "cube_frag.spv" };
		}
	};
	template<class T>
	class add_cube_descriptor_set_layout_binding : public T {
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
		auto get_descriptor_set_layout_bindings() {
			return m_binding;
		}
	private:
		vk::DescriptorSetLayoutBinding m_binding;
	};
	template<class T>
	class add_descriptor_pool : public T {
	public:
		using parent = T;
		add_descriptor_pool() {
			create();
		}
		~add_descriptor_pool() {
			destroy();
		}
		void create() {
			vk::Device device = parent::get_device();
			uint32_t count = parent::get_swapchain_images().size();
			auto pool_sizes = vk::DescriptorPoolSize{}
				.setDescriptorCount(count)
				.setType(vk::DescriptorType::eUniformBuffer);
			m_pool = device.createDescriptorPool(
				vk::DescriptorPoolCreateInfo{}
				.setMaxSets(count)
				.setPoolSizes(pool_sizes)
			);
		}
		void destroy() {
			vk::Device device = parent::get_device();

			device.destroyDescriptorPool(m_pool);
		}
		auto get_descriptor_pool() {
			return m_pool;
		}
	private:
		vk::DescriptorPool m_pool;
	};
	template<class T>
	class add_nonfree_descriptor_set : public T {
	public:
		using parent = T;
		add_nonfree_descriptor_set() {
			create();
		}
		~add_nonfree_descriptor_set() {
			destroy();
		}
		void create() {
			vk::Device device = parent::get_device();
			vk::DescriptorPool pool = parent::get_descriptor_pool();
			vk::DescriptorSetLayout layout = parent::get_descriptor_set_layout();
			uint32_t count = parent::get_swapchain_images().size();
			std::vector<vk::DescriptorSetLayout> layouts(count);
			std::ranges::for_each(layouts, [layout](auto& l) {l = layout; });
			m_set = device.allocateDescriptorSets(
				vk::DescriptorSetAllocateInfo{}
				.setDescriptorPool(pool)
				.setDescriptorSetCount(count)
				.setSetLayouts(layouts)
			);
		}
		void destroy() {
		}
		auto get_descriptor_set() {
			return m_set;
		}
	private:
		std::vector<vk::DescriptorSet> m_set;
	};
	template<class T>
	class set_vector_size_to_swapchain_image_count : public T {
	public:
		using parent = T;
		auto get_vector_size() {
			return parent::get_swapchain_images().size();
		}
	};
	template<class T>
	class write_descriptor_set : public T {
	public:
		using parent = T;
		write_descriptor_set() {
			create();
		}
		auto create() {
			vk::Device device = parent::get_device();
			std::vector<vk::Buffer> buffers = parent::get_uniform_buffer_vector();
			std::vector<vk::DescriptorSet> sets = parent::get_descriptor_set();

			std::vector<vk::WriteDescriptorSet> writes(sets.size());
			for (uint32_t i = 0; i < sets.size(); i++) {
				auto buffer = buffers[i];
				auto set = sets[i];
				auto& write = writes[i];

				auto buffer_info = vk::DescriptorBufferInfo{}
					.setBuffer(buffer)
					.setRange(vk::WholeSize);
				write = 
					vk::WriteDescriptorSet{}
					.setDstSet(set)
					.setDescriptorCount(1)
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setDstBinding(0)
					.setBufferInfo(buffer_info);
			}
			device.updateDescriptorSets(writes, {});
		}
	};
	template<class T>
	class add_framebuffers_cube : public T {
	public:
		using parent = T;
		add_framebuffers_cube() {
			create_framebuffers();
		}
		~add_framebuffers_cube() {
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
				auto& framebuffer = m_framebuffers[i];
				auto attachments = std::array{ swapchain_image_view, depth_image_view };

				framebuffer = device.createFramebuffer(
					vk::FramebufferCreateInfo{}
					.setAttachments(attachments)
					.setRenderPass(render_pass)
					.setWidth(width)
					.setHeight(height)
					.setLayers(1)
				);
			}
		}
		void destroy_framebuffers() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(m_framebuffers,
				[device](auto framebuffer) {
					device.destroyFramebuffer(framebuffer);
				});
		}
		auto get_framebuffers() {
			return m_framebuffers;
		}
	private:
		std::vector<vk::Framebuffer> m_framebuffers;
	};
	template<class T>
	class add_depth_images_views_cube : public T {
	public:
		using parent = T;
		add_depth_images_views_cube() {
			create();
		}
		~add_depth_images_views_cube() {
			destroy();
		}
		void create() {
			vk::Device device = parent::get_device();
			auto images = parent::get_images();
			vk::Format format = parent::get_image_format();

			m_views.resize(images.size());
			std::ranges::transform(
				images,
				m_views.begin(),
				[device, format](auto image) {
					return device.createImageView(
						vk::ImageViewCreateInfo{}
						.setImage(image)
						.setFormat(format)
						.setSubresourceRange(
							vk::ImageSubresourceRange{}
							.setAspectMask(vk::ImageAspectFlagBits::eDepth)
							.setLayerCount(1)
							.setLevelCount(1)
						)
						.setViewType(vk::ImageViewType::e2D)
					);
				}
			);
		}
		void destroy() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(
				m_views,
				[device](auto view) {
					device.destroyImageView(view);
				}
			);
		}
		auto get_images_views() {
			return m_views;
		}
	private:
		std::vector<vk::ImageView> m_views;
	};
	template<class T>
	class add_render_pass_cube : public T {
	public:
		using parent = T;
		add_render_pass_cube() {
			vk::Device device = parent::get_device();
			auto attachments = parent::get_attachments();
			auto dependencies = parent::get_subpass_dependencies();
			auto color_attachment = vk::AttachmentReference{}.setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);
			auto depth_attachment = vk::AttachmentReference{}.setAttachment(1).setLayout(vk::ImageLayout::eGeneral);
			auto subpasses = std::array{
				vk::SubpassDescription{}.setColorAttachments(color_attachment).setPDepthStencilAttachment(&depth_attachment)
			};

			m_render_pass = device.createRenderPass(
				vk::RenderPassCreateInfo{}
				.setAttachments(attachments)
				.setDependencies(dependencies)
				.setSubpasses(subpasses)
			);
		}
		~add_render_pass_cube() {
			vk::Device device = parent::get_device();
			device.destroyRenderPass(m_render_pass);
		}
		auto get_render_pass() {
			return m_render_pass;
		}
	private:
		vk::RenderPass m_render_pass;
	};
	template<class T>
	class add_dynamic_draw : public T {
	public:
		using parent = T;
		void draw() {
			vk::Device device = parent::get_device();
			vk::SwapchainKHR swapchain = parent::get_swapchain();
			vk::Queue queue = parent::get_queue();
			vk::Semaphore acquire_image_semaphore = parent::get_acquire_next_image_semaphore();
			bool need_recreate_surface = false;


			auto [res, index] = device.acquireNextImage2KHR(
				vk::AcquireNextImageInfoKHR{}
				.setSwapchain(swapchain)
				.setSemaphore(acquire_image_semaphore)
				.setTimeout(UINT64_MAX)
				.setDeviceMask(1)
			);
			if (res == vk::Result::eSuboptimalKHR) {
				need_recreate_surface = true;
			}
			else if (res != vk::Result::eSuccess) {
				throw std::runtime_error{ "acquire next image != success" };
			}
			parent::free_acquire_next_image_semaphore(index);

			vk::Fence acquire_next_image_semaphore_fence = parent::get_acquire_next_image_semaphore_fence(index);
			{
				vk::Result res = device.waitForFences(acquire_next_image_semaphore_fence, true, UINT64_MAX);
				if (res != vk::Result::eSuccess) {
					throw std::runtime_error{ "failed to wait fences" };
				}
			}
			device.resetFences(acquire_next_image_semaphore_fence);

			std::vector<void*> upload_memory_ptrs = parent::get_uniform_upload_buffer_memory_ptr_vector();
			void* upload_ptr = upload_memory_ptrs[index];
			memcpy(upload_ptr, &m_frame_index, sizeof(m_frame_index));
			m_frame_index++;
			std::vector<vk::DeviceMemory> upload_memory_vector = parent::get_uniform_upload_buffer_memory_vector();
			vk::DeviceMemory upload_memory = upload_memory_vector[index];
			device.flushMappedMemoryRanges(
				vk::MappedMemoryRange{}
				.setMemory(upload_memory)
				.setOffset(0)
				.setSize(vk::WholeSize)
			);

			vk::Semaphore draw_image_semaphore = parent::get_draw_image_semaphore(index);
			vk::CommandBuffer buffer = parent::get_swapchain_command_buffer(index);
			vk::PipelineStageFlags wait_stage_mask{ vk::PipelineStageFlagBits::eTopOfPipe };
			queue.submit(
				vk::SubmitInfo{}
				.setCommandBuffers(buffer)
				.setWaitSemaphores(
					acquire_image_semaphore
				)
				.setWaitDstStageMask(wait_stage_mask)
				.setSignalSemaphores(
					draw_image_semaphore
				),
				acquire_next_image_semaphore_fence
			);
			try {
				auto res = queue.presentKHR(
					vk::PresentInfoKHR{}
					.setImageIndices(index)
					.setSwapchains(swapchain)
					.setWaitSemaphores(draw_image_semaphore)
				);
				if (res == vk::Result::eSuboptimalKHR) {
					need_recreate_surface = true;
				}
				else if (res != vk::Result::eSuccess) {
					throw std::runtime_error{ "present return != success" };
				}
			}
			catch (vk::OutOfDateKHRError e) {
				need_recreate_surface = true;
			}
			if (need_recreate_surface) {
				queue.waitIdle();
				parent::recreate_surface();
			}
		}
		~add_dynamic_draw() {
			vk::Device device = parent::get_device();
			vk::Queue queue = parent::get_queue();
			queue.waitIdle();
		}
	private:
		uint64_t m_frame_index = 0;
	};
