#include "Badger/Core/Camera.hpp"

namespace Badger {
    void Camera::SetPerspectiveProjection(f32 fovy, f32 aspect, f32 near, f32 far) {
        ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
    }

    void Camera::SetOrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
        ProjectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    }

    void Camera::SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
        ViewMatrix = glm::lookAt(position, position + forward, up);
        ViewMatrix[1][1] *= -1;
    }
}