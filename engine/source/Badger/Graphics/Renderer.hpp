#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

#include "Badger/Graphics/GraphicsContext.hpp"

namespace Badger {
    class Renderer {
        public:
            static std::unique_ptr<Renderer> Create(const std::unique_ptr<GraphicsContext>& context);
            Renderer(const std::unique_ptr<GraphicsContext>& context);
            ~Renderer();
            Renderer(const Renderer&) = delete;
            Renderer &operator=(const Renderer&) = delete;
        private:
    };
}