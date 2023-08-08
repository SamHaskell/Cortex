#include "Badger/Graphics/VulkanImages.hpp"

namespace Badger {
    VulkanTexture2D vulkan_create_texture_2D(const std::shared_ptr<GraphicsDevice> device, const std::string& path) {
        i32 width, height, channels;
        stbi_uc* px = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        VkDeviceSize imageSize = width * height * 4;

        if (stbi_failure_reason()) {
            LOG_ERROR("%s", stbi_failure_reason());
        }

        if (!px) {
            LOG_FATAL("STBI failed to load image: %s", path.c_str());
            ASSERT(false, "Failed to load image from disk.");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        vulkan_create_buffer(
            device->PhysicalDevice, 
            device->Device, 
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingMemory
        );

        void* data;
        vkMapMemory(device->Device, stagingMemory, 0, imageSize, 0, &data);
        memcpy(data, px, static_cast<size_t>(imageSize));
        vkUnmapMemory(device->Device, stagingMemory);

        stbi_image_free(px);

        VulkanTexture2D texture;
        texture.Width = static_cast<u32>(width);
        texture.Height = static_cast<u32>(height);
        texture.Format = VK_FORMAT_R8G8B8A8_UNORM;

        vulkan_create_image(
            device->Device,
            device->PhysicalDevice,
            width,
            height,
            VK_SAMPLE_COUNT_1_BIT,
            texture.Format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            texture.Image,
            texture.ImageMemory
        );

        vulkan_transition_image_layout(
            device->Device, 
            device->TransferCommandPool, 
            device->Queues.Transfer,
            texture.Image,
            texture.Format,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );

        vulkan_copy_buffer_to_image(
            device->Device,
            device->TransferCommandPool,
            device->Queues.Transfer,
            stagingBuffer,
            texture.Image,
            texture.Width,
            texture.Height
        );

        vulkan_transition_image_layout(
            device->Device, 
            device->TransferCommandPool, 
            device->Queues.Transfer,
            texture.Image,
            texture.Format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        texture.ImageView = vulkan_create_image_view(device->Device, texture.Image, texture.Format, VK_IMAGE_ASPECT_COLOR_BIT);

        vkDestroyBuffer(device->Device, stagingBuffer, nullptr);
        vkFreeMemory(device->Device, stagingMemory, nullptr);

        return texture;
    }

    VulkanSampler2D vulkan_create_sampler_2D(const std::shared_ptr<GraphicsDevice> device) {
        VulkanSampler2D sampler = {};
        sampler.MagnificationFilter = VK_FILTER_LINEAR;
        sampler.MinificationFilter = VK_FILTER_LINEAR;

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device->PhysicalDevice, &props);

        VkSamplerCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.magFilter = sampler.MagnificationFilter;
        createInfo.minFilter = sampler.MinificationFilter;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.anisotropyEnable = VK_TRUE;
        createInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
        createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.mipLodBias = 0.0f;
        createInfo.minLod = 0.0f;
        createInfo.maxLod = 0.0f;

        VkResult result = vkCreateSampler(device->Device, &createInfo, nullptr, &sampler.Sampler);
        ASSERT(result == VK_SUCCESS, "Failed to create Vulkan texture sampler.");

        return sampler;
    }

    void vulkan_destroy_texture_2D(const std::shared_ptr<GraphicsDevice> device, VulkanTexture2D& texture) {
        vkDestroyImageView(device->Device, texture.ImageView, nullptr);
        vkFreeMemory(device->Device, texture.ImageMemory, nullptr);
        vkDestroyImage(device->Device, texture.Image, nullptr);
    }

    void vulkan_destroy_sampler_2D(const std::shared_ptr<GraphicsDevice> device, VulkanSampler2D& sampler) {
        vkDestroySampler(device->Device, sampler.Sampler, nullptr);
    }
}