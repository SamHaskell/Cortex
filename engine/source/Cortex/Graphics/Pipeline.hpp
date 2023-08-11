#pragma once

#include "Cortex/Graphics/VulkanHelpers.hpp"
#include "Cortex/Graphics/VulkanTypes.hpp"
#include "Cortex/Graphics/GraphicsDevice.hpp"

#include "Cortex/Graphics/Shader.hpp"

namespace Cortex {
    struct VulkanPipelineConfig {
        std::vector<VkDynamicState> DynamicStates;
        VkPipelineViewportStateCreateInfo Viewport;
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkPipelineRasterizationStateCreateInfo Rasterizer;
        VkPipelineMultisampleStateCreateInfo Multisampler;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo DepthStencil;
        VkRenderPass RenderPass = nullptr;
        u32 SubpassIndex = 0;
        static VulkanPipelineConfig Default();
    };

    class Pipeline {
        public:
            static std::shared_ptr<Pipeline> Create(std::shared_ptr<GraphicsDevice> device, std::shared_ptr<Shader> shader, VulkanPipelineConfig& config);
            Pipeline(std::shared_ptr<GraphicsDevice> device, std::shared_ptr<Shader> shader, VulkanPipelineConfig& config);
            ~Pipeline();
            Pipeline(const Pipeline&) = delete;
            Pipeline &operator=(const Pipeline&) = delete;
            void Bind(VkCommandBuffer commandBuffer);
            inline VkPipelineLayout GetLayout() { return m_PipelineLayout; }
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            std::shared_ptr<Shader> m_Shader;
            VkPipeline m_PipelineHandle;
            VkPipelineLayout m_PipelineLayout;
    };
}