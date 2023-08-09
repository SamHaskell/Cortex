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
            void Compile();
            void DebugPrint();
            inline VkPipelineShaderStageCreateInfo* GetShaderStageCreateInfos() { return m_ShaderStageCreateInfos; }

        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::string m_VertPath;
            std::string m_FragPath;
            VkShaderModule m_VertexShaderModule;
            VkShaderModule m_FragmentShaderModule;
            VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfos[2];
    };

    class ShaderLibrary {
        public:
            static std::shared_ptr<ShaderLibrary> Create(std::shared_ptr<GraphicsDevice> device);
            ShaderLibrary(std::shared_ptr<GraphicsDevice> device);
            void Add(const std::string& name, const std::shared_ptr<Shader> shader);
            std::shared_ptr<Shader> Load(const std::string& name, const std::string& vertPath, const std::string& fragPath);
            std::shared_ptr<Shader> Get(const std::string& name);

        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderLookup;

    };
}