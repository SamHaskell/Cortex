#include "Cortex/Graphics/Shader.hpp"

namespace Cortex {
    std::shared_ptr<Shader> Shader::Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {
        return std::make_shared<Shader>(device, vertPath, fragPath);
    }

    Shader::Shader(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {
        m_GraphicsDevice = device;
        
        m_VertPath = vertPath;
        m_FragPath = fragPath;

        auto vertCode = vulkan_compile_from_source(vertPath, ShaderType::VERTEX);
        auto fragCode = vulkan_compile_from_source(fragPath, ShaderType::FRAGMENT);

        m_VertexShaderModule = vulkan_create_shader_module(m_GraphicsDevice->Device, vertCode);
        m_FragmentShaderModule = vulkan_create_shader_module(m_GraphicsDevice->Device, fragCode);

        m_ShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStageCreateInfos[0].module = m_VertexShaderModule;
        m_ShaderStageCreateInfos[0].pName = "main";

        m_ShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStageCreateInfos[1].module = m_FragmentShaderModule;
        m_ShaderStageCreateInfos[1].pName = "main";

        // Do dome reflection here?
    }

    Shader::~Shader() {
        vkDestroyShaderModule(m_GraphicsDevice->Device, m_VertexShaderModule, nullptr);
        vkDestroyShaderModule(m_GraphicsDevice->Device, m_FragmentShaderModule, nullptr);
    }

    void Shader::Compile() {

    }

    void Shader::DebugPrint() {
        LOG_DEBUG("%s", m_VertPath.c_str());
        LOG_DEBUG("%s", m_FragPath.c_str());
    }

    std::shared_ptr<ShaderLibrary> ShaderLibrary::Create(std::shared_ptr<GraphicsDevice> device) {
        return std::make_shared<ShaderLibrary>(device);
    }

    ShaderLibrary::ShaderLibrary(std::shared_ptr<GraphicsDevice> device) {
        m_GraphicsDevice = device;
    }

    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader> shader) {
        ASSERT(m_ShaderLookup.find(name) != m_ShaderLookup.end(), "A Shader with the same name already exists.");
        m_ShaderLookup[name] = shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
        auto shader = Shader::Create(m_GraphicsDevice, vertPath, fragPath);
        ASSERT(m_ShaderLookup.find(name) == m_ShaderLookup.end(), "A Shader with the same name already exists.");
        m_ShaderLookup[name] = shader;
        return shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
        ASSERT(m_ShaderLookup.find(name) != m_ShaderLookup.end(), "Failed shader lookup: Shader does not exist.")
        return m_ShaderLookup[name];
    }
}