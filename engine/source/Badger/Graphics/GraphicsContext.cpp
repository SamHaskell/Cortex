#include "Badger/Graphics/GraphicsContext.hpp"

namespace Badger {
    static const VulkanSessionConfig defaultVulkanConfig = {
        .ValidationLayers = {"VK_LAYER_KHRONOS_validation"},
        .DeviceExtensions = {"VK_KHR_portability_subset", VK_KHR_SWAPCHAIN_EXTENSION_NAME}};

    constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;
    
    
}