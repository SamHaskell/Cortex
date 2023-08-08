cmake_minimum_required(
    VERSION 3.10.0
)

project(
    CortexDev
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Engine static lib compiled here
add_subdirectory(engine)

# Client exe compiled here
add_subdirectory(testbed)