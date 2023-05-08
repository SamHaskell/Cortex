#pragma once

#include "Cortex/Utils/Defines.h"

#include "vulkan/vulkan.h"
#include <vector>

namespace Cortex {
    struct VulkanPipelineConfig {
        VkPipelineDynamicStateCreateInfo dynamicState;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampler;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlend;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        u32 subpass = 0;
    };

    class VulkanPipeline {
        public:
            ~VulkanPipeline() {};
            static VulkanPipeline CreateDefaultPipeline();
            static VulkanPipelineConfig DefaultPipelineConfig();
            void CreateShaderModule(const std::vector<char>& code);

        private:
            VulkanPipeline();
            VkShaderModule m_VertShader;
            VkShaderModule m_FragShader;
    };
}