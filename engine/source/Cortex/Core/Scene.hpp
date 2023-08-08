#pragma once

#include "Cortex/Base/Base.hpp"

#include "Cortex/Core/Camera.hpp"
#include "Cortex/Entities/Entity.hpp"


#pragma once

namespace Cortex {
    struct Scene
    {
        Camera MainCamera;
        std::vector<Entity> Entities;
    };
}