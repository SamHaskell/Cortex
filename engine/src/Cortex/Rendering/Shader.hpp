#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/RenderContext.hpp"

namespace Cortex
{
    class Shader
    {
    public:
        Shader(const std::unique_ptr<RenderContext> &context, const std::string &path, VkShaderStageFlagBits stage);
        ~Shader();

        void Load();
        inline const VkPipelineShaderStageCreateInfo& GetShaderStageCreateInfo() { return m_ShaderCreateInfo; }
        inline const VkPipelineVertexInputStateCreateInfo& GetVertexInputCreateInfo() { return m_VertexInputCreateInfo; }

        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;

    private:
        b8 m_Outdated;
        VkDevice m_DeviceHandle;
        std::string m_FilePath;
        VkShaderStageFlagBits m_ShaderStage;
        VkShaderModule m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_ShaderCreateInfo;
        VkPipelineVertexInputStateCreateInfo m_VertexInputCreateInfo;

        void CreateShaderModule(const std::vector<char> &code);

        std::vector<char> ReadShader(const std::string &path);
    };
}