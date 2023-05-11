project(
    Cortex
    VERSION 0.1
    DESCRIPTION "Cortex Engine"
    LANGUAGES CXX
)

# add_subdirectory(vendor/glad)
# add_subdirectory(vendor/spdlog)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

set(
    HEADER_LIST

    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Asserts.h
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Defines.h
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Logging.hpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Events/Event.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Events/KeyEvent.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Events/MouseEvent.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Events/SystemEvent.hpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Application.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Entrypoint.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Window.hpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderContext.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderInstance.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderDevice.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/Swapchain.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/Pipeline.hpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/VulkanUtils.h

)

set(
    SOURCE_LIST
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Asserts.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Logging.cpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Application.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Window.cpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderContext.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderInstance.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/RenderDevice.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/Swapchain.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Rendering/Pipeline.cpp

)

add_library(
    Cortex STATIC
    ${SOURCE_LIST}
    ${HEADER_LIST}
)

target_include_directories(
    Cortex PUBLIC
    ${Cortex_SOURCE_DIR}/src
    ${Cortex_SOURCE_DIR}/vendor/glfw/include
    /Users/sam/VulkanSDK/1.3.243.0/macOS/include
)

target_include_directories(
    Cortex PRIVATE
    vendor/glm
)

target_link_directories(
    Cortex PUBLIC
    /Users/sam/VulkanSDK/1.3.243.0/macOS/lib
)

target_link_libraries(
    Cortex PRIVATE
    glfw ${GLFW_LIBRARIES}
    vulkan
    # glslang
    # shaderc
    # glad spdlog::spdlog
)

target_compile_features(
    Cortex PUBLIC
    cxx_std_17
)

file(GLOB PROJECT_SHADERS assets/Shaders/*.comp
                          assets/Shaders/*.frag
                          assets/Shaders/*.geom
                          assets/Shaders/*.vert)

add_custom_command(
    TARGET Cortex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets/Shaders $<TARGET_FILE_DIR:Cortex>/assets/Shaders
    DEPENDS ${PROJECT_SHADERS})