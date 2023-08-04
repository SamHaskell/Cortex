project(
    Badger
    VERSION 0.1
    DESCRIPTION "Badger Engine"
    LANGUAGES C CXX
)

################### VENDOR ###################

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

set(
    VENDOR_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/vendor/glfw/include
    ${PROJECT_SOURCE_DIR}/vendor/glm
)

set(
    VENDOR_SOURCES
)

################### LIBRARY ##################

set(
    LOCAL_HEADERS
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Base/Asserts.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Base/Defines.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Base/Logging.hpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Entrypoint.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/App.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Events.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Window.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Scene.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Camera.hpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanHelpers.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanTypes.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanBuffers.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/GraphicsContext.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/GraphicsDevice.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Swapchain.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Renderer.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Shader.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Pipeline.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Model.hpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/RenderPass.hpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/Entity.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/Transform.hpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/MeshInstance.hpp

)

set(
    LOCAL_SOURCES
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Base/Logging.cpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Entrypoint.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/App.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Events.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Window.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Scene.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Core/Camera.cpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanHelpers.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanTypes.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/VulkanBuffers.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/GraphicsContext.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/GraphicsDevice.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Swapchain.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Shader.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Pipeline.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Graphics/Model.cpp

    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/Entity.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/Transform.cpp
    ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/Entities/MeshInstance.cpp
)

add_library(
    ${PROJECT_NAME} STATIC
    ${LOCAL_SOURCES}
    ${VENDOR_SOURCES}
    ${LOCAL_HEADERS}
)
    
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/source/
    ${VENDOR_INCLUDE_DIRS}
    /Users/sam/VulkanSDK/1.3.243.0/macOS/include
)

target_link_directories(
    ${PROJECT_NAME} PUBLIC
    /Users/sam/VulkanSDK/1.3.243.0/macOS/lib
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    glfw ${GLFW_LIBRARIES}
    vulkan
    spirv-cross-glsl
    spirv-cross-core
    spirv-cross-cpp
)

target_compile_features(
    ${PROJECT_NAME} PUBLIC
    cxx_std_17
)