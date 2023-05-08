#include "Cortex/Rendering/VulkanSwapchain.hpp"

#include "Cortex/Utils/Logging.hpp"

#include "vulkan/vk_enum_string_helper.h"

namespace Cortex
{
    VulkanSwapchain::VulkanSwapchain(const std::unique_ptr<VulkanDevice> &device, const std::unique_ptr<VulkanInstance> &instance, u32 framebufferWidth, u32 framebufferHeight)
        : m_DeviceHandle(device->GetDevice()),
          m_Swapchain(CreateSwapchain(device, instance, framebufferWidth, framebufferHeight)),
          m_SwapchainImages(RetrieveSwapchainImages()),
          m_SwapchainImageViews(CreateSwapchainImageViews())
    {
        LogSwapchainDetails();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        for (auto &view : m_SwapchainImageViews)
        {
            vkDestroyImageView(m_DeviceHandle, view, nullptr);
        }
        vkDestroySwapchainKHR(m_DeviceHandle, m_Swapchain, nullptr);
    }

    VkSwapchainKHR VulkanSwapchain::CreateSwapchain(const std::unique_ptr<VulkanDevice> &device, const std::unique_ptr<VulkanInstance> &instance, u32 framebufferWidth, u32 framebufferHeight)
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        DeviceQueueDetails queueDetails = device->GetQueueDetails();
        DeviceSwapchainSupportDetails swapchainSupportDetails = device->GetSwapchainSupportDetails();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupportDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupportDetails.PresentModes);
        VkExtent2D extent = ChooseSwapchainExtents(swapchainSupportDetails.Capabilities, framebufferWidth, framebufferHeight);

        u32 imageCount = swapchainSupportDetails.Capabilities.minImageCount + 1;
        if (swapchainSupportDetails.Capabilities.maxImageCount > 0 && imageCount > swapchainSupportDetails.Capabilities.maxImageCount)
        {
            imageCount = swapchainSupportDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = instance->GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 queueIndices[2] = {queueDetails.GraphicsIndex, queueDetails.PresentIndex};
        if (queueDetails.GraphicsIndex != queueDetails.PresentIndex)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_DeviceHandle, &createInfo, nullptr, &swapchain);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Swapchain");

        m_SwapchainExtent = extent;
        m_SwapchainFormat = surfaceFormat.format;
        m_SwapchainPresentMode = presentMode;

        return swapchain;
    }

    std::vector<VkImage> VulkanSwapchain::RetrieveSwapchainImages()
    {
        u32 imageCount;
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, images.data());
        return images;
    }

    std::vector<VkImageView> VulkanSwapchain::CreateSwapchainImageViews()
    {
        std::vector<VkImageView> imageViews(m_SwapchainImages.size());
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapchainFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_DeviceHandle, &createInfo, nullptr, &imageViews[i]);
            CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan ImageView");
        }

        return imageViews;
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats)
    {
        for (const auto &surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB || surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
                {
                    return surfaceFormat;
                }
                else if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM || surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    return surfaceFormat;
                }
            }
        }
        CX_WARN("No preferred format available, defaulting to first found");
        return surfaceFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes)
    {
        for (const auto &presentMode : presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D trueExtent = {windowWidth, windowHeight};
            trueExtent.width = std::clamp(trueExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            trueExtent.width = std::clamp(trueExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return trueExtent;
        }
    }

    void VulkanSwapchain::LogSwapchainDetails()
    {
        u32 n = static_cast<u32>(m_SwapchainImages.size());

        CX_INFO("(Vulkan Swapchain) IMAGES: %u, PRESENT MODE: %s, FORMAT: %s", n, string_VkPresentModeKHR(m_SwapchainPresentMode), string_VkFormat(m_SwapchainFormat));
    }
}