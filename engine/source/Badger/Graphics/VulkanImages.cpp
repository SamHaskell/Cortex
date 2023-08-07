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
        texture.Format = VK_FORMAT_R8G8B8A8_SRGB;

        vulkan_create_image(
            device->Device,
            device->PhysicalDevice,
            width,
            height,
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

        //

        vkDestroyBuffer(device->Device, stagingBuffer, nullptr);
        vkFreeMemory(device->Device, stagingMemory, nullptr);

        return texture;
    }
}