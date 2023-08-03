#pragma once

#include "Badger/Graphics/VulkanTypes.hpp"

namespace Badger {

    // MISC RESOURCE CREATION

    VulkanSwapchainSpecification vulkan_create_swapchain_spec(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, u32 width, u32 height);
    VkRenderPass vulkan_create_renderpass(VkDevice device, const VulkanSwapchainSpecification& config);
    VkPipelineLayout vulkan_create_pipeline_layout(VkDevice device);
    VkCommandBuffer vulkan_create_command_buffer(VkDevice device, VkCommandPool commandPool);
    VkCommandPool vulkan_create_command_pool(VkDevice device, u32 queueIndex, VkCommandPoolCreateFlags flags);

    // DEVICE CREATION

    bool vulkan_check_layer_support(const std::vector<const char *> layers);
    bool vulkan_check_device_extension_support(VkPhysicalDevice physicalDevice, const std::vector<const char*> extensions);
    bool vulkan_evaluate_physical_device(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements requirements, i32 &score);
    VkPhysicalDevice vulkan_choose_best_physical_device(std::vector<VkPhysicalDevice> physicalDevices, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements requirements);
    VulkanQueueIndices vulkan_find_queue_indices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VulkanSwapchainProperties vulkan_query_swapchain_properties(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkSurfaceFormatKHR vulkan_choose_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR vulkan_choose_present_mode(const std::vector<VkPresentModeKHR>& availableModes);
    VkExtent2D vulkan_choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height);

    void vulkan_create_instance(std::vector<const char*> validationLayers, VkInstance& outInstance);
    void vulkan_create_surface(VkInstance instance, GLFWwindow* window, VkSurfaceKHR& outSurface);
    void vulkan_obtain_physical_device(VkInstance instance, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements deviceRequirements, VkPhysicalDevice& outPhysicalDevice);
    void vulkan_create_device(VkInstance instance, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions, VkDevice& outDevice, VulkanQueueIndices& outQueueIndices, VulkanQueues& outQueues);

    // CONTEXT CREATION

    std::vector<VulkanFrameResources> vulkan_create_frame_resources(VkDevice device, u32 count, u32 queueIndex);
    void vulkan_destroy_frame_resources(VkDevice device, std::vector<VulkanFrameResources> frameResources);

    // SWAPCHAIN CREATION

    void vulkan_create_swapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const VulkanSwapchainSpecification &config, VkSwapchainKHR& outSwapchain);
    void vulkan_get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage>& outImages);
    void vulkan_create_swapchain_image_views(VkDevice device, const VulkanSwapchainSpecification &config, const std::vector<VkImage> &images, std::vector<VkImageView>& outImageViews);
    void vulkan_create_swapchain_framebuffers(VkDevice device, VulkanSwapchainSpecification config, std::vector<VkImageView> swapchainImageViews, VkRenderPass renderpass, std::vector<VkFramebuffer>& outFramebuffers);

    // SHADER STUFF

    std::vector<char> vulkan_read_shader_binary(const std::string& path);
    VkShaderModule vulkan_create_shader_module(VkDevice device, const std::vector<char>& code);

    // BUFFER STUFF

    void vulkan_create_buffer(
        VkPhysicalDevice physicalDevice, 
        VkDevice device,
        VkDeviceSize size,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory
    );

    u32 vulkan_find_memory_type(u32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

}