#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"
#include "Badger/Graphics/GraphicsDevice.hpp"

namespace Badger {
    struct VulkanPipelineConfig {
        std::vector<VkDynamicState> DynamicStates;
        VkPipelineViewportStateCreateInfo Viewport;
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkPipelineRasterizationStateCreateInfo Rasterizer;
        VkPipelineMultisampleStateCreateInfo Multisampler;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo DepthStencil;
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        u32 SubpassIndex = 0;
        static VulkanPipelineConfig Default();
    };

    class Pipeline {
        public:
            static std::shared_ptr<Pipeline> Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath, const VulkanPipelineConfig& config);
            Pipeline(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath, const VulkanPipelineConfig& config);
            ~Pipeline();
            Pipeline(const Pipeline&) = delete;
            Pipeline &operator=(const Pipeline&) = delete;
            void Bind(VkCommandBuffer commandBuffer);
        private:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
            VkShaderModule m_VertexShaderModule;
            VkShaderModule m_FragmentShaderModule;
            VkPipeline m_PipelineHandle;
    };
}