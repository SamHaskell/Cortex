#pragma once

#include "Cortex/Base/Base.hpp"

#include "Cortex/Entities/Transform.hpp"
#include "Cortex/Entities/MeshInstance.hpp"

namespace Cortex {
    struct Entity {
        u32 Identifier; // Make a GUID at some point
        Transform Transform;
        MeshInstance Mesh;
        static Entity Create() { return Entity {.Identifier=0}; }
    };
}