#include "Cortex/Graphics/Shader.hpp"

namespace Cortex {
    std::shared_ptr<Shader> Shader::Create(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {
        return std::make_shared<Shader>(device, vertPath, fragPath);
    }

    Shader::Shader(std::shared_ptr<GraphicsDevice> device, const std::string& vertPath, const std::string& fragPath) {

    }

    Shader::~Shader() {

    }
}