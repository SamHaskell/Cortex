#include "Cortex/Graphics/Renderer.hpp"

namespace Cortex {
    std::unique_ptr<Renderer> Renderer::Create(const std::unique_ptr<GraphicsContext>& context) {
        return std::make_unique<Renderer>(context);
    }

    Renderer::Renderer(const std::unique_ptr<GraphicsContext>& context) {
        m_GraphicsDevice = context->GetDevice();

        m_ShaderLibrary = ShaderLibrary::Create(m_GraphicsDevice);
        m_ShaderLibrary->Load("basic", "../../testbed/assets/shaders/basic.vert", "../../testbed/assets/shaders/basic.frag");
        m_ShaderLibrary->Get("basic")->DebugPrint();

        m_Texture = vulkan_create_texture_2D(m_GraphicsDevice, "../../testbed/assets/models/viking/viking_room.png");
        m_Sampler = vulkan_create_sampler_2D(m_GraphicsDevice);

        m_MaterialDescriptorSetLayout = vulkan_create_descriptor_set_layout(m_GraphicsDevice->Device);
        m_MaterialDescriptorPool = vulkan_create_descriptor_pool(m_GraphicsDevice->Device);
        m_MaterialDescriptorSet = vulkan_create_descriptor_set(m_GraphicsDevice->Device, m_MaterialDescriptorPool, m_MaterialDescriptorSetLayout, m_Sampler.Sampler, m_Texture.ImageView);

        m_PipelineLayout = vulkan_create_pipeline_layout(m_GraphicsDevice->Device, m_MaterialDescriptorSetLayout);
        auto pipelineConfig = VulkanPipelineConfig::Default();
        pipelineConfig.RenderPass = context->GetRenderPass().Pass;
        pipelineConfig.PipelineLayout = m_PipelineLayout;
        m_Pipeline = Pipeline::Create(m_GraphicsDevice, m_ShaderLibrary->Get("basic"), pipelineConfig);

    }

    Renderer::~Renderer() {
        vkDeviceWaitIdle(m_GraphicsDevice->Device);
        vkDestroyPipelineLayout(m_GraphicsDevice->Device, m_PipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_GraphicsDevice->Device, m_MaterialDescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(m_GraphicsDevice->Device, m_MaterialDescriptorPool, nullptr);

        vulkan_destroy_texture_2D(m_GraphicsDevice, m_Texture);
        vulkan_destroy_sampler_2D(m_GraphicsDevice, m_Sampler);
    }

    void Renderer::DrawScene(VkCommandBuffer commandBuffer, const Scene& scene) {

        m_Pipeline->Bind(commandBuffer);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_MaterialDescriptorSet, 0, nullptr);
        for (auto& e : scene.Entities) {
            VulkanPushData push;
            push.Transform = scene.MainCamera.ProjectionMatrix * scene.MainCamera.ViewMatrix * e.Transform.ModelToWorld;
            vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanPushData), &push);
            e.Mesh.Model->Bind(commandBuffer);
            e.Mesh.Model->Draw(commandBuffer);
        }
    }

    VkDescriptorSetLayout vulkan_create_descriptor_set_layout(VkDevice device) {
        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = 1;
        createInfo.pBindings = &samplerLayoutBinding;

        VkDescriptorSetLayout layout;
        VkResult result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &layout);
        ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout.");
        return layout;
    }

    VkDescriptorPool vulkan_create_descriptor_pool(VkDevice device) {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;
    
        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.poolSizeCount = 1;
        createInfo.pPoolSizes = &poolSize;
        createInfo.maxSets = 1;

        VkDescriptorPool pool;
        VkResult result = vkCreateDescriptorPool(device, &createInfo, nullptr, &pool);
        ASSERT(result == VK_SUCCESS, "Failed to create a Descriptor pool!");

        return pool;
    }

    VkDescriptorSet vulkan_create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout& layout, VkSampler sampler, VkImageView imageView) {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet set;
        VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &set);
        ASSERT(result == VK_SUCCESS, "Failed to allocate Descriptor set!");

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = sampler;
        imageInfo.imageView = imageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;
        
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

        return set;
    }
}