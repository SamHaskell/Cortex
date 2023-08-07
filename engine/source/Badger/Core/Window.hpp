#pragma once

#include "Badger/Base/Base.hpp"
#include "Badger/Core/Events.hpp"

#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <functional>

namespace Badger
{
    struct WindowState
    {
        i32 Width;
        i32 Height;
        i32 FramebufferWidth;
        i32 FramebufferHeight;
        const char *Title;
        std::function<bool(Event &)> Callback;
    };

    class Window
    {
    public:
        Window();
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        ~Window();

        void Update(f64 dt);
        void SetEventCallback(std::function<bool(Event &)> callback);

        inline i32 GetFramebufferWidth() { return m_WindowState.FramebufferWidth; }
        inline i32 GetFramebufferHeight() { return m_WindowState.FramebufferHeight; }
        inline f32 GetAspectRatio() { return (f32)m_WindowState.FramebufferWidth / (f32)m_WindowState.FramebufferHeight; }

        void GetVulkanSurface(VkInstance instance, VkSurfaceKHR& surface);

    private:
        GLFWwindow *m_WindowHandle;
        WindowState m_WindowState;
    };
}