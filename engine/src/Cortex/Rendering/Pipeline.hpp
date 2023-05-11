#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.h"

namespace Cortex
{
    struct PipelineConfig
    {
        VkPipelineDynamicStateCreateInfo DynamicState;
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkPipelineViewportStateCreateInfo ViewportState;
        VkPipelineRasterizationStateCreateInfo Rasterizer;
        VkPipelineMultisampleStateCreateInfo Multisampler;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo DepthStencil;

        static PipelineConfig Default();
    };

    class Pipeline
    {
    public:
        static VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);
        static std::vector<char> ReadShader(const std::string& path);
        static VkPipeline CreatePipeline(const VkDevice &device, const std::string& vertPath, const std::string& fragPath, PipelineConfig pipelineConfig, const VkRenderPass& renderpass);

    private:
        
    };
}