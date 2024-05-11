#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include <vulkan_helper.hpp>
#include <iostream>
#include <numeric>
#include <map>
#include <string>

namespace vulkan_hpp_helper {
	template<class T>
	class add_physical_device : public T {
	public:
		using parent = T;
		add_physical_device() {
			vk::Instance instance = parent::get_instance();
			m_physical_device = instance.enumeratePhysicalDevices()[0];
		}
		auto get_physical_device() {
			return m_physical_device;
		}
	private:
		vk::PhysicalDevice m_physical_device;
	};
	template<class T>
	class add_queue_family_index : public T {
	public:
		uint32_t get_queue_family_index() {
			return 0;
		}
	};
	template<class T>
	class add_device : public T {
	public:
		using parent = T;
		add_device() {
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			auto priorities = std::vector{
				1.0f
			};
			uint32_t queue_family_index = parent::get_queue_family_index();
			auto queue_create_infos = std::vector{
				vk::DeviceQueueCreateInfo{}
				.setQueueCount(priorities.size())
				.setQueuePriorities(priorities)
				.setQueueFamilyIndex(queue_family_index)
			};
			auto exts = parent::get_extensions();
			m_device = physical_device.createDevice(
				vk::DeviceCreateInfo{}
				.setQueueCreateInfos(queue_create_infos)
				.setPEnabledExtensionNames(exts)
			);
		}
		~add_device() {
			m_device.destroy();
		}
		auto get_device() {
			return m_device;
		}
	private:
		vk::Device m_device;
	};
	template<class T>
	class add_queue : public T {
	public:
		using parent = T;
		add_queue() {
			vk::Device device = parent::get_device();
			uint32_t queue_family_index = parent::get_queue_family_index();
			m_queue = device.getQueue(queue_family_index, 0);
		}
		auto get_queue() {
			return m_queue;
		}
	private:
		vk::Queue m_queue;
	};
	template<class T>
	class add_swapchain_image_format : public T {
	public:
		using parent = T;
		add_swapchain_image_format() {
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			vk::SurfaceKHR surface = parent::get_surface();
			std::vector<vk::SurfaceFormatKHR> formats = physical_device.getSurfaceFormatsKHR(surface);
			m_format = vk::Format::eR8G8B8A8Unorm;
		}
		auto get_swapchain_image_format() {
			return m_format;
		}
	private:
		vk::Format m_format;
	};
	template<class T>
	class add_swapchain_image_extent_equal_surface_current_extent : public T {
	public:
		using parent = T;
		auto get_swapchain_image_extent() {
			vk::SurfaceCapabilitiesKHR cap = parent::get_surface_capabilities();
			return cap.currentExtent;
		}
	};
	template<class T>
	class add_recreate_surface_for_swapchain : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_swapchain();
			parent::recreate_surface();
			parent::create_swapchain();
		}
	};
	template<class T>
	class add_swapchain : public T {
	public:
		using parent = T;
		add_swapchain() {
			create_swapchain();
		}
		~add_swapchain() {
			destroy_swapchain();
		}
		void create_swapchain() {
			vk::Device device = parent::get_device();
			vk::SurfaceKHR surface = parent::get_surface();
			vk::Format format = parent::get_swapchain_image_format();
			vk::Extent2D swapchain_image_extent = parent::get_swapchain_image_extent();

			vk::SurfaceCapabilitiesKHR cap = parent::get_surface_capabilities();
			m_swapchain = device.createSwapchainKHR(
				vk::SwapchainCreateInfoKHR{}
				.setMinImageCount(cap.minImageCount)
				.setImageExtent(swapchain_image_extent)
				.setImageFormat(format)
				.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
				.setImageUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment)
				.setImageArrayLayers(1)
				.setSurface(surface)
			);
		}
		void destroy_swapchain() {
			vk::Device device = parent::get_device();
			device.destroySwapchainKHR(m_swapchain);
		}
		auto get_swapchain() {
			return m_swapchain;
		}
	private:
		vk::SwapchainKHR m_swapchain;
	};
	template<class T>
	class add_recreate_surface_for_swapchain_images_views : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_swapchain_images_views();
			parent::recreate_surface();
			parent::create_swapchain_images_views();
		}
	};
	template<class T>
	class add_swapchain_images_views : public T {
	public:
		using parent = T;
		add_swapchain_images_views() {
			create_swapchain_images_views();
		}
		~add_swapchain_images_views() {
			destroy_swapchain_images_views();
		}
		void create_swapchain_images_views() {
			vk::Device device = parent::get_device();
			auto images = parent::get_swapchain_images();
			vk::Format format = parent::get_swapchain_image_format();

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
							.setAspectMask(vk::ImageAspectFlagBits::eColor)
							.setLayerCount(1)
							.setLevelCount(1)
						)
						.setViewType(vk::ImageViewType::e2D)
					);
				}
			);
		}
		void destroy_swapchain_images_views() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(
				m_views,
				[device](auto view) {
					device.destroyImageView(view);
				}
			);
		}
		auto get_swapchain_image_views() {
			return m_views;
		}
	private:
		std::vector<vk::ImageView> m_views;
	};
	template<class T>
	class add_recreate_surface_for_swapchain_images : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::recreate_surface();
			parent::flush_swapchain_images();
		}
	};
	template<class T>
	class add_swapchain_images : public T {
	public:
		using parent = T;
		add_swapchain_images() {
			flush_swapchain_images();
		}
		void flush_swapchain_images() {
			vk::Device device = parent::get_device();
			vk::SwapchainKHR swapchain = parent::get_swapchain();
			m_images = device.getSwapchainImagesKHR(swapchain);
		}
		auto get_swapchain_image(uint32_t index) {
			return m_images[index];
		}
		auto get_swapchain_images() {
			return m_images;
		}
	private:
		std::vector<vk::Image> m_images;
	};
	template<class T>
	class add_image_extent_equal_swapchain_image_extent : public T {
	public:
		using parent = T;
		auto get_image_extent() {
			return vk::Extent3D{ parent::get_swapchain_image_extent(), 1 };
		}
	};
	template<class T>
	class add_image_count_equal_swapchain_image_count : public T {
	public:
		using parent = T;
		auto get_image_count() {
			return parent::get_swapchain_images().size();
		}
	};
	template<vk::Format f, class T>
	class add_image_format : public T {
	public:
		auto get_image_format() {
			return f;
		}
	};
	template<vk::ImageType ImageType, class T>
	class add_image_type : public T {
	public:
		auto get_image_type() {
			return ImageType;
		}
	};
	template<uint32_t ImageCount, class T>
	class add_image_count : public T {
	public:
		auto get_image_count() {
			return ImageCount;
		}
	};
	template<class T>
	class add_empty_image_usages : public T {
	public:
		auto get_image_usages() {
			return vk::ImageUsageFlags{};
		}
	};
	template<vk::ImageUsageFlagBits Usage, class T>
	class add_image_usage : public T {
	public:
		using parent = T;
		auto get_image_usages() {
			vk::ImageUsageFlags usages = parent::get_image_usages();
			return usages | Usage;
		}
	};
	template<vk::SampleCountFlagBits Samples, class T>
	class set_image_samples : public T {
	public:
		using parent = T;
		auto get_image_samples() {
			return Samples;
		}
	};
	template<vk::ImageTiling Tiling, class T>
	class set_image_tiling : public T {
	public:
		using parent = T;
		auto get_image_tiling() {
			return Tiling;
		}
	};
	template<class T>
	class add_recreate_surface_for_images : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_images();
			parent::recreate_surface();
			parent::create_images();
		}
	};
	template<class T>
	class add_images : public T {
	public:
		using parent = T;
		add_images() {
			create_images();
		}
		~add_images() {
			destroy_images();
		}
		void create_images() {
			vk::Device device = parent::get_device();
			vk::Extent3D extent = parent::get_image_extent();
			vk::Format format = parent::get_image_format();
			vk::ImageType image_type = parent::get_image_type();
			uint32_t queue_family_index = parent::get_queue_family_index();
			vk::ImageUsageFlags image_usage = parent::get_image_usages();
			vk::SampleCountFlagBits samples = parent::get_image_samples();
			vk::ImageTiling tiling = parent::get_image_tiling();

			uint32_t image_count = parent::get_image_count();

			m_images.resize(image_count);

			std::ranges::for_each(
				m_images,
				[device, extent, format, image_type, queue_family_index, image_usage, samples, tiling](vk::Image& image) {
					image = device.createImage(
						vk::ImageCreateInfo{}
						.setArrayLayers(1)
						.setExtent(extent)
						.setFormat(format)
						.setImageType(image_type)
						.setInitialLayout(vk::ImageLayout::eUndefined)
						.setMipLevels(1)
						.setQueueFamilyIndices(queue_family_index)
						.setUsage(image_usage)
						.setSamples(samples)
						.setTiling(tiling)
						.setSharingMode(vk::SharingMode::eExclusive)
					);
				}
			);
		}
		void destroy_images() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(
				m_images,
				[device](vk::Image image) {
					device.destroyImage(image);
				}
			);
		}
		auto get_images() {
			return m_images;
		}
	private:
		std::vector<vk::Image> m_images;
	};
	template<class T>
	class rename_images : public T {
	public:
		using parent = T;
		auto get_intermediate_images() {
			return parent::get_images();
		}
	};
	template<class T>
	class add_image_memory : public T {
	public:
		using parent = T;
		add_image_memory() {
			vk::Device device = parent::get_device();
			vk::Image image = parent::get_image();
			vk::MemoryPropertyFlags memory_properties = parent::get_image_memory_properties();

			auto memory_requirements =
				device.getImageMemoryRequirements(
				image
			);
			uint32_t memory_type_index = parent::find_properties(memory_requirements.memoryTypeBits, memory_properties);
			auto memory = device.allocateMemory(
				vk::MemoryAllocateInfo{}
				.setAllocationSize(memory_requirements.size)
				.setMemoryTypeIndex(memory_type_index)
			);
			device.bindImageMemory(image, memory, 0);
			m_memory = memory;
		}
		~add_image_memory() {
			vk::Device device = parent::get_device();
			device.freeMemory(m_memory);
		}
	private:
		vk::DeviceMemory m_memory;
	};
	template<class T>
	class add_buffer_memory : public T {
	public:
		using parent = T;
		add_buffer_memory() {
			vk::Device device = parent::get_device();
			vk::Buffer buffer = parent::get_buffer();
			vk::MemoryPropertyFlags memory_properties = parent::get_buffer_memory_properties();

			auto memory_requirements =
				device.getBufferMemoryRequirements(
					buffer
				);
			uint32_t memory_type_index = parent::find_properties(memory_requirements.memoryTypeBits, memory_properties);
			auto memory = device.allocateMemory(
				vk::MemoryAllocateInfo{}
				.setAllocationSize(memory_requirements.size)
				.setMemoryTypeIndex(memory_type_index)
			);
			device.bindBufferMemory(buffer, memory, 0);
			m_memory = memory;
		}
		~add_buffer_memory() {
			vk::Device device = parent::get_device();
			device.freeMemory(m_memory);
		}
		auto get_buffer_memory() {
			return m_memory;
		}
	private:
		vk::DeviceMemory m_memory;
	};
	template<class T>
	class add_recreate_surface_for_images_memories : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_images_memories();
			parent::recreate_surface();
			parent::create_images_memories();
		}
	};
	template<class T>
	class add_images_memories : public T {
	public:
		using parent = T;
		add_images_memories() {
			create_images_memories();
		}
		~add_images_memories() {
			destroy_images_memories();
		}
		void create_images_memories() {
			vk::Device device = parent::get_device();
			auto images = parent::get_images();
			vk::MemoryPropertyFlags memory_properties = parent::get_image_memory_properties();

			m_memories.resize(images.size());
			std::ranges::transform(images, m_memories.begin(),
				[device, memory_properties, this](vk::Image image) {
					auto memory_requirements = device.getImageMemoryRequirements(
						image
					);
					uint32_t memory_type_index = parent::find_properties(memory_requirements.memoryTypeBits, memory_properties);
					auto memory = device.allocateMemory(
						vk::MemoryAllocateInfo{}
						.setAllocationSize(memory_requirements.size)
						.setMemoryTypeIndex(memory_type_index)
					);
					device.bindImageMemory(image, memory, 0);
					return memory;
				});
		}
		void destroy_images_memories() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(m_memories,
				[device](vk::DeviceMemory memory) {
					device.freeMemory(memory);
				});
		}
		auto get_images_memories() {
			return m_memories;
		}
	private:
		std::vector<vk::DeviceMemory> m_memories;
	};
	template<vk::MemoryPropertyFlagBits Property, class T>
	class add_image_memory_property : public T {
	public:
		using parent = T;
		auto get_image_memory_properties() {
			auto properties = parent::get_image_memory_properties();
			return properties | Property;
		}
	};
	template<class T>
	class add_empty_image_memory_properties : public T {
	public:
		auto get_image_memory_properties() {
			return vk::MemoryPropertyFlagBits{};
		}
	};
	template<class T>
	class cache_physical_device_memory_properties : public T {
	public:
		using parent = T;
		cache_physical_device_memory_properties() {
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			m_properties = physical_device.getMemoryProperties();
		}
		auto get_physical_device_memory_properties() {
			return m_properties;
		}
	private:
		vk::PhysicalDeviceMemoryProperties m_properties;
	};
	template<class T>
	class add_find_properties : public T {
	public:
		using parent = T;
		uint32_t find_properties(uint32_t memory_type_bits_requirements, vk::MemoryPropertyFlags required_property) {
			vk::PhysicalDeviceMemoryProperties memory_properties = parent::get_physical_device_memory_properties();
			const uint32_t memory_count = memory_properties.memoryTypeCount;
			for (uint32_t memoryIndex = 0; memoryIndex < memory_count; memoryIndex++) {
				const uint32_t memoryTypeBits = (1 << memoryIndex);
				const bool is_required_memory_type = memory_type_bits_requirements & memoryTypeBits;
				const vk::MemoryPropertyFlags properties =
					memory_properties.memoryTypes[memoryIndex].propertyFlags;
				const bool has_required_properties =
					(properties & required_property) == required_property;
				if (is_required_memory_type && has_required_properties)
					return memoryIndex;
			}
			throw std::runtime_error{ "failed find memory property" };
		}
	};
	template<class T>
	class add_command_pool : public T {
	public:
		using parent = T;
		add_command_pool() {
			vk::Device device = parent::get_device();
			uint32_t queue_family_index = parent::get_queue_family_index();
			m_pool = device.createCommandPool(
				vk::CommandPoolCreateInfo{}
				.setQueueFamilyIndex(queue_family_index)
			);
		}
		~add_command_pool() {
			vk::Device device = parent::get_device();
			device.destroyCommandPool(m_pool);
		}
		auto get_command_pool() {
			return m_pool;
		}
	private:
		vk::CommandPool m_pool;
	};
	template<class T>
	class add_command_buffer : public T {
	public:
		using parent = T;
		add_command_buffer() {
			vk::Device device = parent::get_device();
			vk::CommandPool pool = parent::get_command_pool();

			m_buffer = device.allocateCommandBuffers(
				vk::CommandBufferAllocateInfo{}
				.setCommandPool(pool)
				.setCommandBufferCount(1)
			)[0];
		}
		~add_command_buffer() {
			vk::Device device = parent::get_device();
			vk::CommandPool pool = parent::get_command_pool();
			device.freeCommandBuffers(pool, m_buffer);
		}
		auto get_command_buffer() {
			return m_buffer;
		}
	private:
		vk::CommandBuffer m_buffer;
	};
	template<class T>
	class add_recreate_surface_for : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy();
			parent::recreate_surface();
			parent::create();
		}
	};
	template<class T>
	class add_swapchain_command_buffers : public T {
	public:
		using parent = T;
		add_swapchain_command_buffers() {
			create();
		}
		~add_swapchain_command_buffers() {
			destroy();
		}
		void create() {
			vk::Device device = parent::get_device();
			vk::CommandPool pool = parent::get_command_pool();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();
			m_buffers = device.allocateCommandBuffers(
				vk::CommandBufferAllocateInfo{}
				.setCommandPool(pool)
				.setCommandBufferCount(swapchain_image_count)
			);
		}
		void destroy() {
			vk::Device device = parent::get_device();
			vk::CommandPool pool = parent::get_command_pool();
			device.freeCommandBuffers(pool, m_buffers);
		}
		auto get_swapchain_command_buffers() {
			return m_buffers;
		}
		auto get_swapchain_command_buffer(uint32_t i) {
			return m_buffers[i];
		}
	private:
		std::vector<vk::CommandBuffer> m_buffers;
	};
	template<class T>
	class add_get_format_clear_color_value_type : public T {
	public:
		enum class clear_color_value_type {
			eInt32,
			eUint32,
			eFloat32,
		};
		clear_color_value_type get_format_clear_color_value_type(vk::Format f) {
			std::map<vk::Format, clear_color_value_type> types{
				{vk::Format::eR8G8B8A8Unorm, clear_color_value_type::eFloat32},
				{vk::Format::eR32G32B32A32Sfloat, clear_color_value_type::eFloat32},
				{vk::Format::eR8G8B8A8Srgb, clear_color_value_type::eUint32},
				{vk::Format::eR32G32B32A32Uint, clear_color_value_type::eUint32},
			};
			if (!types.contains(f)) {
				throw std::runtime_error{ "this format does not support clear color value" };
			}
			return types[f];
		}
	};
	template<class T>
	class add_recreate_surface_for_framebuffers : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_framebuffers();
			parent::recreate_surface();
			parent::create_framebuffers();
		}
	};
	template<class T>
	class add_framebuffers : public T {
	public:
		using parent = T;
		add_framebuffers() {
			create_framebuffers();
		}
		~add_framebuffers() {
			destroy_framebuffers();
		}
		void create_framebuffers() {
			vk::Device device = parent::get_device();
			vk::RenderPass render_pass = parent::get_render_pass();
			auto extent = parent::get_swapchain_image_extent();
			uint32_t width = extent.width;
			uint32_t height = extent.height;
			auto swapchain_image_views = parent::get_swapchain_image_views();
			m_framebuffers.resize(swapchain_image_views.size());
			std::ranges::transform(swapchain_image_views, m_framebuffers.begin(),
				[device, render_pass, extent, width, height](auto& image_view) {
					return device.createFramebuffer(
						vk::FramebufferCreateInfo{}
						.setAttachments(image_view)
						.setRenderPass(render_pass)
						.setWidth(width)
						.setHeight(height)
						.setLayers(1)
					);
				});
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
			auto render_images = parent::get_intermediate_images();
			auto unsampled_images = parent::get_images();
			auto image_extent = parent::get_image_extent();
			auto framebuffers = parent::get_framebuffers();

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
					vk::PipelineStageFlagBits::eTopOfPipe,
					vk::PipelineStageFlagBits::eTransfer,
					{},
					{},
					{},
					vk::ImageMemoryBarrier{}
					.setImage(render_image)
					.setOldLayout(vk::ImageLayout::eUndefined)
					.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
					.setSrcAccessMask(vk::AccessFlagBits::eNone)
					.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
					vk::PipelineStageFlagBits::eTransfer,
					vk::PipelineStageFlagBits::eTransfer,
					{}, {}, {},
					vk::ImageMemoryBarrier{}
					.setImage(render_image)
					.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
					.setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
					.setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
					.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
					vk::PipelineStageFlagBits::eTopOfPipe,
					vk::PipelineStageFlagBits::eTransfer,
					{}, {}, {},
					vk::ImageMemoryBarrier{}
					.setImage(unsampled_image)
					.setOldLayout(vk::ImageLayout::eUndefined)
					.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
					.setSrcAccessMask(vk::AccessFlagBits::eNone)
					.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
					vk::PipelineStageFlagBits::eTransfer,
					vk::PipelineStageFlagBits::eTransfer,
					{}, {}, {},
					vk::ImageMemoryBarrier{}
					.setImage(unsampled_image)
					.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
					.setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
					.setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
					.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
					vk::PipelineStageFlagBits::eTopOfPipe,
					vk::PipelineStageFlagBits::eTransfer,
					{}, {}, {},
					vk::ImageMemoryBarrier{}
					.setImage(swapchain_image)
					.setOldLayout(vk::ImageLayout::eUndefined)
					.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
					.setSrcAccessMask(vk::AccessFlagBits::eNone)
					.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
				/*buffer.blitImage(unsampled_image, vk::ImageLayout::eTransferSrcOptimal, swapchain_image, vk::ImageLayout::eTransferDstOptimal,
					vk::ImageBlit{}
					.setSrcSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
					.setDstSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
					.setSrcOffsets(std::array { vk::Offset3D{ 0,0,0 }, image_end})
					.setDstOffsets(std::array{ vk::Offset3D{ 0,0,0 }, image_end}),
					vk::Filter::eNearest
				);*/
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
				/*buffer.clearAttachments(
					vk::ClearAttachment{}.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setClearValue(vk::ClearValue{}.setColor(clear_color_value)),
					vk::ClearRect{}
				.setLayerCount(1)
					.setRect(vk::Rect2D{}
				.setExtent(swapchain_image_extent)));*/
				vk::Pipeline pipeline = parent::get_pipeline();
				buffer.bindPipeline(
					vk::PipelineBindPoint::eGraphics,
					pipeline
				);
				vk::Buffer vertex_buffer = parent::get_vertex_buffer();
				buffer.bindVertexBuffers(0,
					vertex_buffer, vk::DeviceSize{ 0 });
				buffer.draw(3, 1, 0, 0);
				buffer.endRenderPass();
				buffer.end();
			}
		}
		void destroy() {

		}
	};
	template<class T>
	class rename_vertex_buffer_to_buffer : public T {
	public:
		using parent = T;
		auto get_buffer() {
			return parent::get_vertex_buffer();
		}
	};
	template<class T>
	class set_buffer_memory_properties : public T {
	public:
		using parent = T;
		auto get_buffer_memory_properties() {
			return vk::MemoryPropertyFlagBits::eHostVisible;
		}
	};
	template<class T>
	class rename_vertex_buffer_data_to_buffer_data : public T {
	public:
		using parent = T;
		auto get_buffer_data() {
			return parent::get_vertex_buffer_data();
		}
	};
	template<class T>
	class copy_buffer_data : public T {
	public:
		using parent = T;
		copy_buffer_data() {
			vk::Device device = parent::get_device();
			vk::DeviceMemory memory = parent::get_buffer_memory();
			auto data = parent::get_buffer_data();
			void* ptr = device.mapMemory(memory, 0, vk::WholeSize);
			memcpy(ptr, data.data(), data.size() * sizeof(data[0]));
			device.unmapMemory(memory);
		}
	};
	template<class T>
	class add_vertex_buffer_memory :
		public
		copy_buffer_data<
		rename_vertex_buffer_data_to_buffer_data<
		add_buffer_memory<
		rename_vertex_buffer_to_buffer<
		set_buffer_memory_properties<
		T>>>>>
	{
	};
	template<class T>
	class add_vertex_buffer_data : public T {
	public:
		auto get_vertex_buffer_size() {
			return m_data.size() * sizeof(m_data[0]);
		}
		auto get_vertex_buffer_data() {
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
	class add_vertex_buffer : public T {
	public:
		using parent = T;
		add_vertex_buffer() {
			vk::Device device = parent::get_device();
			uint32_t queue_family_index = parent::get_queue_family_index();
			vk::DeviceSize size = parent::get_vertex_buffer_size();
			m_buffer = device.createBuffer(
				vk::BufferCreateInfo{}
				.setQueueFamilyIndices(queue_family_index)
				.setSize(size)
				.setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
			);
		}
		~add_vertex_buffer() {
			vk::Device device = parent::get_device();
			device.destroyBuffer(m_buffer);
		}
		auto get_vertex_buffer() {
			return m_buffer;
		}
	private:
		vk::Buffer m_buffer;
	};
	template<class T>
	class jump_draw_if_window_minimized : public T {
	public:
		using parent = T;
		void draw() {
			if (!parent::is_window_minimized()) {
				parent::draw();
			}
		}
	};
	template<class T>
	class add_draw : public T{
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
				vk::Result res= device.waitForFences(acquire_next_image_semaphore_fence, true, UINT64_MAX);
				if (res != vk::Result::eSuccess) {
					throw std::runtime_error{ "failed to wait fences" };
				}
			}
			device.resetFences(acquire_next_image_semaphore_fence);

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
			{
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
			if (need_recreate_surface) {
				queue.waitIdle();
				parent::recreate_surface();
			}
		}
		~add_draw() {
			vk::Device device = parent::get_device();
			vk::Queue queue = parent::get_queue();
			queue.waitIdle();
		}
	};
	template<class T>
	class add_acquire_next_image_fences : public T {
	public:
		using parent = T;
		add_acquire_next_image_fences() {
			vk::Device device = parent::get_device();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();

			m_fences.resize(swapchain_image_count + 1);
			std::ranges::for_each(m_fences,
				[device](vk::Fence& fence) {
					fence = device.createFence(
						vk::FenceCreateInfo{}
					); }
			);
		}
		~add_acquire_next_image_fences() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(m_fences,
				[device](vk::Fence fence) {
					device.destroyFence(fence);
				});
		}
		auto get_acquire_next_image_fences() {
			return m_fences;
		}
	private:
		std::vector<vk::Fence> m_fences;
	};
	template<class T>
	class add_draw_semaphores : public T{
	public:
		using parent = T;
		add_draw_semaphores() {
			vk::Device device = parent::get_device();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();
			
			m_semaphores.resize(swapchain_image_count);
			std::ranges::for_each(m_semaphores,
				[device](vk::Semaphore& semaphore) {
					semaphore = device.createSemaphore(
						vk::SemaphoreCreateInfo{}
					);
				});
		}
		~add_draw_semaphores() {
			vk::Device device = parent::get_device();

			std::ranges::for_each(m_semaphores,
				[device](vk::Semaphore semaphore) {
					device.destroySemaphore(semaphore);
				});
		}
		auto get_draw_image_semaphore(uint32_t i) {
			return m_semaphores[i];
		}
	private:
		std::vector<vk::Semaphore> m_semaphores;
	};
	template<class T>
	class add_acquire_next_image_semaphores : public T{
	public:
		using parent = T;
		add_acquire_next_image_semaphores() {
			vk::Device device = parent::get_device();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();

			m_semaphores.resize(swapchain_image_count+1);
			std::ranges::for_each(m_semaphores,
				[device](vk::Semaphore& semaphore) {
					semaphore = device.createSemaphore(
						vk::SemaphoreCreateInfo{}
					);
				});

			m_semaphore_indices.resize(swapchain_image_count);
			std::ranges::iota(m_semaphore_indices, 0);
			m_free_semaphore_index = swapchain_image_count;
		}
		~add_acquire_next_image_semaphores() {
			vk::Device device = parent::get_device();

			std::ranges::for_each(m_semaphores,
				[device](vk::Semaphore semaphore) {
					device.destroySemaphore(semaphore);
				});
		}
		auto get_acquire_next_image_semaphore() {
			return m_semaphores[m_free_semaphore_index];
		}
		auto free_acquire_next_image_semaphore(uint32_t image_index) {
			vk::Device device = parent::get_device();


			std::swap(m_free_semaphore_index,m_semaphore_indices[image_index]);
		}
	private:
		std::vector<vk::Semaphore> m_semaphores;
		uint32_t m_free_semaphore_index;
		std::vector<uint32_t> m_semaphore_indices;
	};
	template<class T>
	class add_acquire_next_image_semaphore_fences : public T {
	public:
		using parent = T;
		add_acquire_next_image_semaphore_fences() {
			vk::Device device = parent::get_device();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();

			m_fences.resize(swapchain_image_count);
			std::ranges::for_each(m_fences,
				[device](vk::Fence& fence) {
					fence = device.createFence(
						vk::FenceCreateInfo{}
						.setFlags(vk::FenceCreateFlagBits::eSignaled)
					); }
			);
		}
		~add_acquire_next_image_semaphore_fences() {
			vk::Device device = parent::get_device();
			std::ranges::for_each(m_fences,
				[device](vk::Fence fence) {
					device.destroyFence(fence);
				});
		}
		auto get_acquire_next_image_semaphore_fence(uint32_t image_index) {
			return m_fences[image_index];
		}
	private:
		std::vector<vk::Fence> m_fences;
	};
	template<class T>
	class add_recreate_surface_for_pipeline : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_pipeline();
			parent::recreate_surface();
			parent::create_pipeline();
		}
	};
	template<class T>
	class add_graphics_pipeline : public T {
	public:
		using parent = T;
		add_graphics_pipeline() {
			create_pipeline();
		}
		~add_graphics_pipeline() {
			destroy_pipeline();
		}
		void create_pipeline() {
			vk::Device device = parent::get_device();
			vk::PipelineLayout pipeline_layout = parent::get_pipeline_layout();
			vk::PipelineColorBlendStateCreateInfo color_blend_state = parent::get_pipeline_color_blend_state_create_info();
			vk::PipelineDepthStencilStateCreateInfo depth_stencil_state = parent::get_pipeline_depth_stencil_state_create_info();
			vk::PipelineDynamicStateCreateInfo dynamic_state = parent::get_pipeline_dynamic_state_create_info();
			vk::PipelineInputAssemblyStateCreateInfo input_assembly_state = parent::get_pipeline_input_assembly_state_create_info();
			vk::PipelineMultisampleStateCreateInfo multisample_state =
				parent::get_pipeline_multisample_state_create_info();
			vk::PipelineRasterizationStateCreateInfo rasterization_state =
				parent::get_pipeline_rasterization_state_create_info();
			auto stages =
				parent::get_pipeline_stages();
			vk::PipelineTessellationStateCreateInfo tessellation_state =
				parent::get_pipeline_tessellation_state_create_info();
			vk::PipelineVertexInputStateCreateInfo vertex_input_state =
				parent::get_pipeline_vertex_input_state_create_info();
			vk::PipelineViewportStateCreateInfo viewport_state =
				parent::get_pipeline_viewport_state_create_info();
			vk::RenderPass render_pass =
				parent::get_render_pass();
			uint32_t subpass = parent::get_subpass();

			auto [res, pipeline] = device.createGraphicsPipeline(
				{},
				vk::GraphicsPipelineCreateInfo{}
				.setLayout(pipeline_layout)
				.setPColorBlendState(&color_blend_state)
				.setPDepthStencilState(&depth_stencil_state)
				.setPDynamicState(&dynamic_state)
				.setPInputAssemblyState(&input_assembly_state)
				.setPMultisampleState(&multisample_state)
				.setPRasterizationState(&rasterization_state)
				.setStages(stages)
				.setPTessellationState(&tessellation_state)
				.setPVertexInputState(&vertex_input_state)
				.setPViewportState(&viewport_state)
				.setRenderPass(render_pass)
				.setSubpass(subpass)

			);
			if (res != vk::Result::eSuccess) {
				throw std::runtime_error{ "failed to create graphics pipeline" };
			}
			m_pipeline = pipeline;
		}
		void destroy_pipeline() {
			vk::Device device = parent::get_device();
			device.destroyPipeline(m_pipeline);
		}
		auto get_pipeline() {
			return m_pipeline;
		}
	private:
		vk::Pipeline m_pipeline;
	};
	template<uint32_t Subpass, class T>
	class set_subpass : public T {
	public:
		auto get_subpass() {
			return Subpass;
		}
	};
	template<class T>
	class add_empty_attachments : public T {
	public:
		auto get_attachments() {
			return std::vector<vk::AttachmentDescription>{};
		}
	};
	template<class T>
	class add_attachment : public T {
	public:
		using parent = T;
		auto get_attachments() {
			auto attachments = parent::get_attachments();
			vk::Format format = parent::get_swapchain_image_format();
			vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined;
			vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR;
			attachments.emplace_back(
				vk::AttachmentDescription{}
				.setInitialLayout(initial_layout)
				.setFinalLayout(final_layout)
				.setFormat(format)
				.setLoadOp(vk::AttachmentLoadOp::eDontCare)
				.setStoreOp(vk::AttachmentStoreOp::eStore)
				.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
				.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			);
			return attachments;
		}
	};
	template<class T>
	class add_subpass_dependency : public T {
	public:
		using parent = T;
		auto get_subpass_dependencies() {
			auto depends = parent::get_subpass_dependencies();
			depends.emplace_back(
				vk::SubpassDependency{}
				.setSrcSubpass(vk::SubpassExternal)
				.setDstSubpass(0)
				.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
				.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead)
				.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
				.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			);
			return depends;
		}
	};
	template<class T>
	class add_empty_subpass_dependencies : public T {
	public:
		auto get_subpass_dependencies() {
			return std::vector<vk::SubpassDependency>{};
		}
	};
	template<class T>
	class add_subpasses : public T {
	public:
		auto get_subpasses() {
			return vk::SubpassDescription{}
			.setColorAttachments(
				vk::AttachmentReference{}
				.setAttachment(0)
				.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
			);
		}
	};
	template<class T>
	class add_render_pass : public T {
	public:
		using parent = T;
		add_render_pass() {
			vk::Device device = parent::get_device();
			auto attachments = parent::get_attachments();
			auto dependencies = parent::get_subpass_dependencies();
			auto subpasses = parent::get_subpasses();

			m_render_pass = device.createRenderPass(
				vk::RenderPassCreateInfo{}
				.setAttachments(attachments)
				.setDependencies(dependencies)
				.setSubpasses(subpasses)
			);
		}
		~add_render_pass() {
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
	class add_pipeline_viewport_state : public T {
	public:
		using parent = T;
		auto get_pipeline_viewport_state_create_info() {
			m_viewports = parent::get_viewports();
			m_scissors = parent::get_scissors();
			if (m_viewports.size() != m_scissors.size()) {
				throw std::runtime_error{ "viewports count != scissors count" };
			}
			return vk::PipelineViewportStateCreateInfo{}
				.setViewports(m_viewports)
				.setScissors(m_scissors);
		}
	private:
		std::vector<vk::Viewport> m_viewports;
		std::vector<vk::Rect2D> m_scissors;
	};
	template<class T>
	class add_scissor_equal_surface_rect : public T {
	public:
		using parent = T;
		auto get_scissors() {
			vk::SurfaceCapabilitiesKHR surface_cap = parent::get_surface_capabilities();
			auto scissors = parent::get_scissors();

			scissors.emplace_back(
				vk::Rect2D{}
				.setOffset({})
				.setExtent(surface_cap.currentExtent)
			);
			return scissors;
		}
	};
	template<class T>
	class add_recreate_surface_for_cache_surface_capabilites : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::recreate_surface();
			parent::flush_surface_capabilities_cache();
		}
	};
	template<class T>
	class cache_surface_capabilities : public T {
	public:
		using parent = T;
		cache_surface_capabilities() {
			flush_surface_capabilities_cache();
		}
		void flush_surface_capabilities_cache() {
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			vk::SurfaceKHR surface = parent::get_surface();
			m_capabilities = physical_device.getSurfaceCapabilitiesKHR(surface);
		}
		auto get_surface_capabilities() {
			return m_capabilities;
		}
	private:
		vk::SurfaceCapabilitiesKHR m_capabilities;
	};
	template<class T>
	class test_physical_device_support_surface : public T {
	public:
		using parent = T;
		test_physical_device_support_surface() {
			create();
		}
		void create() {
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			vk::SurfaceKHR surface = parent::get_surface();
			uint32_t queue_family_index = parent::get_queue_family_index();
			m_support = physical_device.getSurfaceSupportKHR(queue_family_index, surface);
		}
		void destroy() {

		}
		auto get_physical_device_support_surface() {
			return m_support;
		}
	private:
		bool m_support;
	};
	template<class T>
	class add_empty_scissors : public T {
	public:
		auto get_scissors() {
			return std::vector<vk::Rect2D>{};
		}
	};
	template<class T>
	class add_viewport : public T {
	public:
		using parent = T;
		auto get_viewports() {
			auto viewports = parent::get_viewports();
			float x = parent::get_viewport_x();
			float y = parent::get_viewport_y();
			float width = parent::get_viewport_width();
			float height = parent::get_viewport_height();
			float min_depth = parent::get_viewport_min_depth();
			float max_depth = parent::get_viewport_max_depth();
			viewports.emplace_back(
				vk::Viewport{}
				.setX(x)
				.setY(y)
				.setWidth(width)
				.setHeight(height)
				.setMinDepth(min_depth)
				.setMaxDepth(max_depth)
			);
			return viewports;
		}
	};
	template<class T>
	class add_viewport_equal_swapchain_image_rect : public T {
	public:
		using parent = T;
		auto get_viewports() {
			auto viewports = parent::get_viewports();
			auto extent = parent::get_swapchain_image_extent();
			viewports.emplace_back(
				vk::Viewport{}
				.setWidth(extent.width)
				.setHeight(extent.height)
				.setMinDepth(0)
				.setMaxDepth(1)
			);
			return viewports;
		}
	};
	template<float X, class T>
	class set_viewport_x : public T {
	public:
		auto get_viewport_x() {
			return X;
		}
	};
	template<float Y, class T>
	class set_viewport_y : public T {
	public:
		auto get_viewport_y() {
			return Y;
		}
	};
	template<float Width, class T>
	class set_viewport_width : public T {
	public:
		auto get_viewport_width() {
			return Width;
		}
	};
	template<float Height, class T>
	class set_viewport_height : public T {
	public:
		auto get_viewport_height() {
			return Height;
		}
	};
	template<float Min_depth, class T>
	class set_viewport_min_depth : public T {
	public:
		auto get_viewport_min_depth() {
			return Min_depth;
		}
	};
	template<float Max_depth, class T>
	class set_viewport_max_depth : public T {
	public:
		auto get_viewport_max_depth() {
			return Max_depth;
		}
	};
	template<class T>
	class add_empty_viewports : public T {
	public:
		auto get_viewports() {
			return std::vector<vk::Viewport>{};
		}
	};
	template<class T>
	class add_pipeline_vertex_input_state : public T {
	public:
		using parent = T;
		auto get_pipeline_vertex_input_state_create_info() {
			m_attribute_descriptions = parent::get_vertex_attribute_descriptions();
			m_binding_descriptions = parent::get_vertex_binding_descriptions();
			return vk::PipelineVertexInputStateCreateInfo{}
				.setVertexAttributeDescriptions(m_attribute_descriptions)
				.setVertexBindingDescriptions(m_binding_descriptions);
		}
	private:
		std::vector<vk::VertexInputAttributeDescription> m_attribute_descriptions;
		std::vector<vk::VertexInputBindingDescription> m_binding_descriptions;
	};
	template<class T>
	class add_vertex_attribute_description : public T {
	public:
		using parent = T;
		auto get_vertex_attribute_descriptions() {
			auto descs = parent::get_vertex_attribute_descriptions();
			descs.emplace_back(
				vk::VertexInputAttributeDescription{}
				.setLocation(0)
				.setBinding(0)
				.setOffset(0)
				.setFormat(vk::Format::eR32G32Sfloat)
			);
			return descs;
		}
	};
	template<class T>
	class add_empty_vertex_attribute_descriptions : public T {
	public:
		auto get_vertex_attribute_descriptions() {
			return std::vector<vk::VertexInputAttributeDescription>{};
		}
	};
	template<vk::VertexInputRate InputRate, class T>
	class set_input_rate : public T {
	public:
		auto get_input_rate() {
			return InputRate;
		}
	};
	template<uint32_t Binding, class T>
	class set_binding : public T {
	public:
		auto get_binding() {
			return Binding;
		}
	};
	template<uint32_t Stride, class T>
	class set_stride : public T {
	public:
		auto get_stride() {
			return Stride;
		}
	};
	template<class T>
	class add_vertex_binding_description : public T {
	public:
		using parent = T;
		auto get_vertex_binding_descriptions() {
			auto descs = parent::get_binding_descriptions();
			uint32_t binding = parent::get_binding();
			vk::VertexInputRate input_rate = parent::get_input_rate();
			uint32_t stride = parent::get_stride();
			descs.emplace_back(
				vk::VertexInputBindingDescription{}
				.setBinding(binding)
				.setInputRate(input_rate)
				.setStride(stride)
			);
			return descs;
		}
	};
	template<class T>
	class add_empty_binding_descriptions : public T {
	public:
		auto get_binding_descriptions() {
			return std::vector<vk::VertexInputBindingDescription>{};
		}
	};
	template<uint32_t Count, class T>
	class set_tessellation_patch_control_point_count : public T {
	public:
		auto get_pipeline_tessellation_state_create_info() {
			return vk::PipelineTessellationStateCreateInfo{}
			.setPatchControlPoints(Count);
		}
	};
	template< class T>
	class add_pipeline_stage : public T {
	public:
		using parent = T;
		auto get_pipeline_stages() {
			auto stages = parent::get_pipeline_stages();
			vk::ShaderModule shader_module = parent::get_shader_module();
			m_entry_name = parent::get_shader_entry_name();
			vk::ShaderStageFlagBits stage = parent::get_shader_stage();
			stages.emplace_back(vk::PipelineShaderStageCreateInfo{}
				.setModule(shader_module)
				.setPName(m_entry_name.data())
				.setStage(stage));
			return stages;
		}
	private:
		std::string m_entry_name;
	};
	template<vk::ShaderStageFlagBits Shader_stage, class T>
	class set_shader_stage : public T {
	public:
		auto get_shader_stage() {
			return Shader_stage;
		}
	};
	template<class T>
	class set_shader_entry_name_with_main : public T {
	public:
		auto get_shader_entry_name() {
			return std::string{ "main" };
		}
	};
	template<class T>
	class add_shader_module : public T {
	public:
		using parent = T;
		add_shader_module() {
			vk::Device device = parent::get_device();
			auto code = parent::get_spirv_code();
			m_module = device.createShaderModule(
				vk::ShaderModuleCreateInfo{}
				.setCode(code)
			);
		}
		~add_shader_module() {
			vk::Device device = parent::get_device();
			device.destroyShaderModule(m_module);
		}
		auto get_shader_module() {
			return m_module;
		}
	private:
		vk::ShaderModule m_module;
	};
	template<class T>
	class add_spirv_code : public T {
	public:
		using parent = T;
		auto get_spirv_code() {
			void* ptr = parent::get_code_pointer();
			std::uint32_t size = parent::get_size_in_bytes();
			return std::span{ reinterpret_cast<uint32_t*>(ptr), size/4 };
		}
	};
	template<class T>
	class adapte_map_file_to_spirv_code : public T {
	public:
		using parent = T;
		auto get_code_pointer() {
			return parent::get_mapped_pointer();
		}
		auto get_size_in_bytes() {
			return parent::get_file_size();
		}
	};
	template<class T>
	class map_file_mapping : public T {
	public:
		using parent = T;
		map_file_mapping() {
			HANDLE mapping = parent::get_file_mapping();
			m_memory = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
			if (m_memory == INVALID_HANDLE_VALUE) {
				throw std::runtime_error{ "failed to map view of file" };
			}
		}
		~map_file_mapping() {
			UnmapViewOfFile(m_memory);
		}
		auto get_mapped_pointer() {
			return m_memory;
		}
	private:
		void* m_memory;
	};
	template<class T>
	class cache_file_size : public T {
	public:
		using parent = T;
		cache_file_size() {
			HANDLE file = parent::get_file();
			m_size = GetFileSize(file, NULL);
		}
		auto get_file_size() {
			return m_size;
		}
	private:
		uint32_t m_size;
	};
	template<class T>
	class add_file_mapping : public T {
	public:
		using parent = T;
		add_file_mapping() {
			uint64_t maximum_size{ 0 };
			HANDLE file = parent::get_file();
			m_mapping = CreateFileMapping(file, nullptr, PAGE_READONLY,
				static_cast<uint32_t>(maximum_size >> 32), static_cast<uint32_t>(maximum_size),
				nullptr);
			if (m_mapping == INVALID_HANDLE_VALUE) {
				throw std::runtime_error{ "failed to create file mapping" };
			}
		}
		~add_file_mapping() {
			CloseHandle(m_mapping);
		}
		auto get_file_mapping() {
			return m_mapping;
		}
	private:
		HANDLE m_mapping;
	};
	template<class T>
	class add_file : public T {
	public:
		using parent = T;
		add_file() {
			auto path = parent::get_file_path();
			m_file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_file == NULL) {
				throw std::runtime_error{ "failed to create file" };
			}
		}
		~add_file() {
			CloseHandle(m_file);
		}
		auto get_file() {
			return m_file;
		}
	private:
		HANDLE m_file;
	};
	template<class T>
	class add_vertex_shader_path : public T {
	public:
		auto get_file_path() {
			return std::filesystem::path{ "vert.spv" };
		}
	};
	template<class T>
	class add_fragment_shader_path : public T {
	public:
		auto get_file_path() {
			return std::filesystem::path{ "frag.spv" };
		}
	};
	template<class T>
	class add_empty_pipeline_stages : public T {
	public:
		auto get_pipeline_stages() {
			return std::vector<vk::PipelineShaderStageCreateInfo>{};
		}
	};
	template<vk::PolygonMode Polygon_mode, class T>
	class set_pipeline_rasterization_polygon_mode : public T {
	public:
		auto get_pipeline_rasterization_state_create_info() {
			return vk::PipelineRasterizationStateCreateInfo{}
			.setPolygonMode(Polygon_mode)
				.setLineWidth(1.0f)
				.setCullMode(vk::CullModeFlagBits::eNone);
		}
	};
	template<class T>
	class disable_pipeline_multisample : public T {
	public:
		auto get_pipeline_multisample_state_create_info() {
			return vk::PipelineMultisampleStateCreateInfo{}
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		}
	};
	template<vk::PrimitiveTopology Topology, class T>
	class set_pipeline_input_topology : public T {
	public:
		auto get_pipeline_input_assembly_state_create_info() {
			return vk::PipelineInputAssemblyStateCreateInfo{}
			.setTopology(Topology);
		}
	};
	template<class T>
	class disable_pipeline_dynamic : public T {
	public:
		auto get_pipeline_dynamic_state_create_info() {
			return vk::PipelineDynamicStateCreateInfo{};
		}
	};
	template<class T>
	class disable_pipeline_depth_stencil : public T {
	public:
		auto get_pipeline_depth_stencil_state_create_info() {
			return vk::PipelineDepthStencilStateCreateInfo{}
			.setDepthTestEnable(false)
				.setDepthCompareOp(vk::CompareOp::eAlways);
		}
	};
	template<class T>
	class add_pipeline_color_blend_state_create_info : public T {
	public:
		using parent = T;
		auto get_pipeline_color_blend_state_create_info() {
			m_attachments = parent::get_pipeline_color_blend_attachment_states();
			return vk::PipelineColorBlendStateCreateInfo{}
				.setAttachments(
					m_attachments
				);
		}
	private:
		std::vector<vk::PipelineColorBlendAttachmentState> m_attachments;
	};
	template<uint32_t AttachmentIndex, class T>
	class disable_pipeline_attachment_color_blend : public T {
	public:
		using parent = T;
		auto get_pipeline_color_blend_attachment_states() {
			auto attachments = parent::get_pipeline_color_blend_attachment_states();
			attachments[AttachmentIndex]
				.setBlendEnable(false);
			return attachments;
		}
	};
	template<uint32_t AttachmentCount, class T>
	class add_pipeline_color_blend_attachment_states : public T {
	public:
		auto get_pipeline_color_blend_attachment_states() {
			auto states = std::vector<vk::PipelineColorBlendAttachmentState>(AttachmentCount);
			std::ranges::for_each(states, [](auto& state) {
				state.setColorWriteMask(vk::ColorComponentFlagBits::eA |
					vk::ColorComponentFlagBits::eR |
					vk::ColorComponentFlagBits::eG |
					vk::ColorComponentFlagBits::eB);
				});
			return states;
		}
	};
	template<class T>
	class add_pipeline_layout : public T {
	public:
		using parent = T;
		add_pipeline_layout() {
			vk::Device device = parent::get_device();
			//auto set_layouts = parent::get_descriptor_set_layouts();
			
			m_layout = device.createPipelineLayout(
				vk::PipelineLayoutCreateInfo{}
				//.setSetLayouts(set_layouts)
			);
		}
		~add_pipeline_layout() {
			vk::Device device = parent::get_device();
			device.destroyPipelineLayout(m_layout);
		}
		auto get_pipeline_layout() {
			return m_layout;
		}
	private:
		vk::PipelineLayout m_layout;
	};
	template<class T>
	class add_descriptor_set_layout : public T {
	public:
		using parent = T;
		add_descriptor_set_layout() {
			vk::Device device = parent::get_device();
			auto bindings = parent::get_descriptor_set_layout_bindings();
			device.createDescriptorSetLayout(
				vk::DescriptorSetLayoutCreateInfo{}
				.setBindings(bindings)
			);
		}
		~add_descriptor_set_layout() {
			vk::Device device = parent::get_device();
			device.destroyDescriptorSetLayout(m_layout);
		}
	private:
		vk::DescriptorSetLayout m_layout;
	};
	template<class T>
	class add_descriptor_set_layout_binding : public T {
	public:
		using parent = T;
		add_descriptor_set_layout_binding() {
			uint32_t binding = parent::get_binding();
			uint32_t descriptor_count = parent::get_descriptor_count();
			vk::DescriptorType descriptor_type = parent::get_desciptor_type();
			auto immutable_samplers = parent::get_immutable_samplers();
			vk::PipelineStageFlags stage_flags = parent::get_pipeline_stage_flags();
			m_binding = vk::DescriptorSetLayoutBinding{}
				.setBinding(binding)
				.setDescriptorCount(descriptor_count)
				.setDescriptorType(descriptor_type)
				.setImmutableSamplers(immutable_samplers)
				.setStageFlags(stage_flags);
		}
		auto get_descriptor_set_layout_binding() {
			return m_binding;
		}
	private:
		vk::DescriptorSetLayoutBinding m_binding;
	};
	template<uint32_t Descriptor_count, class T> 
	class set_descriptor_count : public T {
	public:
		auto get_descriptor_count() {
			return Descriptor_count;
		}
	};
	template<vk::DescriptorType Descriptor_type, class T>
	class set_descriptor_type : public T {
	public:
		auto get_descriptor_type() {
			return Descriptor_type;
		}
	};
	template<class T>
	class add_empty_immutable_samplers : public T {
	public:
		auto get_immutable_samplers() {
			return std::vector<vk::Sampler>{};
		}
	};
	template<class T>
	class add_recreate_surface : public T {
	public:
		using parent = T;
		void recreate_surface() {
			parent::destroy_surface();
			parent::create_surface();
		}
	};
}

namespace windows_helper {
	template<class T>
	class add_window_process : public T {
	public:
		static LRESULT CALLBACK window_process(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			static std::map<HWND, add_window_process<T>*> hwnd_this{};
			switch (uMsg)
			{
			case WM_CREATE:
			{
				auto create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
				hwnd_this.emplace(hwnd, reinterpret_cast<add_window_process<T>*>(create_struct->lpCreateParams));
				break;
			}
			case WM_SIZE:
			{
				uint16_t width = lParam;
				uint16_t height = lParam >> 16;
				hwnd_this[hwnd]->set_size(width, height);
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			}
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		}
		void* get_lparam() {
			return this;
		}
		void set_size(uint16_t width, uint16_t height) {
			m_width = width;
			m_height = height;
		}
		bool is_window_minimized() {
			return m_width == 0 || m_height == 0;
		}
	private:
		static add_window_process<T>* m_this;
		uint16_t m_width;
		uint16_t m_height;
	};
	template<class T>
	class add_window_class : public T {
	public:
		using parent = T;
		add_window_class() {
			const char* window_class_name = "draw_pixels";
			WNDCLASS window_class{};
			window_class.hInstance = GetModuleHandle(NULL);
			window_class.lpszClassName = window_class_name;
			window_class.lpfnWndProc = parent::window_process;
			m_window_class = RegisterClass(&window_class);
		}
		~add_window_class() {
			UnregisterClass((LPCSTR)m_window_class, GetModuleHandle(NULL));
		}
		auto get_window_class() {
			return m_window_class;
		}
	private:
		ATOM m_window_class;

	};
	template<int Width, int Height, class T>
	class set_window_resolution : public T {
	public:
		auto get_window_width() {
			return Width;
		}
		auto get_window_height() {
			return Height;
		}
	};
	template<int WindowStyle, class T>
	class set_window_style : public T {
	public:
		auto get_window_style() {
			return WindowStyle;
		}
	};
	template<class T>
	class adjust_window_resolution : public T {
	public:
		using parent = T;
		adjust_window_resolution() {
			auto width = parent::get_window_width();
			auto height = parent::get_window_height();
			auto window_style = parent::get_window_style();
			RECT rect = { 0,0,width,height };
			AdjustWindowRect(&rect, window_style, false);
			m_width = rect.right - rect.left;
			m_height = rect.bottom - rect.top;
		}
		auto get_window_width() {
			return m_width;
		}
		auto get_window_height() {
			return m_height;
		}
	private:
		int m_width;
		int m_height;
	};
	template<class T>
	class add_window : public T {
	public:
		using parent = T;
		add_window() {
			int width = parent::get_window_width();
			int height = parent::get_window_height();
			int window_style = parent::get_window_style();
			m_window = CreateWindowA(
				(LPCSTR)parent::get_window_class(), "draw_pixels", window_style, CW_USEDEFAULT, CW_USEDEFAULT,
				width, height, NULL, NULL, GetModuleHandle(NULL), parent::get_lparam());
			if (m_window == NULL) {
				throw std::runtime_error("failed to create window");
			}
			ShowWindow(m_window, SW_SHOWNORMAL);
		}
		auto get_window() {
			return m_window;
		}
	private:
		HWND m_window;
	};
	template<class T>
	class add_window_loop : public T {
	public:
		using parent = T;
		add_window_loop() {
			MSG msg = { };
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					parent::draw();
				}
			}
		}
	};

}

namespace vulkan_windows_helper {

	template<class T>
	class add_windows_surface : public T {
	public:
		using parent = T;
		add_windows_surface() {
			create_surface();
		}
		~add_windows_surface() {
			destroy_surface();
		}
		void create_surface() {
			vk::Instance instance = parent::get_instance();
			m_surface = instance.createWin32SurfaceKHR(
				vk::Win32SurfaceCreateInfoKHR{}
				.setHinstance(GetModuleHandleA(NULL))
				.setHwnd(parent::get_window())
			);
		}
		void destroy_surface() {
			vk::Instance instance = parent::get_instance();
			instance.destroySurfaceKHR(m_surface);
		}
		auto get_surface() {
			return m_surface;
		}
	private:
		vk::SurfaceKHR m_surface;
	};
}

int main() {
	try {
		using namespace windows_helper;
		using namespace vulkan_hpp_helper;
		auto show_window =
			add_window_loop<
			jump_draw_if_window_minimized<
			add_draw<
			add_acquire_next_image_semaphores<
			add_acquire_next_image_semaphore_fences<
			add_draw_semaphores<
			add_recreate_surface_for<
			record_swapchain_command_buffers<
			add_get_format_clear_color_value_type<
			add_recreate_surface_for<
			add_swapchain_command_buffers<
			add_command_pool<
			add_queue<
			add_vertex_buffer_memory<
			add_vertex_buffer<
			add_vertex_buffer_data<
			add_recreate_surface_for_pipeline<
			add_graphics_pipeline<
			add_pipeline_vertex_input_state<
			add_vertex_binding_description<
			add_empty_binding_descriptions<
			add_vertex_attribute_description<
			add_empty_vertex_attribute_descriptions<
			set_binding<0,
			set_stride<sizeof(float)*2,
			set_input_rate<vk::VertexInputRate::eVertex,
			set_subpass<0,
			add_recreate_surface_for_framebuffers<
			add_framebuffers <
			add_render_pass<
			add_subpasses<
			add_subpass_dependency<
			add_empty_subpass_dependencies<
			add_attachment<
			add_empty_attachments<
			add_pipeline_viewport_state<
			add_scissor_equal_surface_rect <
			add_empty_scissors <
			add_viewport_equal_swapchain_image_rect<
			add_empty_viewports<
			set_tessellation_patch_control_point_count<1,
			add_pipeline_stage<
			set_shader_stage<vk::ShaderStageFlagBits::eVertex,
			add_shader_module<
			add_spirv_code<
			adapte_map_file_to_spirv_code<
			map_file_mapping<
			cache_file_size<
			add_file_mapping<
			add_file<
			add_vertex_shader_path <
			add_pipeline_stage<
			set_shader_stage<vk::ShaderStageFlagBits::eFragment,
			set_shader_entry_name_with_main <
			add_shader_module<
			add_spirv_code<
			adapte_map_file_to_spirv_code<
			map_file_mapping<
			cache_file_size<
			add_file_mapping<
			add_file<
			add_fragment_shader_path<
			add_empty_pipeline_stages<
			add_pipeline_layout<
			set_pipeline_rasterization_polygon_mode<vk::PolygonMode::eFill,
			disable_pipeline_multisample<
			set_pipeline_input_topology<vk::PrimitiveTopology::eTriangleList,
			disable_pipeline_dynamic<
			disable_pipeline_depth_stencil<
			add_pipeline_color_blend_state_create_info<
			disable_pipeline_attachment_color_blend<0, // disable index 0 attachment
			add_pipeline_color_blend_attachment_states<1, // 1 attachment
			add_recreate_surface_for_images_memories <
			add_images_memories<
			add_recreate_surface_for_images <
			add_images<
			set_image_samples<vk::SampleCountFlagBits::e1,
			rename_images<
			add_recreate_surface_for_images_memories<
			add_images_memories<
			add_image_memory_property<vk::MemoryPropertyFlagBits::eDeviceLocal,
			add_empty_image_memory_properties<
			add_recreate_surface_for_images<
			add_images<
			set_image_tiling<vk::ImageTiling::eOptimal,
			add_image_usage<vk::ImageUsageFlagBits::eTransferDst,
			add_image_usage<vk::ImageUsageFlagBits::eTransferSrc,
			add_empty_image_usages<
			add_image_count_equal_swapchain_image_count<
			add_image_format<vk::Format::eR32G32B32A32Sfloat,
			add_image_extent_equal_swapchain_image_extent<
			add_image_type<vk::ImageType::e2D,
			set_image_samples<vk::SampleCountFlagBits::e8,
			add_recreate_surface_for_swapchain_images_views<
			add_swapchain_images_views<
			add_recreate_surface_for_swapchain_images<
			add_swapchain_images<
			add_recreate_surface_for_swapchain<
			add_swapchain<
			add_swapchain_image_extent_equal_surface_current_extent<
			add_swapchain_image_format<
			add_device<
			add_swapchain_extension <
			add_empty_extensions<
			add_find_properties<
			cache_physical_device_memory_properties<
			add_recreate_surface_for_cache_surface_capabilites<
			cache_surface_capabilities<
			add_recreate_surface_for<
			test_physical_device_support_surface<
			add_queue_family_index <
			add_physical_device<
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
			>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		{};
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}