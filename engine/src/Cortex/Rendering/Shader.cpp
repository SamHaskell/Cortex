#include "Cortex/Rendering/Shader.hpp"

#include <fstream>

namespace Cortex
{
    Shader::Shader(const std::string& path, ShaderStage stage)
    {

    }

    Shader::~Shader()
    {
        
    }

    VkShaderModule Shader::CreateShaderModule(const VkDevice &device, const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const u32 *>(code.data());
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &m_ShaderModule);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Shader Module");
    }

    std::vector<char> Shader::ReadShader(const std::string &path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        CX_ASSERT_MSG(file, "Could not open Shader File");
        size_t size = file.tellg();
        std::vector<char> buff(size);
        file.seekg(0);
        file.read(buff.data(), size);
        file.close();
        return buff;
    }
}