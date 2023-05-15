#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Rendering/VulkanUtils.hpp"
#include "Cortex/Rendering/RenderContext.hpp"
#include "Cortex/Rendering/Shader.hpp"

namespace Cortex
{
    // TODO: Look into a builder pattern. Could make sense to want to do stuff like pipelineBuilder.AddRenderPass().AddShader().AddShader().Build() ...

    struct PipelineConfig
    {
        std::vector<VkDynamicState> DynamicStates;
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkViewport Viewport;
        VkRect2D Scissor;
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
        Pipeline(const std::unique_ptr<RenderContext>& context, PipelineConfig config, std::shared_ptr<Shader> vert, std::shared_ptr<Shader> frag);
        ~Pipeline();

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

    private:
        VkDevice m_DeviceHandle;
        VkRenderPass m_RenderPassHandle;
        PipelineConfig m_PipelineConfig;
        std::shared_ptr<Shader> m_VertexShader;
        std::shared_ptr<Shader> m_FragmentShader;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;

        void CreatePipelineLayout();
        void CreatePipeline();
        
    };
}