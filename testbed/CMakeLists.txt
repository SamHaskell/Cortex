project(
    testbed
)

set(
    SOURCE_LIST
    ${testbed_SOURCE_DIR}/src/Testbed.cpp
)

set(
    HEADER_LIST
    
)

add_executable(
    testbed
    ${SOURCE_LIST}
    ${HEADER_LIST}
)

target_compile_features(
    testbed PRIVATE
    cxx_std_17
)

target_link_libraries(
    testbed PRIVATE
    Cortex
)