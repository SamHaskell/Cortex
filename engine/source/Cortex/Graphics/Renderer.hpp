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
            u32 m_CurrentFrameIndex;
            std::shared_ptr<ShaderLibrary> m_ShaderLibrary;
            VkDescriptorSetLayout m_MaterialDescriptorSetLayout;
            std::vector<VkDescriptorSet> m_MaterialDescriptorSets;
            VkDescriptorPool m_MaterialDescriptorPool;
            VkPipelineLayout m_PipelineLayout;
            std::shared_ptr<Pipeline> m_Pipeline;
            std::shared_ptr<Texture2D> m_Texture;
            std::vector<VulkanUniformBuffer> m_UniformBuffers;
    };

    VkDescriptorSetLayout vulkan_create_descriptor_set_layout(VkDevice device);
    VkDescriptorPool vulkan_create_descriptor_pool(VkDevice device);
    std::vector<VkDescriptorSet> vulkan_create_descriptor_sets(VkDevice device, VkDescriptorPool pool, u32 count, VkDescriptorSetLayout layout, std::shared_ptr<Texture2D> tex, const std::vector<VulkanUniformBuffer>& buffers);

}