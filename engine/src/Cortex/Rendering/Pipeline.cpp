#include "Cortex/Rendering/Pipeline.hpp"

#include <vector>

namespace Cortex
{
    PipelineConfig PipelineConfig::Default()
    {
        PipelineConfig pipelineConfig = {};

        std::vector<VkDynamicState> DynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        pipelineConfig.InputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        pipelineConfig.InputAssembly.primitiveRestartEnable = VK_FALSE;
        pipelineConfig.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        pipelineConfig.Viewport = {};
        pipelineConfig.Viewport.width = 100;
        pipelineConfig.Viewport.height = 100;
        pipelineConfig.Scissor = {};

        pipelineConfig.Rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        pipelineConfig.Rasterizer.depthClampEnable = VK_FALSE;
        pipelineConfig.Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        pipelineConfig.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineConfig.Rasterizer.lineWidth = 1.0f;
        pipelineConfig.Rasterizer.cullMode = VK_CULL_MODE_NONE;
        pipelineConfig.Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        pipelineConfig.Rasterizer.depthBiasEnable = VK_FALSE;
        pipelineConfig.Rasterizer.depthBiasConstantFactor = 0.0f;
        pipelineConfig.Rasterizer.depthBiasClamp = 0.0f;
        pipelineConfig.Rasterizer.depthBiasSlopeFactor = 0.0f;

        pipelineConfig.Multisampler = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        pipelineConfig.Multisampler.sampleShadingEnable = VK_FALSE;
        pipelineConfig.Multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineConfig.Multisampler.minSampleShading = 1.0f;
        pipelineConfig.Multisampler.pSampleMask = nullptr;
        pipelineConfig.Multisampler.alphaToCoverageEnable = VK_FALSE;
        pipelineConfig.Multisampler.alphaToOneEnable = VK_FALSE;

        pipelineConfig.ColorBlendAttachment = {};
        pipelineConfig.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipelineConfig.ColorBlendAttachment.blendEnable = VK_TRUE;
        pipelineConfig.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        pipelineConfig.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        pipelineConfig.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineConfig.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineConfig.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineConfig.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        pipelineConfig.DepthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

        return pipelineConfig;
    }

    Pipeline::Pipeline(const std::unique_ptr<RenderContext> &context, std::shared_ptr<Shader> vert, std::shared_ptr<Shader> frag)
        : m_DeviceHandle(context->GetDevice()->GetDevice()),
          m_RenderPassHandle(context->GetSwapchain()->GetRenderPass()),
          m_PipelineConfig(PipelineConfig::Default()),
          m_VertexShader(vert),
          m_FragmentShader(frag)
    {
        CreatePipelineLayout();
        CreatePipeline();
    }

    Pipeline::Pipeline(const std::unique_ptr<RenderContext> &context, PipelineConfig config, std::shared_ptr<Shader> vert, std::shared_ptr<Shader> frag)
        : m_DeviceHandle(context->GetDevice()->GetDevice()),
          m_RenderPassHandle(context->GetSwapchain()->GetRenderPass()),
          m_PipelineConfig(config),
          m_VertexShader(vert),
          m_FragmentShader(frag)
    {
        CreatePipelineLayout();
        CreatePipeline();
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipeline(m_DeviceHandle, m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_DeviceHandle, m_PipelineLayout, nullptr);
    }

    void Pipeline::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        VkResult result = vkCreatePipelineLayout(m_DeviceHandle, &createInfo, nullptr, &m_PipelineLayout);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Pipeline Layout");
    }

    void Pipeline::CreatePipeline()
    {
        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0] = m_VertexShader -> GetShaderStageCreateInfo();
        shaderStages[1] = m_FragmentShader -> GetShaderStageCreateInfo();

        VkPipelineColorBlendStateCreateInfo blendState = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        blendState.attachmentCount = 1;
        blendState.logicOpEnable = VK_FALSE;
        blendState.logicOp = VK_LOGIC_OP_COPY;
        blendState.pAttachments = &m_PipelineConfig.ColorBlendAttachment;

        VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &m_PipelineConfig.Viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &m_PipelineConfig.Scissor;

        VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamicState.dynamicStateCount = static_cast<u32>(m_PipelineConfig.DynamicStates.size());
        dynamicState.pDynamicStates = m_PipelineConfig.DynamicStates.data();
        
        VkGraphicsPipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        
        createInfo.pVertexInputState = &m_VertexShader->GetVertexInputCreateInfo();
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.pColorBlendState = &blendState;        
        createInfo.pViewportState = &viewportState;
        createInfo.pDynamicState = &dynamicState;
        createInfo.pInputAssemblyState = &m_PipelineConfig.InputAssembly;
        createInfo.pRasterizationState = &m_PipelineConfig.Rasterizer;
        createInfo.pMultisampleState = &m_PipelineConfig.Multisampler;
        createInfo.pDepthStencilState = &m_PipelineConfig.DepthStencil;
        
        createInfo.layout = m_PipelineLayout;
        createInfo.renderPass = m_RenderPassHandle;

        VkResult result = vkCreateGraphicsPipelines(m_DeviceHandle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_Pipeline);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Pipeline");
    }
}