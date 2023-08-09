#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/VulkanImages.hpp"
#include "Cortex/Graphics/GraphicsContext.hpp"
#include "Cortex/Graphics/Pipeline.hpp"

#include "Cortex/Core/Scene.hpp"

namespace Cortex {
    class Renderer {
        public:
            static std::unique_ptr<Renderer> Create(const std::unique_ptr<GraphicsContext>& context);
            Renderer(const std::unique_ptr<GraphicsContext>& context);
            ~Renderer();
            Renderer(const Renderer&) = delete;
            Renderer &operator=(const Renderer&) = delete;
            void DrawScene(VkCommandBuffer commandBuffer, const Scene& scene);
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::shared_ptr<ShaderLibrary> m_ShaderLibrary;
            VkDescriptorSetLayout m_MaterialDescriptorSetLayout;
            VkDescriptorSet m_MaterialDescriptorSet;
            VkDescriptorPool m_MaterialDescriptorPool;
            VkPipelineLayout m_PipelineLayout;
            std::shared_ptr<Pipeline> m_Pipeline;
            VulkanTexture2D m_Texture;
            VulkanSampler2D m_Sampler;
    };

    VkDescriptorSetLayout vulkan_create_descriptor_set_layout(VkDevice device);
    VkDescriptorPool vulkan_create_descriptor_pool(VkDevice device);
    VkDescriptorSet vulkan_create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout& layout, VkSampler sampler, VkImageView imageView);

}