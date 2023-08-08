#pragma once

#include "Cortex/Graphics/Model.hpp"
#include "Cortex/Graphics/Material.hpp"

namespace Cortex {
    struct MeshInstance {
        std::shared_ptr<Model> Model;
        std::shared_ptr<Material> Material;
    };
}