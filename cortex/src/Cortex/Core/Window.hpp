#pragma once

#include "Cortex/Utils/Asserts.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Cortex {
    class Window {
        public:
            Window();
            ~Window();
            b8 Init();
            b8 Update();
            b8 Shutdown();
        private:
            GLFWwindow* m_WindowInstance;
    };
}