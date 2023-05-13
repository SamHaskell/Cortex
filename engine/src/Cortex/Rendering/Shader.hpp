#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Rendering/VulkanUtils.h"

namespace Cortex
{
    enum ShaderStage {
        Vertex,
        Fragment,
        Geometry,
        Tessellation,
        Compute,
    };

    class Shader
    {
    public:
        Shader(const std::string& path, ShaderStage stage);
        ~Shader();

        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;

    private:
        ShaderStage m_ShaderStage;
        VkShaderModule m_ShaderModule;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_PipelineLayout;

        VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<char> &code);
        std::vector<char> ReadShader(const std::string &path);
    };
}