#pragma once

#include "vulkan/vulkan.h"

#include <vector>

namespace Cortex {
    class Swapchain {
        public:
        Swapchain();
        ~Swapchain();

        private:
        VkColorSpaceKHR m_ColorSpace;
        VkFormat m_Format;
        VkSurfaceTransformFlagBitsKHR m_Transform;
        VkPresentModeKHR m_PresentMode;
        VkExtent2D m_Extent;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImageView> m_ImageViews;

    };
}