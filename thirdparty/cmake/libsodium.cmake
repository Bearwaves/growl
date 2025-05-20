set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libsodium-cmake)
set(SODIUM_DISABLE_TESTS ON)
set(SODIUM_MINIMAL ON)
add_subdirectory(${SOURCE_PREFIX})

add_library(growl-thirdparty::sodium ALIAS sodium)
