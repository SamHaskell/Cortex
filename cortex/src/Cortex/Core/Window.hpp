#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Events/Event.hpp"
#include "Cortex/Events/KeyEvent.hpp"
#include "Cortex/Events/SystemEvent.hpp"
#include "Cortex/Events/MouseEvent.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Cortex
{
    class Window
    {
        using EventCallback = std::function<void(Event &)>;

    public:
        Window();
        ~Window();
        b8 Init(const char *title, u32 width, u32 height);
        b8 Update(b8 suspended);
        b8 Shutdown();

        void SetEventCallback(const EventCallback &cb);

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
            i32 PositionX;
            i32 PositionY;
        } m_WindowData;
    };
}