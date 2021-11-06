# header only library with instruction codes
project (identifiers)

add_library(${PROJECT_NAME} INTERFACE)
add_library(lc3::identifiers ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)