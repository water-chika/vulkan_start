#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include <vulkan_helper.hpp>
#include <iostream>

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
	class add_swapchain : public T {
	public:
		using parent = T;
		add_swapchain() {
			vk::Device device = parent::get_device();
			vk::PhysicalDevice physical_device = parent::get_physical_device();
			vk::SurfaceKHR surface = parent::get_surface();
			vk::Format format = parent::get_swapchain_image_format();

			auto cap = physical_device.getSurfaceCapabilitiesKHR(surface);
			m_swapchain = device.createSwapchainKHR(
				vk::SwapchainCreateInfoKHR{}
				.setMinImageCount(cap.minImageCount)
				.setImageExtent(cap.currentExtent)
				.setImageFormat(format)
				.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
				.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
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
		add_window_loop() {
			MSG msg = { };
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
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
			add_command_buffer<
			add_command_pool<
			add_queue<
			add_swapchain_images<
			add_swapchain<
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
			>>>>>>>>>>>>>>>>>>>
		{};
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}