#pragma once

#include "Badger/Base/Base.hpp"

#include "Badger/Entities/Transform.hpp"
#include "Badger/Entities/MeshInstance.hpp"

namespace Badger {
    struct Entity {
        u32 Identifier; // Make a GUID at some point
        Transform Transform;
        MeshInstance Mesh;
        static Entity Create() { return Entity {.Identifier=0}; }
    };
}