#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Graphics/GraphicsContext.hpp"
#include "Badger/Graphics/Pipeline.hpp"

#include "Badger/Entities/Entity.hpp"

namespace Badger {
    class Renderer {
        public:
            static std::unique_ptr<Renderer> Create(const std::unique_ptr<GraphicsContext>& context);
            Renderer(const std::unique_ptr<GraphicsContext>& context);
            ~Renderer();
            Renderer(const Renderer&) = delete;
            Renderer &operator=(const Renderer&) = delete;

            void DrawEntities(std::vector<Entity> entities);
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VkPipelineLayout m_PipelineLayout;
            std::shared_ptr<Pipeline> m_Pipeline;

    };
}