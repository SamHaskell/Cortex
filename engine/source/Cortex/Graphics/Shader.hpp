#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"

#include "Cortex/Graphics/GraphicsDevice.hpp"

#include "spirv_cross/spirv_cross.hpp"

namespace Cortex {
    class Shader {
        public:
            static std::shared_ptr<Shader> Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath);
            Shader(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath);
            ~Shader();
            Shader(const Shader&) = delete;
            Shader &operator=(const Shader&) = delete;
            void Compile();
            inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos() { return m_ShaderStageCreateInfos; }
            void Reflect();

        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::string m_VertPath;
            std::string m_FragPath;
            std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;
            std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;
            std::unordered_map<VkShaderStageFlagBits, std::vector<u32>> m_ShaderBinaries;
            VkDescriptorSetLayout m_DescriptorSetLayout;
            VkDescriptorPool m_DescriptorPool;
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