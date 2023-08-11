#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
    struct VulkanSampler2D {
        VkSampler Sampler;
        VkFilter MagnificationFilter;
        VkFilter MinificationFilter;
    };

    class Texture2D {
        public:
            Texture2D(const std::shared_ptr<GraphicsDevice> device, const std::string& path);
            ~Texture2D();
            static std::shared_ptr<Texture2D> Create(const std::shared_ptr<GraphicsDevice> device, const std::string& path);
            inline const VkDescriptorImageInfo& GetDescriptor() { return m_Descriptor; }

        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::string m_FilePath;
            VkImage m_Image;
            VkDeviceMemory m_ImageMemory;
            VkImageView m_ImageView;
            u32 m_Width;
            u32 m_Height;
            VkFormat m_Format;
            VulkanSampler2D m_Sampler;
            VkDescriptorImageInfo m_Descriptor;            
    };

    struct VulkanTexture2D {
        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;
        u32 Width;
        u32 Height;
        VkFormat Format;
        VulkanSampler2D Sampler;
        VkDescriptorImageInfo Descriptor;
    };

    VulkanTexture2D vulkan_create_texture_2D(const std::shared_ptr<GraphicsDevice> device, const std::string& path);
    VulkanSampler2D vulkan_create_sampler_2D(const std::shared_ptr<GraphicsDevice> device);

    void vulkan_destroy_texture_2D(const std::shared_ptr<GraphicsDevice> device, VulkanTexture2D& texture);
    void vulkan_destroy_sampler_2D(const std::shared_ptr<GraphicsDevice> device, VulkanSampler2D& sampler);
}