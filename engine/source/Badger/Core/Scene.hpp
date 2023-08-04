#pragma once

#include "Badger/Base/Base.hpp"

#include "Badger/Core/Camera.hpp"
#include "Badger/Entities/Entity.hpp"


#pragma once

namespace Badger {
    struct Scene
    {
        Camera MainCamera;
        std::vector<Entity> Entities;
    };
}