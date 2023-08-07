#include "Badger/Graphics/VulkanHelpers.hpp"

namespace Badger {

    // MISC RESOURCE CREATION

    VulkanSwapchainSpecification vulkan_create_swapchain_spec(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, u32 width, u32 height) {
        VulkanSwapchainProperties properties = vulkan_query_swapchain_properties(physicalDevice, surface);
        u32 imageCount = properties.Capabilities.minImageCount + 1;
        if (properties.Capabilities.maxImageCount > 0 && imageCount > properties.Capabilities.maxImageCount)
        {
            imageCount = properties.Capabilities.maxImageCount;
        }

        VulkanSwapchainSpecification config;
        config.SurfaceFormat = vulkan_choose_surface_format(properties.Formats);
        config.PresentMode = vulkan_choose_present_mode(properties.PresentModes);
        config.Extent = vulkan_choose_extent(properties.Capabilities, width, height);
        config.ImageCount = imageCount;
        config.CurrentTransform = properties.Capabilities.currentTransform;

        return config;
    }

    VkRenderPass vulkan_create_renderpass(VkDevice device, const VulkanSwapchainSpecification& config) {
        VkRenderPass renderpass;
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = config.SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependancy = {};
        dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependancy.dstSubpass = 0;
        dependancy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependancy.srcAccessMask = 0;
        dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderpassCreateInfo = {};
        renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassCreateInfo.attachmentCount = 1;
        renderpassCreateInfo.pAttachments = &colorAttachment;
        renderpassCreateInfo.subpassCount = 1;
        renderpassCreateInfo.pSubpasses = &subpass;
        renderpassCreateInfo.dependencyCount = 1;
        renderpassCreateInfo.pDependencies = &dependancy;

        VkResult result = vkCreateRenderPass(device, &renderpassCreateInfo, nullptr, &renderpass);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan renderpass!");

        return renderpass;
    }
    
    VkPipelineLayout vulkan_create_pipeline_layout(VkDevice device) {
        VkPushConstantRange pushRange = {};
        pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushRange.size = sizeof(VulkanPushData);

        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushRange;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pSetLayouts = nullptr;

        VkPipelineLayout layout;
        VkResult result = vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout);
        ASSERT(result == VK_SUCCESS, "Could not create a Vulkan pipeline layout!");
        return layout;
    }
    
    VkCommandBuffer vulkan_create_command_buffer(VkDevice device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        VkCommandBuffer commandbuffer;
        VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &commandbuffer);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan command buffer.");
        return commandbuffer;
    }
    
    VkCommandPool vulkan_create_command_pool(VkDevice device, u32 queueIndex, VkCommandPoolCreateFlags flags) {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = flags;
        createInfo.queueFamilyIndex = queueIndex;
        VkCommandPool pool;
        VkResult result = vkCreateCommandPool(device, &createInfo, nullptr, &pool);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan command pool!");
        return pool;
    }

    // DEVICE CREATION

    bool vulkan_check_layer_support(const std::vector<const char *> layers) {
        u32 availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());
        for (const char* name : layers) {
            bool found = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(name, layerProperties.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) { return false; }
        }
        return true;
    }

    bool vulkan_check_device_extension_support(VkPhysicalDevice physicalDevice, const std::vector<const char*> extensions) {
        u32 extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
        for (const char* extension : extensions) {
            bool found = false;
            for (VkExtensionProperties properties : availableExtensions) {
                if (strcmp(properties.extensionName, extension) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) { std::cout << "Device does not support a requested extension!"; return false; }
        }
        return true;
    }
    
    bool vulkan_evaluate_physical_device(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements requirements, i32 &score) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        if (!vulkan_check_device_extension_support(physicalDevice, requirements.DeviceExtensions)) {
            return false;
        }

        VulkanSwapchainProperties swapchainProperties = vulkan_query_swapchain_properties(physicalDevice, surface);
        if (swapchainProperties.Formats.empty() || swapchainProperties.PresentModes.empty()) {
            return false;
        }

        if (requirements.DiscreteGPU && (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {
            std::cout << "Device does not have required feature: Discrete GPU\n";
            return false;
        } else if (requirements.GeometryShader && !deviceFeatures.geometryShader) {
            std::cout << "Device does not have required feature: Geometry Shader\n";
            return false;
        } else if (requirements.TessellationShader && !deviceFeatures.tessellationShader) {
            std::cout << "Device does not have required feature: Tessellation Shader\n";
            return false;
        }

        VulkanQueueIndices indices = vulkan_find_queue_indices(physicalDevice, surface);
        if (indices.Graphics == VULKAN_QUEUE_NOT_FOUND_INDEX) {
            std::cout << "Device does not have a Graphics Queue\n";
            return false;
        } else if (indices.Transfer == VULKAN_QUEUE_NOT_FOUND_INDEX) {
            std::cout << "Device does not have a Transfer Queue\n";
            return false;
        } else if ((indices.Compute == VULKAN_QUEUE_NOT_FOUND_INDEX) && (requirements.ComputeQueue)) {
            std::cout << "Device does not have the requested Compute Queue\n";
            return false;
        }

        // TODO: Implement some scoring regime for different GPU's
        score = 1;

        return true;
    }
    
    VkPhysicalDevice vulkan_choose_best_physical_device(std::vector<VkPhysicalDevice> physicalDevices, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements requirements) {
        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
        i32 bestScore = 0;
        for (auto device : physicalDevices) {
            i32 score;
            bool suitable = vulkan_evaluate_physical_device(device, surface, requirements, score);
            if (!suitable) { continue; }
            if (score > bestScore) {
                bestDevice = device;
                bestScore = score;
            }
        }
        return bestDevice;
    }
    
    VulkanQueueIndices vulkan_find_queue_indices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        VulkanQueueIndices indices;
        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        i32 i = 0;
        for (const auto& family : queueFamilies) {
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && indices.Graphics == VULKAN_QUEUE_NOT_FOUND_INDEX) {
                indices.Graphics = i;
            }
            if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) && indices.Compute == VULKAN_QUEUE_NOT_FOUND_INDEX) {
                indices.Compute = i;
            }
            if ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) && indices.Transfer == VULKAN_QUEUE_NOT_FOUND_INDEX) {
                indices.Transfer = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport && (indices.Present == VULKAN_QUEUE_NOT_FOUND_INDEX)) {
                indices.Present = i;
            }
            i++;
        }
        return indices;
    }
    
    VulkanSwapchainProperties vulkan_query_swapchain_properties(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        VulkanSwapchainProperties properties;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &properties.Capabilities);
        
        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            properties.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, properties.Formats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            properties.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, properties.PresentModes.data());
        }

        return properties;
    }
    
    VkSurfaceFormatKHR vulkan_choose_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& format : availableFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
            if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }
        LOG_DEBUG("Falling back to first found Swapchain Format");
        return availableFormats[0];
    }
    
    VkPresentModeKHR vulkan_choose_present_mode(const std::vector<VkPresentModeKHR>& availableModes) {
        for (const auto& mode : availableModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
            LOG_DEBUG("%s", string_VkPresentModeKHR(mode));
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D vulkan_choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height) {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) { return capabilities.currentExtent; }
        VkExtent2D trueExtent = {
            width, height
        };
        trueExtent.width = std::clamp(trueExtent.width, capabilities.maxImageExtent.width, capabilities.maxImageExtent.width);
        trueExtent.height = std::clamp(trueExtent.height, capabilities.maxImageExtent.height, capabilities.maxImageExtent.height);
        return trueExtent;
    }

    void vulkan_create_instance(std::vector<const char*> validationLayers, VkInstance& outInstance) {
        ASSERT(vulkan_check_layer_support(validationLayers), "Detected Vulkan implentation does not support requested validation layers.");

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Vulkan Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        u32 glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char *> requiredExtensions;
        for (u32 i = 0; i < glfwExtensionCount; i++)
        {
            requiredExtensions.emplace_back(glfwExtensions[i]);
        }
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
        instanceCreateInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &outInstance);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan instance!");
    }
    
    void vulkan_create_surface(VkInstance instance, GLFWwindow* window, VkSurfaceKHR& outSurface) {
        VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &outSurface);
        ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface object!");
    }
    
    void vulkan_obtain_physical_device(VkInstance instance, VkSurfaceKHR surface, VulkanPhysicalDeviceRequirements deviceRequirements, VkPhysicalDevice& outPhysicalDevice) {
        VkPhysicalDevice physicalDevice;

        u32 availablePhysicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &availablePhysicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> availablePhysicalDevices(availablePhysicalDeviceCount);
        vkEnumeratePhysicalDevices(instance, &availablePhysicalDeviceCount, availablePhysicalDevices.data());
        ASSERT(availablePhysicalDeviceCount > 0, "Failed to find any Vulkan physical devices!");

        outPhysicalDevice = vulkan_choose_best_physical_device(availablePhysicalDevices, surface, deviceRequirements);
        ASSERT(outPhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan physical device!");
    }
    
    void vulkan_create_device(VkInstance instance, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions, VkDevice& outDevice, VulkanQueueIndices& outQueueIndices, VulkanQueues& outQueues) {
        outQueueIndices = vulkan_find_queue_indices(physicalDevice, surface);
        f32 queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueIndices = {outQueueIndices.Graphics, outQueueIndices.Present, outQueueIndices.Transfer, outQueueIndices.Compute};

        for (u32 index : uniqueIndices)
        {
            if (index == VULKAN_QUEUE_NOT_FOUND_INDEX)
            {
                continue;
            }
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = index;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<u32>(deviceExtensions.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &outDevice);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan device!");

        if (outQueueIndices.Graphics != VULKAN_QUEUE_NOT_FOUND_INDEX)
            vkGetDeviceQueue(outDevice, outQueueIndices.Graphics, 0, &outQueues.Graphics);
        if (outQueueIndices.Present != VULKAN_QUEUE_NOT_FOUND_INDEX)
            vkGetDeviceQueue(outDevice, outQueueIndices.Present, 0, &outQueues.Present);
        if (outQueueIndices.Transfer != VULKAN_QUEUE_NOT_FOUND_INDEX)
            vkGetDeviceQueue(outDevice, outQueueIndices.Transfer, 0, &outQueues.Transfer);
        if (outQueueIndices.Present != VULKAN_QUEUE_NOT_FOUND_INDEX)
            vkGetDeviceQueue(outDevice, outQueueIndices.Compute, 0, &outQueues.Compute);
    }

    // CONTEXT CREATION

    std::vector<VulkanFrameResources> vulkan_create_frame_resources(VkDevice device, u32 count, u32 queueIndex) {
        std::vector<VulkanFrameResources> resources(count);
        for (u32 i = 0; i < count; i++) {
            resources[i].CommandPool = vulkan_create_command_pool(device, queueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
            resources[i].CommandBuffer = vulkan_create_command_buffer(device, resources[i].CommandPool);
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkFenceCreateInfo fenceInfo = {};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &resources[i].ImageAvailableSemaphore);
            ASSERT(result == VK_SUCCESS, "Failed to create Vulkan semaphore!");
            result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &resources[i].RenderFinishSemaphore);
            ASSERT(result == VK_SUCCESS, "Failed to create Vulkan semaphore!");
            result = vkCreateFence(device, &fenceInfo, nullptr, &resources[i].InFlightFence);
            ASSERT(result == VK_SUCCESS, "Failed to create Vulkan fence!");
        }
        return resources;
    }
    
    void vulkan_destroy_frame_resources(VkDevice device, std::vector<VulkanFrameResources> frameResources) {
        for (auto& data : frameResources) {
            vkDestroyFence(device, data.InFlightFence, nullptr);
            vkDestroySemaphore(device, data.RenderFinishSemaphore, nullptr);
            vkDestroySemaphore(device, data.ImageAvailableSemaphore, nullptr);
            vkDestroyCommandPool(device, data.CommandPool, nullptr);
        }
    }

    // MISC

    VkCommandBuffer vulkan_begin_transient_commands(VkDevice device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void vulkan_end_transient_commands(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    // SWAPCHAIN CREATION

    void vulkan_create_swapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const VulkanSwapchainSpecification &config, VkSwapchainKHR& outSwapchain) {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        VulkanQueueIndices indices = vulkan_find_queue_indices(physicalDevice, surface);

        if (indices.Graphics != indices.Present) {
            u32 queueIndices[] = {indices.Graphics, indices.Present};
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueIndices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = config.ImageCount;
        swapchainCreateInfo.imageFormat = config.SurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = config.SurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = config.Extent;
        swapchainCreateInfo.preTransform = config.CurrentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &outSwapchain);
        ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain!");
    }
    
    void vulkan_get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage>& outImages) {
        u32 imageCount;
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        outImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, outImages.data());
    }
    
    void vulkan_create_swapchain_image_views(VkDevice device, const VulkanSwapchainSpecification &config, const std::vector<VkImage> &images, std::vector<VkImageView>& outImageViews) {
        outImageViews.resize(config.ImageCount);
        for (u32 i = 0; i < config.ImageCount; i++)
        {
            VkImageViewCreateInfo viewCreateInfo = {};
            viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCreateInfo.image = images[i];
            viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewCreateInfo.format = config.SurfaceFormat.format;
            viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCreateInfo.subresourceRange.baseMipLevel = 0;
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseArrayLayer = 0;
            viewCreateInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(device, &viewCreateInfo, nullptr, &outImageViews[i]);
            ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain image views!");
        }
    }
    
    void vulkan_create_swapchain_framebuffers(VkDevice device, VulkanSwapchainSpecification config, std::vector<VkImageView> swapchainImageViews, VkRenderPass renderpass, std::vector<VkFramebuffer>& outFramebuffers) {
        outFramebuffers.resize(config.ImageCount);
        for (u32 i = 0; i < config.ImageCount; i++) {
            VkImageView attachments[] = {
                swapchainImageViews[i]
            };
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderpass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = config.Extent.width;
            framebufferCreateInfo.height = config.Extent.height;
            framebufferCreateInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &outFramebuffers[i]);
            assert(result == VK_SUCCESS);
        }
    }

    // SHADER STUFF

    std::vector<char> vulkan_read_shader_binary(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            LOG_WARN("Failed to read shader binaries from: %s", path.c_str());
            assert(false);
        }
        u32 size = file.tellg();
        std::vector<char> buffer(size);
        file.seekg(0);
        file.read(buffer.data(), size);
        file.close();
        return buffer;
    }

    VkShaderModule vulkan_create_shader_module(VkDevice device, const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const u32*>(code.data());
        VkShaderModule module;
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &module);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan shader module!");
        return module;
    }

    // BUFFER STUFF

    void vulkan_create_buffer(
        VkPhysicalDevice physicalDevice, 
        VkDevice device,
        VkDeviceSize size,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory
    ) {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = size;
        createInfo.usage = usageFlags;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(device, &createInfo, nullptr, &buffer);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan buffer");

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = vulkan_find_memory_type(memoryRequirements.memoryTypeBits, memoryPropertyFlags, physicalDevice);

        result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
        ASSERT(result == VK_SUCCESS, "Failed to allocate device memory for a Vulkan buffer");

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    u32 vulkan_find_memory_type(u32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        for (u32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if (!(typeFilter & (1 << i))) {
                continue;
            } else if ((memoryProperties.memoryTypes[i].propertyFlags & properties) != properties) {
                continue;
            }
            return i;
        }
        ASSERT(false, "Failed to find Vulkan Memory Type");
    }

    // IMAGE STUFF

    void vulkan_create_image(VkDevice device, VkPhysicalDevice physicalDevice, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
        ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan image!");

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = vulkan_find_memory_type(memoryRequirements.memoryTypeBits, properties, physicalDevice);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void vulkan_transition_image_layout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = vulkan_begin_transient_commands(device, commandPool);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(commandBuffer, 0, 0, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        vulkan_end_transient_commands(device, commandPool, queue, commandBuffer);
    }

    void vulkan_copy_buffer_to_image(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer, VkImage image, u32 width, u32 height) {
        VkCommandBuffer commandBuffer = vulkan_begin_transient_commands(device, commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        vulkan_end_transient_commands(device, commandPool, queue, commandBuffer);
    }

}