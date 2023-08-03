#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

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
            static std::shared_ptr<Pipeline> Create(VkDevice device, const std::string& vertPath, const std::string& fragPath, const VulkanPipelineConfig& config);
            Pipeline(VkDevice device, const std::string& vertPath, const std::string& fragPath, const VulkanPipelineConfig& config);
            ~Pipeline();
            Pipeline(const Pipeline&) = delete;
            Pipeline &operator=(const Pipeline&) = delete;
            void Bind(VkCommandBuffer commandBuffer, VkExtent2D extent);
        private:
            VkDevice m_DeviceHandle;
            VkShaderModule m_VertexShaderModule;
            VkShaderModule m_FragmentShaderModule;
            VkPipeline m_PipelineHandle;
    };
}