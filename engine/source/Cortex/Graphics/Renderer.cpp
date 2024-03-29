#include "Cortex/Graphics/Renderer.hpp"

namespace Cortex {
    std::unique_ptr<Renderer> Renderer::Create(const std::unique_ptr<GraphicsContext>& context) {
        return std::make_unique<Renderer>(context);
    }

    Renderer::Renderer(const std::unique_ptr<GraphicsContext>& context) {
        m_GraphicsDevice = context->GetDevice();
        m_CurrentFrameIndex = 0;

        m_ShaderLibrary = ShaderLibrary::Create(m_GraphicsDevice);
        auto shader = m_ShaderLibrary->Load("basic", "../../testbed/assets/shaders/basic.vert", "../../testbed/assets/shaders/basic.frag");
        m_Texture = Texture2D::Create(m_GraphicsDevice, "../../testbed/assets/models/viking/viking_room.png");

        m_UniformBuffers = vulkan_create_uniform_buffers(m_GraphicsDevice, MAX_FRAMES_IN_FLIGHT);
        m_MaterialDescriptorSets = vulkan_create_descriptor_sets(m_GraphicsDevice->Device, shader->m_DescriptorPool, MAX_FRAMES_IN_FLIGHT, shader->m_DescriptorSetLayouts[0], m_Texture, m_UniformBuffers);

        auto pipelineConfig = VulkanPipelineConfig::Default();
        pipelineConfig.RenderPass = context->GetRenderPass().Pass;
        m_Pipeline = Pipeline::Create(m_GraphicsDevice, m_ShaderLibrary->Get("basic"), pipelineConfig);
    }

    Renderer::~Renderer() {
        vkDeviceWaitIdle(m_GraphicsDevice->Device);
        vkDestroyDescriptorPool(m_GraphicsDevice->Device, m_MaterialDescriptorPool, nullptr);
    }

    void Renderer::DrawScene(VkCommandBuffer commandBuffer, const Scene& scene) {

        m_Pipeline->Bind(commandBuffer);
        
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetLayout(), 0, 1, &m_MaterialDescriptorSets[m_CurrentFrameIndex], 0, nullptr);

        for (auto& e : scene.Entities) {
            // VulkanPushData push;
            // push.ModelMatrix = scene.MainCamera.ProjectionMatrix * scene.MainCamera.ViewMatrix * e.Transform.ModelMatrix;
            // vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanPushData), &push);
            
            VulkanCameraUniformData cameraData;
            cameraData.ModelToWorldSpace = e.Transform.ModelMatrix;
            cameraData.WorldToClipSpace = scene.MainCamera.ProjectionMatrix * scene.MainCamera.ViewMatrix;
            memcpy(m_UniformBuffers[m_CurrentFrameIndex].UniformBufferMapped, &cameraData, sizeof(cameraData));
            
            e.Mesh.Model->Bind(commandBuffer);
            e.Mesh.Model->Draw(commandBuffer);
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    VkDescriptorSetLayout vulkan_create_descriptor_set_layout(VkDevice device) {
        VkDescriptorSetLayoutBinding cameraUniformsLayoutBinding = {};
        cameraUniformsLayoutBinding.binding = 0;
        cameraUniformsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraUniformsLayoutBinding.descriptorCount = 1;
        cameraUniformsLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        cameraUniformsLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {cameraUniformsLayoutBinding, samplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = 2;
        createInfo.pBindings = bindings.data();

        VkDescriptorSetLayout layout;
        VkResult result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &layout);
        ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout.");
        return layout;
    }

    VkDescriptorPool vulkan_create_descriptor_pool(VkDevice device) {
        VkDescriptorPoolSize samplerSize = {};
        samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolSize uniformSize = {};
        uniformSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        std::array<VkDescriptorPoolSize, 2> sizes = {uniformSize, samplerSize};
    
        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.poolSizeCount = 2;
        createInfo.pPoolSizes = sizes.data();
        createInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPool pool;
        VkResult result = vkCreateDescriptorPool(device, &createInfo, nullptr, &pool);
        ASSERT(result == VK_SUCCESS, "Failed to create a Descriptor pool!");

        return pool;
    }

    std::vector<VkDescriptorSet> vulkan_create_descriptor_sets(VkDevice device, VkDescriptorPool pool, u32 count, VkDescriptorSetLayout layout, std::shared_ptr<Texture2D> tex, const std::vector<VulkanUniformBuffer>& buffers) {
        
        std::vector<VkDescriptorSet> sets(count);
        
        std::vector<VkDescriptorSetLayout> layouts(count);
        for (u32 i = 0; i < count; i++) {
            layouts[i] = layout;
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = count;
        allocInfo.pSetLayouts = layouts.data();

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo, sets.data());
        ASSERT(result == VK_SUCCESS, "Failed to allocate Descriptor set!");

        for (u32 i = 0; i < count; i++) {
            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = sets[i];
            write.dstBinding = 1;
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = &tex->GetDescriptor();
            
            vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = buffers[i].UniformBuffer;
            bufferInfo.offset = 0;
            bufferInfo.range = buffers[i].Size; 

            VkWriteDescriptorSet ubo_write = {};
            ubo_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            ubo_write.dstSet = sets[i];
            ubo_write.dstBinding = 0;
            ubo_write.dstArrayElement = 0;
            ubo_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_write.descriptorCount = 1;
            ubo_write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device, 1, &ubo_write, 0, nullptr);
        }

        return sets;
    }
}