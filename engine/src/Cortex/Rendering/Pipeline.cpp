#include "Cortex/Rendering/Pipeline.hpp"

#include <fstream>
#include <vector>

namespace Cortex
{
    PipelineConfig PipelineConfig::Default()
    {
        PipelineConfig pipelineConfig = {};

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        pipelineConfig.DynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        pipelineConfig.DynamicState.dynamicStateCount = static_cast<u32>(dynamicStates.size());
        pipelineConfig.DynamicState.pDynamicStates = dynamicStates.data();

        pipelineConfig.InputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        pipelineConfig.InputAssembly.primitiveRestartEnable = VK_FALSE;
        pipelineConfig.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        pipelineConfig.ViewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        pipelineConfig.ViewportState.viewportCount = 1;
        pipelineConfig.ViewportState.scissorCount = 1;

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

    VkShaderModule Pipeline::CreateShaderModule(const VkDevice &device, const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const u32 *>(code.data());

        VkShaderModule shader;
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shader);
        CX_ASSERT_MSG(result == VK_SUCCESS, "Failed to create a Vulkan Shader Module");
        return shader;
    }

    std::vector<char> Pipeline::ReadShader(const std::string &path)
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

    VkPipeline Pipeline::CreatePipeline(const VkDevice &device, const std::string& vertPath, const std::string& fragPath, PipelineConfig pipelineConfig, const VkRenderPass& renderpass) {
        auto vertCode = ReadShader(vertPath);
        auto fragCode = ReadShader(fragPath);

        CX_INFO("Vertex Shader Size: %u", vertCode.size());
    }
}