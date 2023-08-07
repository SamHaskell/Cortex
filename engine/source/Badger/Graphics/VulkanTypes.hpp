#pragma once

#include "Badger/Base/Base.hpp"

#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "stb_image.h"

#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <memory>
#include <numeric>
#include <chrono>

namespace Badger {

    #define VULKAN_QUEUE_NOT_FOUND_INDEX std::numeric_limits<u32>::max()

    using VulkanIndex = u32;

    struct VulkanVertex {
        glm::vec3 Position;
        glm::vec3 Color;
        static std::vector<VkVertexInputBindingDescription> BindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> AttributeDescriptions();
    };

    struct VulkanPushData {
        glm::mat4 Transform;
    };

    struct VulkanVertex2D {
        glm::vec2 Position;
        glm::vec3 Color;
        static std::vector<VkVertexInputBindingDescription> BindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> AttributeDescriptions();
    };

    struct VulkanPushData2D {
        glm::mat2 Transform;
    };

    struct VulkanQueueIndices {
        u32 Graphics = VULKAN_QUEUE_NOT_FOUND_INDEX;
        u32 Present = VULKAN_QUEUE_NOT_FOUND_INDEX;
        u32 Transfer = VULKAN_QUEUE_NOT_FOUND_INDEX;
        u32 Compute = VULKAN_QUEUE_NOT_FOUND_INDEX;
    };

    struct VulkanQueues {
        VkQueue Graphics = VK_NULL_HANDLE;
        VkQueue Present = VK_NULL_HANDLE;
        VkQueue Transfer = VK_NULL_HANDLE;
        VkQueue Compute = VK_NULL_HANDLE;
    };

    struct VulkanSessionConfig {
        std::vector<const char *> ValidationLayers;
        std::vector<const char *> DeviceExtensions;
    };

    struct VulkanPhysicalDeviceRequirements {
        std::vector<const char*> DeviceExtensions;
        bool DiscreteGPU;
        bool GeometryShader;
        bool TessellationShader;
        bool ComputeQueue;
    };

    struct VulkanSwapchainProperties {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct VulkanSwapchainSpecification {
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
        VkSurfaceTransformFlagBitsKHR CurrentTransform;
        u32 ImageCount;
    };

    struct VulkanDepthResources {

    };

    struct VulkanFrameResources {
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishSemaphore;
        VkFence InFlightFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

}