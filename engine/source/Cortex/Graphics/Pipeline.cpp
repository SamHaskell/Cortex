#include "Cortex/Graphics/Pipeline.hpp"

namespace Cortex {
    VulkanPipelineConfig VulkanPipelineConfig::Default() {
        VulkanPipelineConfig config;

        config.DynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        config.Viewport = {};
        config.Viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config.Viewport.scissorCount = 1;
        config.Viewport.viewportCount = 1;
        config.Viewport.pScissors = nullptr;
        config.Viewport.pViewports = nullptr;

        config.InputAssembly = {};
        config.InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.InputAssembly.primitiveRestartEnable = VK_FALSE;

        config.Rasterizer = {};
        config.Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.Rasterizer.depthClampEnable = VK_FALSE; // Requires a GPU feature
        config.Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        config.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Others require GPU feature
        config.Rasterizer.lineWidth = 1.0f; // Changing required widelines GPU feature
        config.Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        config.Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        config.Rasterizer.depthBiasEnable = VK_FALSE;
        config.Rasterizer.depthBiasConstantFactor = 0.0f;
        config.Rasterizer.depthBiasClamp = 0.0f;
        config.Rasterizer.depthBiasSlopeFactor = 0.0f;

        config.Multisampler = {};
        config.Multisampler.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.Multisampler.sampleShadingEnable = VK_FALSE;
        config.Multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        config.DepthStencil = {};
        config.DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.DepthStencil.depthTestEnable = VK_TRUE;
        config.DepthStencil.depthWriteEnable = VK_TRUE;
        config.DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        config.DepthStencil.depthBoundsTestEnable = VK_FALSE;
        config.DepthStencil.stencilTestEnable = VK_FALSE;

        config.ColorBlendAttachment = {};
        config.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.ColorBlendAttachment.blendEnable = VK_TRUE;
        config.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        config.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        config.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        config.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        config.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        config.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        return config;
    }

    std::shared_ptr<Pipeline> Pipeline::Create(std::shared_ptr<GraphicsDevice> device, std::shared_ptr<Shader> shader, VulkanPipelineConfig& config) {
        return std::make_shared<Pipeline>(device, shader, config);
    }
    
    Pipeline::Pipeline(std::shared_ptr<GraphicsDevice> device, std::shared_ptr<Shader> shader, VulkanPipelineConfig& config) {
        m_GraphicsDevice = device;
        m_Shader = shader;

        ASSERT(config.RenderPass != VK_NULL_HANDLE, "Cannot create graphics pipeline without a valid RenderPass"); 

        auto bindings = VulkanVertex::BindingDescriptions();
        auto attributes = VulkanVertex::AttributeDescriptions();

        config.Multisampler.rasterizationSamples = device->Details.MaxMultisamplingCount;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributes.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<u32>(bindings.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
        vertexInputInfo.pVertexBindingDescriptions = bindings.data();
        
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<u32>(config.DynamicStates.size());
        dynamicStateInfo.pDynamicStates = config.DynamicStates.data();

        VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &config.ColorBlendAttachment;

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        
        createInfo.stageCount = 2;
        createInfo.pStages = m_Shader->GetShaderStageCreateInfos().data();

        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pViewportState = &config.Viewport;
        createInfo.pInputAssemblyState = &config.InputAssembly;
        createInfo.pRasterizationState = &config.Rasterizer;
        createInfo.pMultisampleState = &config.Multisampler;
        createInfo.pDepthStencilState = &config.DepthStencil;
        createInfo.pDynamicState = &dynamicStateInfo;
        createInfo.pColorBlendState = &colorBlendInfo;

        m_PipelineLayout = shader->GetPipelineLayout();
        createInfo.layout = m_PipelineLayout;

        createInfo.renderPass = config.RenderPass;
        createInfo.subpass = 0;

        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;
        
        VkResult result = vkCreateGraphicsPipelines(m_GraphicsDevice->Device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_PipelineHandle);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan graphics pipeline!");
    }
    
    Pipeline::~Pipeline() {
        vkDestroyPipeline(m_GraphicsDevice->Device, m_PipelineHandle, nullptr);
    }
    
    void Pipeline::Bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineHandle);
    }
}