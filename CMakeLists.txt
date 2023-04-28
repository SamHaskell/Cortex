cmake_minimum_required(
    VERSION 3.26.0
)

project(
    CortexDev
    VERSION 0.1
    LANGUAGES CXX
)

# Cortex Engine dylib compiled here
add_subdirectory(Cortex)

# Client exe compiled here
add_subdirectory(testbed)