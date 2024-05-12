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