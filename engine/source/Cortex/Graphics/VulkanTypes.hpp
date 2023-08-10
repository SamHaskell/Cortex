#pragma once

#include "Cortex/Base/Base.hpp"

#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

#include "shaderc/shaderc.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "stb_image.h"

#include "tiny_obj_loader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <set>
#include <memory>
#include <numeric>
#include <chrono>
#include <unordered_map>

namespace Cortex {

    #define VULKAN_QUEUE_NOT_FOUND_INDEX std::numeric_limits<u32>::max()

    using VulkanIndex = u32;

    struct VulkanVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Color;
        glm::vec2 TexCoord;
        static std::vector<VkVertexInputBindingDescription> BindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> AttributeDescriptions();
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

    struct VulkanDeviceDetails {
        VkSampleCountFlagBits MaxMultisamplingCount;
        VkFormat DepthFormat;
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
        VkSampleCountFlagBits MultisamplingCount;
        VkFormat DepthFormat;
    };

    struct VulkanDepthResources {
        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;
    };

    struct VulkanColorResources {
        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;    
    };

    struct VulkanFrameResources {
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishSemaphore;
        VkFence InFlightFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

    enum ShaderType {
        VERTEX,
        FRAGMENT
    };

    ////////////////////////////////////////////////////
    // UNIFORMS ////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    struct VulkanPushData {
        glm::mat4 ModelMatrix;
    };

    struct VulkanCameraUniformData {
        alignas(16) glm::mat4 ModelToWorldSpace;
        alignas(16) glm::mat4 WorldToClipSpace;
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
}