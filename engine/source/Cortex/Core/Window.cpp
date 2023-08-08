#include "Cortex/Core/Window.hpp"

namespace Cortex {
    Window::Window() {
        m_WindowState.Width = 1280;
        m_WindowState.Height = 720;
        m_WindowState.Title = "Application";

        bool ok = glfwInit();
        ASSERT(ok, "GLFW failed to Initialize");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // TEMP UNTIL SWAPCHAIN RECREATION IS SORTED

        m_WindowHandle = glfwCreateWindow(1280, 720, "Cortex Application", nullptr, nullptr);

        glfwGetFramebufferSize(m_WindowHandle, &m_WindowState.FramebufferWidth, &m_WindowState.FramebufferHeight);

        glfwSetWindowUserPointer(m_WindowHandle, &m_WindowState);

        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window)
                                   {
            auto state = (WindowState*)glfwGetWindowUserPointer(window);
            Event e = {EventTag::WindowCloseEvent};
            if (!state->Callback(e)) {
                glfwSetWindowShouldClose(window, GLFW_FALSE);
            } });

        glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods)
                           {
            auto state = (WindowState*)glfwGetWindowUserPointer(window);
            Event e = {EventTag::KeyEvent};
            switch (action) {
                case GLFW_PRESS:
                    e.KeyEvent = {key, KeyAction::KEY_PRESS};
                    break;
                case GLFW_RELEASE:
                    e.KeyEvent = {key, KeyAction::KEY_RELEASE};
                    break;
                case GLFW_REPEAT:
                    e.KeyEvent = {key, KeyAction::KEY_REPEAT};
                    break;
            }
            state->Callback(e); });

        glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, i32 button, i32 action, i32 mods)
                                   {
            auto state = (WindowState*)glfwGetWindowUserPointer(window);
            Event e = {EventTag::MouseButtonEvent};
            switch (action) {
                case GLFW_PRESS:
                    e.MouseButtonEvent = {button, MouseButtonAction::MOUSEBUTTON_PRESS};
                break;
                case GLFW_RELEASE:
                    e.MouseButtonEvent = {button, MouseButtonAction::MOUSEBUTTON_RELEASE};
                break;
            }
            state->Callback(e); });

        glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, f64 xpos, f64 ypos)
                                 {
            auto state = (WindowState *)glfwGetWindowUserPointer(window);
            Event e = {EventTag::MouseMoveEvent};
            e.MouseMoveEvent = {xpos, ypos};
            state->Callback(e); });

        glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow *window, i32 width, i32 height)
                                  {
            auto state = (WindowState *)glfwGetWindowUserPointer(window);
            state->FramebufferWidth = width;
            state->FramebufferHeight = height;
            Event e = {EventTag::WindowFramebufferSizeEvent};
            e.WindowFramebufferSizeEvent = {width, height};
            state->Callback(e); });

        glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow *window, i32 width, i32 height)
                                  {
            auto state = (WindowState *)glfwGetWindowUserPointer(window);
            state->Width = width;
            state->Height = height;
            Event e = {EventTag::WindowSizeEvent};
            e.WindowSizeEvent = {width, height};
            state->Callback(e); });
    }

    Window::~Window() {
        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();
    }

    void Window::Update(f64 dt) {
        glfwSwapBuffers(m_WindowHandle);
        glfwPollEvents();
    }

    void Window::SetEventCallback(std::function<bool(Event &)> callback) {
        m_WindowState.Callback = callback;
    }

    void Window::GetVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) {
        glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, &surface);
    }
}