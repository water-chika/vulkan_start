#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include <vulkan_helper.hpp>
#include <iostream>
#include <numeric>
#include <map>
#include <string>

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

using namespace vulkan_hpp_helper;
using namespace vulkan_windows_helper;
using namespace windows_helper;


template<class T>
class add_dispatch_loader : public T {
public:
	auto get_dispatch_loader() {
		return m_dld;
	}
private:
	vk::DispatchLoaderDynamic m_dld;
};

template<class T>
class add_acceleration_structure_geometry : public T {
public:
	auto get_acceleration_structure_geometry() {
		return std::array{
			vk::AccelerationStructureGeometryKHR{}
			.setGeometryType(vk::GeometryTypeKHR::eTriangles)
			.setGeometry(
				vk::AccelerationStructureGeometryTrianglesDataKHR{}
				.setVertexFormat(vk::Format::eR32G32B32Sfloat)
				.setVertexStride(sizeof(float)*3)
				.setMaxVertex(3)
				.setIndexType(vk::IndexType::eUint16)
			)
		};
	}
};

template<class T>
class add_accelration_structure_build_sizes : public T {
public:
	using parent = T;
	add_accelration_structure_build_sizes() {
		vk::Device device = parent::get_device();
	}
};

template<class T>
class add_acceleration_structure : public T {
public:
	using parent = T;
	add_acceleration_structure() {
		vk::Device device = parent::get_device();
		vk::Buffer buffer = parent::get_buffer();
		auto loader = parent::get_dispatch_loader();

		m_acceleration_structure = device.createAccelerationStructureKHR(
			vk::AccelerationStructureCreateInfoKHR{}
			.setBuffer(buffer)
			.setType(vk::AccelerationStructureTypeKHR::eBottomLevel)
			.setSize(100),
			nullptr,
			loader
		);
	}
	~add_acceleration_structure() {
		vk::Device device = parent::get_device();
		auto loader = parent::get_dispatch_loader();

		device.destroyAccelerationStructureKHR(m_acceleration_structure, nullptr, loader);
	}
private:
	vk::AccelerationStructureKHR m_acceleration_structure;
};


template<class T>
class build_acceleration_structure : public T {
public:
	using parent = T;
	build_acceleration_structure() {
		vk::Device device = parent::get_device();

		vk::CommandBuffer cmd = parent::get_command();
		auto geometries = parent::get_acceleration_structure_geometry();

		auto build_geometries = std::vector{
			vk::AccelerationStructureBuildGeometryInfoKHR{}
			.setGeometries(geometries)
		};
		auto build_ranges = std::array{
			vk::AccelerationStructureBuildRangeInfoKHR{}
			.setPrimitiveCount(1)
		};

		cmd.buildAccelerationStructuresKHR();
	}
	~build_acceleration_structure() {
	}
private:
};


using ray_tracing_app =
	add_acceleration_structure<
	add_dispatch_loader<
	add_buffer_as_member<
	set_buffer_size<100,
	set_buffer_usage<vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR,
	add_command_buffer<
	add_command_pool <
	add_queue <
	add_recreate_surface_for_swapchain_images <
	add_swapchain_images <
	add_recreate_surface_for_swapchain <
	add_swapchain <
	add_swapchain_image_extent_equal_surface_current_extent <
	add_swapchain_image_format <
	add_device <
	add_swapchain_extension <
	add_empty_extensions <
	add_find_properties <
	cache_physical_device_memory_properties <
	add_recreate_surface_for_cache_surface_capabilites <
	cache_surface_capabilities <
	add_recreate_surface_for <
	test_physical_device_support_surface <
	add_queue_family_index <
	add_physical_device <
	add_recreate_surface <
	vulkan_windows_helper::add_windows_surface <
	add_instance <
	add_win32_surface_extension <
	add_surface_extension <
	add_empty_extensions <
	add_window <
	adjust_window_resolution <
	set_window_resolution < 151, 151,
	set_window_style < WS_OVERLAPPEDWINDOW,
	add_window_class <
	add_window_process <
	empty_class
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	;

int main() {
	try {
		ray_tracing_app{};
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}