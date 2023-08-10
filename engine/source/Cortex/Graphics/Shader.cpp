#include "Cortex/Graphics/Shader.hpp"

namespace Cortex {
    std::shared_ptr<Shader> Shader::Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {
        return std::make_shared<Shader>(device, vertPath, fragPath);
    }

    Shader::Shader(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {
        m_GraphicsDevice = device;
        
        m_VertPath = vertPath;
        m_FragPath = fragPath;

        m_ShaderBinaries[VK_SHADER_STAGE_VERTEX_BIT] = vulkan_compile_from_source(vertPath, ShaderType::VERTEX);
        m_ShaderBinaries[VK_SHADER_STAGE_FRAGMENT_BIT] = vulkan_compile_from_source(fragPath, ShaderType::FRAGMENT);

        Reflect();
        
        m_ShaderModules[VK_SHADER_STAGE_VERTEX_BIT] = vulkan_create_shader_module(m_GraphicsDevice->Device, m_ShaderBinaries[VK_SHADER_STAGE_VERTEX_BIT]);
        m_ShaderModules[VK_SHADER_STAGE_FRAGMENT_BIT] = vulkan_create_shader_module(m_GraphicsDevice->Device, m_ShaderBinaries[VK_SHADER_STAGE_FRAGMENT_BIT]);

        m_ShaderStageCreateInfos.resize(2);

        m_ShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStageCreateInfos[0].module = m_ShaderModules[VK_SHADER_STAGE_VERTEX_BIT];
        m_ShaderStageCreateInfos[0].pName = "main";

        m_ShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStageCreateInfos[1].module = m_ShaderModules[VK_SHADER_STAGE_FRAGMENT_BIT];
        m_ShaderStageCreateInfos[1].pName = "main";
    }

    Shader::~Shader() {
        for (auto& module : m_ShaderModules) {
            vkDestroyShaderModule(m_GraphicsDevice->Device, module.second, nullptr);
        }
    }

    void Shader::Compile() {

    }

    void Shader::Reflect() {
        for (auto& stage : m_ShaderBinaries) {
            LOG_INFO("%s", string_VkShaderStageFlagBits(stage.first));
            spirv_cross::Compiler comp(stage.second);
            auto resources = comp.get_shader_resources();

            LOG_INFO("Uniform Buffers");
            for (const auto& res : resources.uniform_buffers) {
                auto& type = comp.get_type(res.base_type_id);
                i32 memberCount = type.member_types.size();
                u32 binding = comp.get_decoration(res.id, spv::DecorationBinding);
                u32 size = comp.get_declared_struct_size(type);
                const auto& name = res.name;
                LOG_DEBUG("Name: %s, Member Count: %i, Binding: %u, Size: %u", res.name.c_str(), memberCount, binding, size);
            }

            LOG_INFO("Combined Image Samplers");
            for (const auto& res : resources.sampled_images) {
                auto& type = comp.get_type(res.base_type_id);
                i32 memberCount = type.member_types.size();
                u32 binding = comp.get_decoration(res.id, spv::DecorationBinding);
                const auto& name = res.name;
                LOG_DEBUG("Name: %s, Member Count: %i, Binding: %u", res.name.c_str(), memberCount, binding);
            }

            LOG_INFO("Push Constants");
            for (const auto& res : resources.push_constant_buffers) {
                auto& type = comp.get_type(res.base_type_id);
                i32 memberCount = type.member_types.size();
                u32 size = comp.get_declared_struct_size(type);
                const auto& name = res.name;
                LOG_DEBUG("Name: %s, Member Count: %i, Size: %u", res.name.c_str(), memberCount, size);
            }
        }

        // VkDescriptorSetLayoutBinding someLayoutBinding = {};
        // someLayoutBinding.binding = 0;
        // someLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // someLayoutBinding.descriptorCount = 1;
        // someLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        // someLayoutBinding.pImmutableSamplers = nullptr;

        // VkDescriptorSetLayoutCreateInfo createInfo = {};
        // createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        // createInfo.bindingCount = 1;
        // createInfo.pBindings = &someLayoutBinding;

        // VkResult result = vkCreateDescriptorSetLayout(m_GraphicsDevice->Device, &createInfo, nullptr, &m_DescriptorSetLayout);
        // ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout.");
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