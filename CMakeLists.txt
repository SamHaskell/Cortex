cmake_minimum_required(
    VERSION 3.26.0
)

project(
    CortexDev
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Cortex Engine dylib compiled here
add_subdirectory(Cortex)

# Client exe compiled here
add_subdirectory(testbed)