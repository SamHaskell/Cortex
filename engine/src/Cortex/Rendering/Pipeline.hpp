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
        void CreatePipeline(const VkDevice &device, const std::string& vertPath, const std::string& fragPath, PipelineConfig pipelineConfig, const VkRenderPass& renderpass);

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

    private:
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        
    };
}