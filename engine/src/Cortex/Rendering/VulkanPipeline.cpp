#include "Cortex/Rendering/VulkanPipeline.hpp"

#include <vector>

namespace Cortex
{
    VulkanPipeline VulkanPipeline::CreateDefaultPipeline()
    {
        VulkanPipelineConfig pipelineConfig = DefaultPipelineConfig();

        VkGraphicsPipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    }

    VulkanPipelineConfig VulkanPipeline::DefaultPipelineConfig()
    {
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        VulkanPipelineConfig pipelineConfig;

        pipelineConfig.dynamicState.sType = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        pipelineConfig.dynamicState.dynamicStateCount = static_cast<u32>(dynamicStates.size());
        pipelineConfig.dynamicState.pDynamicStates = dynamicStates.data();
        pipelineConfig.dynamicState.flags = 0;
        pipelineConfig.dynamicState.pNext = nullptr;

        pipelineConfig.inputAssembly.sType = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        pipelineConfig.inputAssembly.primitiveRestartEnable = VK_FALSE;
        pipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineConfig.inputAssembly.flags = 0;
        pipelineConfig.inputAssembly.pNext = nullptr;

        pipelineConfig.rasterizer.sType = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        pipelineConfig.rasterizer.lineWidth = 1.0;
        pipelineConfig.rasterizer.depthClampEnable = VK_FALSE;
        pipelineConfig.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        pipelineConfig.rasterizer.cullMode = VK_CULL_MODE_NONE;
        pipelineConfig.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        pipelineConfig.rasterizer.depthBiasEnable = VK_FALSE;
        pipelineConfig.rasterizer.depthBiasConstantFactor = 0.0f;
        pipelineConfig.rasterizer.depthBiasClamp = 0.0f;
        pipelineConfig.rasterizer.depthBiasSlopeFactor = 0.0f;
        pipelineConfig.rasterizer.flags = 0;
        pipelineConfig.rasterizer.pNext = nullptr;

        pipelineConfig.multisampler.sType = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        pipelineConfig.multisampler.sampleShadingEnable = VK_FALSE;
        pipelineConfig.multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineConfig.multisampler.minSampleShading = 1.0f;
        pipelineConfig.multisampler.pSampleMask = nullptr;
        pipelineConfig.multisampler.alphaToCoverageEnable = VK_FALSE;
        pipelineConfig.multisampler.alphaToOneEnable = VK_FALSE;
        pipelineConfig.multisampler.flags = 0;
        pipelineConfig.multisampler.pNext = nullptr;

        pipelineConfig.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipelineConfig.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineConfig.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineConfig.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineConfig.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineConfig.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineConfig.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        pipelineConfig.colorBlend.sType = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        pipelineConfig.colorBlend.logicOpEnable = VK_FALSE;
        pipelineConfig.colorBlend.logicOp = VK_LOGIC_OP_COPY;
        pipelineConfig.colorBlend.attachmentCount = 1;
        pipelineConfig.colorBlend.pAttachments = &pipelineConfig.colorBlendAttachment;
        pipelineConfig.colorBlend.blendConstants[0] = 0.0f;
        pipelineConfig.colorBlend.blendConstants[1] = 0.0f;
        pipelineConfig.colorBlend.blendConstants[2] = 0.0f;
        pipelineConfig.colorBlend.blendConstants[3] = 0.0f;
        pipelineConfig.colorBlend.flags = 0;
        pipelineConfig.colorBlend.pNext = nullptr;

        pipelineConfig.depthStencil.sType = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        pipelineConfig.depthStencil.depthTestEnable = VK_TRUE;
        pipelineConfig.depthStencil.depthWriteEnable = VK_TRUE;
        pipelineConfig.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        pipelineConfig.depthStencil.depthBoundsTestEnable = VK_FALSE;
        pipelineConfig.depthStencil.minDepthBounds = 0.0f;
        pipelineConfig.depthStencil.maxDepthBounds = 1.0f;
        pipelineConfig.depthStencil.stencilTestEnable = VK_FALSE;
        pipelineConfig.depthStencil.front = {};
        pipelineConfig.depthStencil.back = {};
        pipelineConfig.depthStencil.flags = 0;
        pipelineConfig.depthStencil.pNext = nullptr;


        return pipelineConfig;
    }
}