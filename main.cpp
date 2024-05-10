#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include <vulkan_helper.hpp>
#include <iostream>
#include <numeric>
#include <map>

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
			vk::SurfaceKHR surface = parent::get_surface();
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			auto cap = physical_device.getSurfaceCapabilitiesKHR(surface);
			return cap.currentExtent;
		}
	};
	template<class T>
	class add_swapchain : public T {
	public:
		using parent = T;
		add_swapchain() {
			vk::Device device = parent::get_device();
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			vk::SurfaceKHR surface = parent::get_surface();
			vk::Format format = parent::get_swapchain_image_format();
			vk::Extent2D swapchain_image_extent = parent::get_swapchain_image_extent();

			auto cap = physical_device.getSurfaceCapabilitiesKHR(surface);
			m_swapchain = device.createSwapchainKHR(
				vk::SwapchainCreateInfoKHR{}
				.setMinImageCount(cap.minImageCount)
				.setImageExtent(swapchain_image_extent)
				.setImageFormat(format)
				.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
				.setImageUsage(vk::ImageUsageFlagBits::eTransferDst)
				.setImageArrayLayers(1)
				.setSurface(surface)
			);
		}
		~add_swapchain() {
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
	class add_swapchain_images : public T {
	public:
		using parent = T;
		add_swapchain_images() {
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
	class add_images : public T {
	public:
		using parent = T;
		add_images() {
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
		~add_images() {
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
	class add_images_memories : public T {
	public:
		using parent = T;
		add_images_memories() {
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
		~add_images_memories() {
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
	class add_swapchain_command_buffers : public T {
	public:
		using parent = T;
		add_swapchain_command_buffers() {
			vk::Device device = parent::get_device();
			vk::CommandPool pool = parent::get_command_pool();
			uint32_t swapchain_image_count = parent::get_swapchain_images().size();
			m_buffers = device.allocateCommandBuffers(
				vk::CommandBufferAllocateInfo{}
				.setCommandPool(pool)
				.setCommandBufferCount(swapchain_image_count)
			);
		}
		~add_swapchain_command_buffers() {
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
	class record_swapchain_command_buffers : public T {
	public:
		using parent = T;
		record_swapchain_command_buffers() {
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
				buffer.blitImage(unsampled_image, vk::ImageLayout::eTransferSrcOptimal, swapchain_image, vk::ImageLayout::eTransferDstOptimal,
					vk::ImageBlit{}
					.setSrcSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
					.setDstSubresource(vk::ImageSubresourceLayers{}.setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor))
					.setSrcOffsets(std::array { vk::Offset3D{ 0,0,0 }, image_end})
					.setDstOffsets(std::array{ vk::Offset3D{ 0,0,0 }, image_end}),
					vk::Filter::eNearest
				);
				buffer.pipelineBarrier(
					vk::PipelineStageFlagBits::eTransfer,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					{},
					{},
					{},
					vk::ImageMemoryBarrier{}
					.setImage(swapchain_image)
					.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
					.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
					.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
					.setDstAccessMask({})
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
			
			
			auto [res, index] = device.acquireNextImage2KHR(
				vk::AcquireNextImageInfoKHR{}
				.setSwapchain(swapchain)
				.setSemaphore(acquire_image_semaphore)
				.setTimeout(UINT64_MAX)
				.setDeviceMask(1)
			);
			if (res != vk::Result::eSuccess) {
				throw std::runtime_error{ "acquire next image != success" };
			}
			parent::free_acquire_next_image_semaphore(index);

			vk::Fence acquire_next_image_semaphore_fence = parent::get_acquire_next_image_semaphore_fence(index);
			device.waitForFences(acquire_next_image_semaphore_fence, true, UINT64_MAX);
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
				if (res != vk::Result::eSuccess) {
					throw std::runtime_error{ "present return != success" };
				}
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
	class add_graphics_pipeline : public T {
	public:
		using parent = T;
		add_graphics_pipeline() {
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
			uint32_t subpass = parent::get_subpass();
			
			m_pipeline = device.createGraphicsPipeline(
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
				.setSubpass(subpass)
			);
		}
		~add_graphics_pipeline() {
			vk::Device device = parent::get_device();
			device.destroyPipeline(m_pipeline);
		}
		auto get_pipeline() {
			return m_pipeline;
		}
	private:
		vk::Pipeline m_pipeline;
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
			return vk::PipelineDepthStencilStateCreateInfo{};
		}
	};
	template<class T>
	class add_pipeline_color_blend_state_create_info : public T {
	public:
		using parent = T;
		auto get_pipeline_color_blend_state_create_info() {
			auto attachments = parent::get_pipeline_color_blend_attachment_states();
			return vk::PipelineColorBlendStateCreateInfo{}
				.setAttachments(
					attachments
				);
		}
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
			return std::vector<vk::PipelineColorBlendAttachmentState>(AttachmentCount);
		}
	};
	template<class T>
	class add_pipeline_layout : public T {
	public:
		using parent = T;
		add_pipeline_layout() {
			vk::Device device = parent::get_device();
			auto set_layouts = parent::get_descriptor_set_layouts();
			auto push_constant_ranges = parent::get_pipeline_layout_push_constant_ranges();
			
			m_layout = device.createPipelineLayout(
				vk::PipelineLayoutCreateInfo{}
				.setSetLayouts(set_layouts)
				.setPushConstantRanges(push_constant_ranges)
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
	template<uint32_t Binding, class T>
	class set_binding : public T {
	public:
		auto get_binding() {
			return Binding;
		}
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
}

namespace windows_helper {
	template<class T>
	class add_window_class : public T {
	public:
		add_window_class() {
			const char* window_class_name = "draw_pixels";
			WNDCLASS window_class{};
			window_class.hInstance = GetModuleHandle(NULL);
			window_class.lpszClassName = window_class_name;
			window_class.lpfnWndProc = window_process;
			m_window_class = RegisterClass(&window_class);
		}
		static LRESULT CALLBACK window_process(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg)
			{
			case WM_CREATE:
			{
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
				width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
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
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				parent::draw();
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	};
}

namespace vulkan_windows_helper {
	template<class T>
	class add_create_win32_surface_khr : public T {
	public:
	private:
		
	};
	template<class T>
	class add_windows_surface : public T {
	public:
		using parent = T;
		add_windows_surface() {
			vk::Instance instance = parent::get_instance();
			m_surface = instance.createWin32SurfaceKHR(
				vk::Win32SurfaceCreateInfoKHR{}
				.setHinstance(GetModuleHandleA(NULL))
				.setHwnd(parent::get_window())
			);
		}
		~add_windows_surface() {
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
			add_draw<
			add_acquire_next_image_fences<
			add_acquire_next_image_semaphores<
			add_acquire_next_image_semaphore_fences<
			add_draw_semaphores<
			record_swapchain_command_buffers<
			add_get_format_clear_color_value_type<
			add_swapchain_command_buffers<
			add_command_pool<
			add_queue<
			add_graphics_pipeline<
			add_pipeline_layout<
			set_pipeline_input_topology<vk::PrimitiveTopology::eTriangleList,
			disable_pipeline_dynamic<
			disable_pipeline_depth_stencil<
			add_pipeline_color_blend_state_create_info<
			disable_pipeline_attachment_color_blend<0, // disable index 0 attachment
			add_pipeline_color_blend_attachment_states<1, // 1 attachment
			add_images_memories<
			add_images<
			set_image_samples<vk::SampleCountFlagBits::e1,
			rename_images<
			add_images_memories<
			add_image_memory_property<vk::MemoryPropertyFlagBits::eDeviceLocal,
			add_empty_image_memory_properties<
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
			add_swapchain_images<
			add_swapchain<
			add_swapchain_image_extent_equal_surface_current_extent<
			add_swapchain_image_format<
			add_device<
			add_queue_family_index<
			add_swapchain_extension <
			add_empty_extensions<
			add_find_properties<
			cache_physical_device_memory_properties<
			add_physical_device<
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
			empty_class
			>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		{};
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}