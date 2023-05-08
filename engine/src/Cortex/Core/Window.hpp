#pragma once

#include "Cortex/Utils/Asserts.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Cortex/Events/Event.hpp"
#include "Cortex/Events/KeyEvent.hpp"
#include "Cortex/Events/SystemEvent.hpp"
#include "Cortex/Events/MouseEvent.hpp"

namespace Cortex
{
    class Window
    {
        using EventCallback = std::function<void(Event &)>;

    public:
        Window(const char *title, u32 width, u32 height);
        ~Window();

        Window(const Window &) = delete;
        void operator=(const Window &) = delete;

        void Update();
        void SetEventCallback(const EventCallback &cb);

        inline u32 GetFramebufferWidth() const { return static_cast<u32>(m_WindowData.FramebufferWidth); }
        inline u32 GetFramebufferHeight() const { return static_cast<u32>(m_WindowData.FramebufferHeight); }

        void CreateVulkanSurface(const VkInstance instance, VkSurfaceKHR &surface) const;

    private:
        void InitWindowCallbacks();
        void InitKeyCallbacks();
        void InitMouseCallbacks();

    private:
        GLFWwindow *m_WindowInstance;

        struct WindowData
        {
            EventCallback EventCallback;
            const char *Title;
            u32 Width;
            u32 Height;
            i32 FramebufferWidth;
            i32 FramebufferHeight;
            i32 PositionX;
            i32 PositionY;
        } m_WindowData;
    };
}