#include "Cortex/Core/Window.hpp"
#include "Cortex/Utils/Logging.hpp"

namespace Cortex
{
    Window::Window(const char *title, u32 width, u32 height)
    {
        b8 result = glfwInit();
        CX_ASSERT(result);

        m_WindowData.Title = title;
        m_WindowData.Width = width;
        m_WindowData.Height = height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_WindowInstance = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_WindowData.Title, NULL, NULL);

        glfwGetWindowPos(m_WindowInstance, &m_WindowData.PositionX, &m_WindowData.PositionY);
        glfwSetWindowUserPointer(m_WindowInstance, &m_WindowData);

        InitWindowCallbacks();
        InitKeyCallbacks();
        InitMouseCallbacks();

        glfwGetFramebufferSize(m_WindowInstance, &m_WindowData.FramebufferWidth, &m_WindowData.FramebufferHeight);

        glfwSetErrorCallback([](i32 code, const char *msg)
                             { CX_ERROR("GLFW Error %i: %s", code, msg); });
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_WindowInstance);
        glfwTerminate();
    }

    void Window::Update()
    {
        glfwPollEvents();
    }

    void Window::SetEventCallback(const EventCallback &cb)
    {
        m_WindowData.EventCallback = cb;
    }

    void Window::InitWindowCallbacks()
    {
        glfwSetWindowSizeCallback(m_WindowInstance, [](GLFWwindow *window, i32 w, i32 h)
                                  {
                                      WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
                                      data.Width = (u32)w;
                                      data.Height = (u32)h;
                                      glfwGetFramebufferSize(window, &data.FramebufferWidth, &data.FramebufferHeight);
                                      WindowResizeEvent e1(data.Title, (u32)w, (u32)h);
                                      WindowFramebufferResizeEvent e2((u32)data.FramebufferWidth, (u32)data.FramebufferHeight);
                                      data.EventCallback(e1);
                                      data.EventCallback(e2); });

        glfwSetWindowCloseCallback(m_WindowInstance, [](GLFWwindow *window)
                                   {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent e(data.Title);
            data.EventCallback(e); });

        glfwSetWindowPosCallback(m_WindowInstance, [](GLFWwindow *window, i32 x, i32 y)
                                 {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            glfwGetWindowPos(window, &data.PositionX, &data.PositionY);
            WindowMoveEvent e(data.Title, x, y);
            data.EventCallback(e); });

        glfwSetWindowFocusCallback(m_WindowInstance, [](GLFWwindow *window, int focused)
                                   {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            glfwGetWindowPos(window, &data.PositionX, &data.PositionY);
            if (focused) {
                WindowFocusEvent e(data.Title);
                data.EventCallback(e);
            } else {
                WindowDefocusEvent e(data.Title);
                data.EventCallback(e);
            } });
    }

    void Window::InitKeyCallbacks()
    {
        glfwSetKeyCallback(m_WindowInstance, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS : { KeyPressEvent e(key, false); data.EventCallback(e); break; };
                case GLFW_REPEAT : { KeyPressEvent e(key, true); data.EventCallback(e); break; };
                case GLFW_RELEASE : { KeyReleaseEvent e(key); data.EventCallback(e); break; };
            } });
    }

    void Window::InitMouseCallbacks()
    {
        glfwSetScrollCallback(m_WindowInstance, [](GLFWwindow *window, f64 xoffset, f64 yoffset)
                              {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrollEvent e(xoffset, yoffset);
            data.EventCallback(e); });

        glfwSetCursorPosCallback(m_WindowInstance, [](GLFWwindow *window, f64 xpos, f64 ypos)
                                 {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMoveEvent e(xpos, ypos);
            data.EventCallback(e); });

        glfwSetMouseButtonCallback(m_WindowInstance, [](GLFWwindow *window, i32 button, i32 action, i32 mods)
                                   {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS : { MouseButtonPressEvent e(button); data.EventCallback(e); break; };
                case GLFW_REPEAT : { MouseButtonPressEvent e(button); data.EventCallback(e); break; };
                case GLFW_RELEASE : { MouseButtonReleaseEvent e(button); data.EventCallback(e); break; };
            } });
    }

    void Window::CreateVulkanSurface(const VkInstance instance, VkSurfaceKHR &surface)
    {
        VkResult result = glfwCreateWindowSurface(instance, m_WindowInstance, nullptr, &surface);
        CX_ASSERT_MSG(result == VK_SUCCESS, "GLFW failed to create a Vulkan surface");
    }
}