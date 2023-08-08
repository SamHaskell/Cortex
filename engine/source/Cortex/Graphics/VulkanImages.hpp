#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
    struct VulkanTexture2D {
        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;
        u32 Width;
        u32 Height;
        VkFormat Format;
    };

    struct VulkanSampler2D {
        VkSampler Sampler;
        VkFilter MagnificationFilter;
        VkFilter MinificationFilter;
    };

    VulkanTexture2D vulkan_create_texture_2D(const std::shared_ptr<GraphicsDevice> device, const std::string& path);
    VulkanSampler2D vulkan_create_sampler_2D(const std::shared_ptr<GraphicsDevice> device);

    void vulkan_destroy_texture_2D(const std::shared_ptr<GraphicsDevice> device, VulkanTexture2D& texture);
    void vulkan_destroy_sampler_2D(const std::shared_ptr<GraphicsDevice> device, VulkanSampler2D& sampler);
}