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

    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Application.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Entrypoint.hpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Window.hpp
)

set(
    SOURCE_LIST
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Asserts.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Utils/Logging.cpp

    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Application.cpp
    ${Cortex_SOURCE_DIR}/src/Cortex/Core/Window.cpp
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

target_link_directories(
    Cortex PUBLIC
    /Users/sam/VulkanSDK/1.3.243.0/macOS/lib
)

target_link_libraries(
    Cortex PRIVATE
    glfw ${GLFW_LIBRARIES}
    vulkan
    # glad spdlog::spdlog
)

target_compile_features(
    Cortex PUBLIC
    cxx_std_17
)