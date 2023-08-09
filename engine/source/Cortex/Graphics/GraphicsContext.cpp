#include "Cortex/Graphics/GraphicsContext.hpp"

namespace Cortex {
    static const VulkanSessionConfig defaultVulkanConfig = {
        .ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
            },
        .DeviceExtensions = {
            "VK_KHR_portability_subset", 
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
            }
        };

    constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;
    
    std::unique_ptr<GraphicsContext> GraphicsContext::Create(const std::unique_ptr<Window>& window) {
        return std::make_unique<GraphicsContext>(window);
    }

    GraphicsContext::GraphicsContext(const std::unique_ptr<Window>& window) {
        m_MaxFrameIndex = MAX_FRAMES_IN_FLIGHT;
        m_CurrentFrameIndex = 0;
        m_GraphicsDevice = GraphicsDevice::Create(defaultVulkanConfig, window);
        m_SwapchainSuboptimal = false;
        m_SwapchainSpec = vulkan_create_swapchain_spec(
                                                m_GraphicsDevice->PhysicalDevice, 
                                                m_GraphicsDevice->Device, 
                                                m_GraphicsDevice->Details.MaxMultisamplingCount,
                                                m_GraphicsDevice->Details.DepthFormat,
                                                m_GraphicsDevice->Surface,
                                                (u32)window->GetFramebufferWidth(), (u32)window->GetFramebufferHeight()
                                            );
        m_RenderPass = {vulkan_create_renderpass(m_GraphicsDevice->PhysicalDevice, m_GraphicsDevice->Device, m_SwapchainSpec)};
        m_Swapchain = Swapchain::Create(m_GraphicsDevice, m_SwapchainSpec, m_RenderPass);
        m_FrameResources = vulkan_create_frame_resources(m_GraphicsDevice->Device, m_MaxFrameIndex, m_GraphicsDevice->QueueIndices.Graphics);
    }

    GraphicsContext::~GraphicsContext() {
        vkDeviceWaitIdle(m_GraphicsDevice->Device);
        vulkan_destroy_frame_resources(m_GraphicsDevice->Device, m_FrameResources);
        vkDestroyRenderPass(m_GraphicsDevice->Device, m_RenderPass.Pass, nullptr);
    }

    bool GraphicsContext::BeginFrame(VkCommandBuffer& commandBuffer) {
        VulkanFrameResources frameData = m_FrameResources[m_CurrentFrameIndex];

        VkResult result = m_Swapchain->SwapBuffers(frameData.InFlightFence, frameData.ImageAvailableSemaphore);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            LOG_WARN("Swapchain out of date!");
            m_SwapchainSuboptimal = true;
        } else if (result == VK_SUBOPTIMAL_KHR) {
            LOG_WARN("Swapchain suboptimal!");
            // m_SwapchainSuboptimal = true;
        } else if (result != VK_SUCCESS) {
            LOG_WARN("Couldn't retrieve image from swapchain!");
            m_SwapchainSuboptimal = true;
        }

        if (m_SwapchainSuboptimal) {
            RecreateSwapchain();
            return false;
        }

        vkResetFences(m_GraphicsDevice->Device, 1, &frameData.InFlightFence);
        vkResetCommandBuffer(frameData.CommandBuffer, 0);
    
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        result = vkBeginCommandBuffer(frameData.CommandBuffer, &beginInfo);
        ASSERT(result == VK_SUCCESS, "Failed to begin recording a Vulkan command buffer!");

        commandBuffer = frameData.CommandBuffer;
        return true;
    }

    bool GraphicsContext::BeginRenderPass(VkCommandBuffer commandBuffer) {
        
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.8f, 0.8f, 0.8f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo passBeginInfo = {};
        passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passBeginInfo.renderPass = m_RenderPass.Pass;
        passBeginInfo.framebuffer = m_Swapchain->GetCurrentFramebuffer();
        passBeginInfo.renderArea.offset = {0, 0};
        passBeginInfo.renderArea.extent = m_SwapchainSpec.Extent;
        passBeginInfo.clearValueCount = static_cast<u32>(clearValues.size());
        passBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = m_SwapchainSpec.Extent.width;
        viewport.height = m_SwapchainSpec.Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapchainSpec.Extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        return true;
    }

    bool GraphicsContext::EndRenderPass(VkCommandBuffer commandBuffer) {
        vkCmdEndRenderPass(commandBuffer);
        return true;
    }

    bool GraphicsContext::EndFrame() {
        VulkanFrameResources frameData = m_FrameResources[m_CurrentFrameIndex];

        VkResult result = vkEndCommandBuffer(frameData.CommandBuffer);
        ASSERT(result == VK_SUCCESS, "Failed to finish recording a Vulkan command buffer.");

        VkSemaphore waitSemaphores[] = { frameData.ImageAvailableSemaphore };
        VkSemaphore signalSemaphores[] = { frameData.RenderFinishSemaphore };
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frameData.CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        result = vkQueueSubmit(m_GraphicsDevice->Queues.Graphics, 1, &submitInfo, frameData.InFlightFence);
        ASSERT(result == VK_SUCCESS, "Failed to submit command buffers to Graphics queue!");

        result = m_Swapchain->PresentImage(m_GraphicsDevice->Queues.Present, frameData.RenderFinishSemaphore);
        ASSERT(result == VK_SUCCESS, "Failed to present new swapchain image!");

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFrameIndex;
        return true;
    }
    
    bool GraphicsContext::OnFramebufferResize(i32 width, i32 height) {
        m_SwapchainSuboptimal = true;
        m_SwapchainSpec.Extent.width = width;
        m_SwapchainSpec.Extent.height = height;
        return true;
    }

    bool GraphicsContext::RecreateSwapchain() {
        m_Swapchain = Swapchain::Create(m_GraphicsDevice, m_SwapchainSpec, m_RenderPass);
        m_SwapchainSuboptimal = false;
        return true;
    }

    std::shared_ptr<Model> GraphicsContext::LoadModelFromOBJ(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        ASSERT(ok, "Failed to load OBJ file.");

        std::vector<VulkanVertex> vertices;
        std::vector<VulkanIndex> indices;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                VulkanVertex vert {};

                if (index.vertex_index >= 0) {
                    vert.Position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0) {
                    vert.Normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) {
                    vert.TexCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                vert.Color = {1.0f, 1.0f, 1.0f};

                vertices.push_back(vert);
                indices.push_back(indices.size());
            }
        }

        return std::make_shared<Model>(m_GraphicsDevice, vertices, indices);
    }
}