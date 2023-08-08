#pragma once

#include "Badger/Graphics/Model.hpp"
#include "Badger/Graphics/Material.hpp"

namespace Badger {
    struct MeshInstance {
        std::shared_ptr<Model> Model;
        std::shared_ptr<Material> Material;
    };
}