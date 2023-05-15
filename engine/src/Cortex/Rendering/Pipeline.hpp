#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.h"
#include "Cortex/Rendering/RenderContext.hpp"
#include "Cortex/Rendering/Shader.hpp"

namespace Cortex
{
    // TODO: Look into a builder pattern. Could make sense to want to do stuff like pipelineBuilder.AddRenderPass().AddShader().AddShader().Build() ...

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
        Pipeline(const std::unique_ptr<RenderContext>& context, std::shared_ptr<Shader> vert, std::shared_ptr<Shader> frag);
        ~Pipeline();

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

    private:
        VkRenderPass m_RenderPassHandle;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;

        std::shared_ptr<Shader> m_VertexShader;
        std::shared_ptr<Shader> m_FragmentShader;
        
    };
}