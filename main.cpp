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

			uint32_t image_count = parent::get_image_count();

			m_images.resize(image_count);

			std::ranges::for_each(
				m_images,
				[device, extent, format, image_type, queue_family_index, image_usage](vk::Image& image) {
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
	private:
		std::vector<vk::Image> m_images;
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
			auto images = parent::get_swapchain_images();
			auto queue_family_index = parent::get_queue_family_index();

			auto clear_color_value_type = parent::get_format_clear_color_value_type(parent::get_swapchain_image_format());
			using value_type = decltype(clear_color_value_type);
			std::map<value_type, vk::ClearColorValue> clear_color_values{
				{value_type::eFloat32, vk::ClearColorValue{}.setFloat32({0.5f,0.5f,0.5f,1.0f})},
			};
			vk::ClearColorValue clear_color_value{ clear_color_values[clear_color_value_type] };

			if (buffers.size() != images.size()) {
				throw std::runtime_error{ "swapchain images count != command buffers count" };
			}
			uint32_t index = 0;
			for (uint32_t index = 0; index < buffers.size(); index++) {
				vk::Image image = images[index];
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
					.setImage(image)
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
					image,
					vk::ImageLayout::eTransferDstOptimal,
					clear_color_value,
					vk::ImageSubresourceRange{}
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLayerCount(1)
					.setLevelCount(1));
				buffer.pipelineBarrier(
					vk::PipelineStageFlagBits::eTransfer,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					{},
					{},
					{},
					vk::ImageMemoryBarrier{}
					.setImage(image)
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
	template<class T>
	class add_window : public T {
	public:
		using parent = T;
		add_window() {
			m_window = CreateWindowA(
				(LPCSTR)parent::get_window_class(), "draw_pixels", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
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
			add_images<
			add_image_usage<vk::ImageUsageFlagBits::eTransferSrc,
			add_empty_image_usages<
			add_image_count_equal_swapchain_image_count<
			add_image_format<vk::Format::eR32G32B32A32Uint,
			add_image_extent_equal_swapchain_image_extent<
			add_image_type<vk::ImageType::e2D,
			add_swapchain_images<
			add_swapchain<
			add_swapchain_image_extent_equal_surface_current_extent<
			add_swapchain_image_format<
			add_device<
			add_queue_family_index<
			add_swapchain_extension <
			add_empty_extensions<
			add_physical_device<
			vulkan_windows_helper::add_windows_surface<
			add_instance<
			add_win32_surface_extension<
			add_surface_extension<
			add_empty_extensions<
			add_window<
			add_window_class<
			empty_class
			>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		{};
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}