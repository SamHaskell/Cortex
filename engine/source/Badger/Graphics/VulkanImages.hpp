#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
    struct VulkanTexture2D {
        VkImage Image;
        VkDeviceMemory ImageMemory;
        u32 Width;
        u32 Height;
        VkFormat Format;
    };

    VulkanTexture2D vulkan_create_texture_2D(const std::shared_ptr<GraphicsDevice> device, const std::string& path);
}