#include "Cortex/Graphics/RendererSwapchain.hpp"

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Graphics/RendererDevice.hpp"

#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

namespace Cortex
{
    RendererSwapchainConfig RendererSwapchainConfig::Default(u32 width, u32 height)
    {
        RendererSwapchainConfig config;
        config.FramebufferWidth = width;
        config.FramebufferHeight = height;
        return config;
    }

    RendererSwapchain::RendererSwapchain(const RendererSwapchainConfig &config, const VkDevice &device, const VkSurfaceKHR& surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails)
    : m_DeviceHandle(device),
      m_Swapchain(CreateSwapchain(surface, queueDetails, swapchainDetails, config)),
      m_SwapchainImages(RetrieveSwapchainImages()),
      m_SwapchainImageViews(CreateSwapchainImageViews())
    {
        LogSwapchainDetails();
    }

    RendererSwapchain::~RendererSwapchain()
    {
        for (auto& view : m_SwapchainImageViews) {
            vkDestroyImageView(m_DeviceHandle, view, nullptr);
        }
        vkDestroySwapchainKHR(m_DeviceHandle, m_Swapchain, nullptr);
    }

    VkSwapchainKHR RendererSwapchain::CreateSwapchain(const VkSurfaceKHR &surface, const DeviceQueueFamilies& queueDetails, const DeviceSwapchainSupportDetails& swapchainDetails, const RendererSwapchainConfig &config)
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainDetails.PresentModes);
        VkExtent2D extent = ChooseSwapchainExtents(swapchainDetails.Capabilities, config.FramebufferWidth, config.FramebufferHeight);

        u32 imageCount = swapchainDetails.Capabilities.minImageCount + 1;
        if (swapchainDetails.Capabilities.maxImageCount > 0 && imageCount > swapchainDetails.Capabilities.maxImageCount)
        {
            imageCount = swapchainDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 queueIndices[2] = {queueDetails.GraphicsFamily, queueDetails.PresentFamily};
        if (queueDetails.GraphicsFamily != queueDetails.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainDetails.Capabilities.currentTransform;
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

    std::vector<VkImage> RendererSwapchain::RetrieveSwapchainImages()
    {
        u32 imageCount;
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(m_DeviceHandle, m_Swapchain, &imageCount, images.data());
        return images;
    }

    std::vector<VkImageView> RendererSwapchain::CreateSwapchainImageViews()
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

    VkSurfaceFormatKHR RendererSwapchain::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> surfaceFormats)
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

    VkPresentModeKHR RendererSwapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> presentModes)
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

    VkExtent2D RendererSwapchain::ChooseSwapchainExtents(const VkSurfaceCapabilitiesKHR capabilities, u32 windowWidth, u32 windowHeight)
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

    void RendererSwapchain::LogSwapchainDetails()
    {
        u32 n = static_cast<u32>(m_SwapchainImages.size());

        CX_INFO("(Vulkan Swapchain) IMAGES: %u, PRESENT MODE: %s, FORMAT: %s", n, string_VkPresentModeKHR(m_SwapchainPresentMode), string_VkFormat(m_SwapchainFormat));
    }
}