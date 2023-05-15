#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/VulkanUtils.h"
#include "Cortex/Rendering/RenderContext.hpp"

namespace Cortex
{
    class Shader
    {
    public:
        Shader(const std::unique_ptr<RenderContext> &context, const std::string &path, VkShaderStageFlagBits stage);
        ~Shader();

        void Load();

        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;

    private:
        b8 m_Outdated;
        VkDevice m_DeviceHandle;
        std::string m_FilePath;
        VkShaderStageFlagBits m_ShaderStage;
        VkShaderModule m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_ShaderCreateInfo;

        void CreateShaderModule(const std::vector<char> &code);
        void CreatePipelineLayout();

        std::vector<char> ReadShader(const std::string &path);
    };
}