#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

namespace Cortex {
    class Shader {
        public:
            static std::shared_ptr<Shader> Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath);
            Shader(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath);
            ~Shader();
            Shader(const Shader&) = delete;
            Shader &operator=(const Shader&) = delete;
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VkShaderModule m_VertexShaderModule;
            VkShaderModule m_FragmentShaderModule;        

    };

    class ShaderLibrary {
        public:

        private:
            std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderLookup;

    };
}