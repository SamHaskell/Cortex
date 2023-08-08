project(
    CortexTest
    VERSION 0.1
    DESCRIPTION "Cortex Engine Testbed"
    LANGUAGES CXX
)

set(
    LOCAL_SOURCES
    ${PROJECT_SOURCE_DIR}/source/testbed.cpp
)

set(
    LOCAL_HEADERS
)

add_executable(
    ${PROJECT_NAME}
    ${LOCAL_SOURCES}
    ${LOCAL_HEADERS}
)

target_compile_features(
    ${PROJECT_NAME} PRIVATE
    cxx_std_17
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Cortex
)