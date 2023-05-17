#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/RenderDevice.hpp"

namespace Cortex
{
    class Shader
    {
    public:
        Shader(const std::shared_ptr<RenderDevice>& device, const std::string &path, VkShaderStageFlagBits stage);
        ~Shader();

        void Load();
        inline const VkPipelineShaderStageCreateInfo& GetShaderStageCreateInfo() { return m_ShaderCreateInfo; }

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

        std::vector<char> ReadShader(const std::string &path);
    };
}