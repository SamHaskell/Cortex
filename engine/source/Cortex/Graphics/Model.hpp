#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"
#include "Cortex/Graphics/VulkanBuffers.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
    class Model {
        public:
            Model(std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices, const std::vector<VulkanIndex>& indices);
            ~Model();
            Model(const Model&) = delete;
            Model &operator=(const Model&) = delete;
            void Bind(VkCommandBuffer commandBuffer);
            void Draw(VkCommandBuffer commandBuffer);
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VulkanVertexBuffer m_VertexBuffer;
            VulkanIndexBuffer m_IndexBuffer;
    };
}