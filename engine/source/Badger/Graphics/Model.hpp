#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"
#include "Badger/Graphics/VulkanBuffers.hpp"

#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
    class Model {
        public:
            Model(std::shared_ptr<GraphicsDevice> device, const std::vector<VulkanVertex>& vertices);
            ~Model();
            Model(const Model&) = delete;
            Model &operator=(const Model&) = delete;
            void Bind(VkCommandBuffer commandBuffer);
            void Draw(VkCommandBuffer commandBuffer);
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VulkanVertexBuffer m_VertexBuffer;
    };
}