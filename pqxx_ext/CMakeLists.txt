
set(PUBLIC_INCLUDE_DIR inc)
set(INCLUDE_DIR inc/pqxx_ext)

add_library(pqxx_ext
    INTERFACE
    ${INCLUDE_DIR}/Vector.h

)

target_include_directories(pqxx_ext
    INTERFACE
    ${PUBLIC_INCLUDE_DIR}
)