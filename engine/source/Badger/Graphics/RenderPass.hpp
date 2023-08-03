#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

// Will add some way to store the identifying info of a renderpass for checking compatibility etc etc.

namespace Badger {
    struct RenderPass {
        VkRenderPass RenderPass;
    };
}