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

        m_ShaderSpec = Reflect();
        CreateDescriptorPool();
        CreateDescriptorSetLayouts();
        CreatePipelineLayout();
        
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
        for (auto& module : m_ShaderModules)
            vkDestroyShaderModule(m_GraphicsDevice->Device, module.second, nullptr);
        for (auto& layout : m_DescriptorSetLayouts)
            vkDestroyDescriptorSetLayout(m_GraphicsDevice->Device, layout.second, nullptr);
    }

    void Shader::Compile() {

    }

    VulkanShaderSpec Shader::Reflect() {
        VulkanShaderSpec spec = {};
        for (auto& stage : m_ShaderBinaries) {
            spirv_cross::Compiler comp(stage.second);
            auto resources = comp.get_shader_resources();

            for (const auto& res : resources.uniform_buffers) {
                auto& type = comp.get_type(res.base_type_id);
                u32 size = comp.get_declared_struct_size(type);
                u32 count = type.member_types.size();
                u32 set = comp.get_decoration(res.id, spv::DecorationDescriptorSet);
                u32 binding = comp.get_decoration(res.id, spv::DecorationBinding);

                VkShaderStageFlags stageFlags = spec.DescriptorSets[set].Descriptors[binding].Stages | stage.first;
                VulkanDescriptorSpec descriptorSpec = {
                        .Name = res.name,
                        .Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .Stages = stageFlags,
                        .Count = count
                    };
                spec.DescriptorSets[set].Descriptors[binding] = descriptorSpec;
                spec.TypeCounts[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] += 1;
            }

            for (const auto& res : resources.sampled_images) {
                auto& type = comp.get_type(res.type_id);
                u32 count = 1;
                u32 set = comp.get_decoration(res.id, spv::DecorationDescriptorSet);
                u32 binding = comp.get_decoration(res.id, spv::DecorationBinding);

                VkShaderStageFlags stageFlags = spec.DescriptorSets[set].Descriptors[binding].Stages | stage.first;
                VulkanDescriptorSpec descriptorSpec = {
                        .Name = res.name,
                        .Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .Stages = stageFlags,
                        .Count = count
                    };
                spec.DescriptorSets[set].Descriptors[binding] = descriptorSpec;
                spec.TypeCounts[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] += 1;
            }

            for (const auto& res : resources.push_constant_buffers) {
                auto& type = comp.get_type(res.base_type_id);
                i32 memberCount = type.member_types.size();
                u32 size = comp.get_declared_struct_size(type);
                spec.PushConstants[stage.first].Name = res.name;
            }
        }
        return spec;
    }

    void Shader::CreateDescriptorPool() {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (auto& type : m_ShaderSpec.TypeCounts) {
            VkDescriptorPoolSize poolSize = {
                .type = type.first,
                .descriptorCount = type.second * MAX_FRAMES_IN_FLIGHT
            };
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<u32>(m_ShaderSpec.DescriptorSets.size()) * MAX_FRAMES_IN_FLIGHT;        
        VkResult result = vkCreateDescriptorPool(m_GraphicsDevice->Device, &poolInfo, nullptr, &m_DescriptorPool);
    }

    void Shader::CreateDescriptorSetLayouts() {
        m_DescriptorSetLayouts.clear();
        for (auto& set : m_ShaderSpec.DescriptorSets) {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            for (auto& descriptor : set.second.Descriptors) {
                VkDescriptorSetLayoutBinding binding = {};
                binding.descriptorCount = 1;
                binding.descriptorType = descriptor.second.Type;
                binding.stageFlags = descriptor.second.Stages;
                binding.binding = descriptor.first;
                bindings.push_back(binding);
            }
            VkDescriptorSetLayoutCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = static_cast<u32>(bindings.size());
            createInfo.pBindings = bindings.data();

            m_DescriptorSetLayouts[set.first] = VK_NULL_HANDLE;
            VkResult result = vkCreateDescriptorSetLayout(m_GraphicsDevice->Device, &createInfo, nullptr, &m_DescriptorSetLayouts[set.first]);
        }
    }

    void Shader::CreatePipelineLayout() {

        // VkPushConstantRange pushRange = {};
        // pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushRange.size = sizeof(VulkanPushData);
        
        std::vector<VkDescriptorSetLayout> setLayouts;
        for (auto& layout : m_DescriptorSetLayouts) {
            setLayouts.push_back(layout.second);            
        }

        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;
        layoutInfo.setLayoutCount = static_cast<u32>(setLayouts.size());
        layoutInfo.pSetLayouts = setLayouts.data();

        VkResult result = vkCreatePipelineLayout(m_GraphicsDevice->Device, &layoutInfo, nullptr, &m_PipelineLayout);
        ASSERT(result == VK_SUCCESS, "Could not create a Vulkan pipeline layout!");        
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