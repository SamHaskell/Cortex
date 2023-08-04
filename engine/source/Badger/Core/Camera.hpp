#pragma once

#include "Badger/Graphics/VulkanHelpers.hpp"
#include "Badger/Graphics/VulkanTypes.hpp"

namespace Badger {
    struct Camera {
        glm::mat4 ProjectionMatrix {1.0f};
        glm::mat4 ViewMatrix {1.0f};

        void SetPerspectiveProjection(f32 fovy, f32 aspect, f32 near, f32 far);
        void SetOrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

        void SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
    };
}