#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vulkan_helper.hpp>

namespace vulkan_start {

using namespace vulkan_hpp_helper;
enum class platform {
    win32,
    wayland,
    display,
};

template <class T> class rename_images : public T {
public:
  using parent = T;
  auto get_intermediate_images() { return parent::get_images(); }
};

template<platform PLATFORM>
class use_platform {
public:

template<class T>
class add_vulkan_surface : public T
{
public:
    add_vulkan_surface() = delete;
};

template<class T>
class add_platform_needed_extensions : public T
{
public:
    add_platform_needed_extensions() = delete;
};

template<class T>
class add_event_loop : public T
{
public:
    add_event_loop() = delete;
};

template<class T>
class add_window : public T
{
public:
    add_window() = delete;
};

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




template<platform PLATFORM>
class use_platform_add_swapchain_image_extent {
public:
template<class T>
class add_swapchain_image_extent
    : public add_swapchain_image_extent_equal_surface_current_extent<T> {
};
};

} // namespace vulkan_start

#if WIN32
#include "vulkan_start_windows.hpp"
#else
#include "vulkan_start_wayland.hpp"
#include "cube_display.hpp"
#endif

namespace vulkan_start {
using namespace vulkan_hpp_helper;
using namespace std::literals;

template <platform PLATFORM, template<typename> typename C> class run_on_platform
  : public
  use_platform<PLATFORM>::template add_event_loop<
  C<
	add_instance<
	typename use_platform<PLATFORM>::template add_platform_needed_extensions<
	add_surface_extension<
	add_empty_extensions<
	typename use_platform<PLATFORM>::template add_window<
  empty_class
  >>>>>>>
{};

template <std::invocable<> CALL, class T> class add_file_path : public T {
public:
    auto get_file_path() { return CALL{}(); }
};

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

using namespace std::chrono;
template<class T>
class add_frame_time_analyser : public T{
public:
    using parent = T;
    add_frame_time_analyser(const configure auto& conf) : parent{conf},
        m_frame_time{}, m_frame_index{}, m_last_time_point{}, m_previous_index{}{
    }
    void draw() {
        auto now = steady_clock::now();
        if (now - m_last_time_point > 500ms && m_previous_index != m_frame_index) {
            m_frame_time = (now - m_last_time_point) / (m_frame_index - m_previous_index);
            double fps = 1000000000.0/m_frame_time.count();
            /*std::clog
                << "frame time: "
                << std::setw(10)
                << m_frame_time.count()/1000000.0
                << "ms"
                << "fps: "
                << fps
                << "\t\r";*/
            m_last_time_point = now;
            m_previous_index = m_frame_index;
        }

        parent::draw();

        m_frame_index++;
    }
    auto get_cpu_frame_time() {
        return m_frame_time;
    }
private:
    nanoseconds m_frame_time;
    uint64_t m_frame_index;
    time_point<steady_clock, nanoseconds> m_last_time_point;
    uint64_t m_previous_index;
};

}
