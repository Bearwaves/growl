set(ENABLE_TESTING OFF CACHE INTERNAL "Do not build tests")
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "Do not build tools")
add_subdirectory(mbedtls)

set(MBEDTLS_LIBRARY mbedtls)
set(MBEDX509_LIBRARY mbedx509)
set(MBEDCRYPTO_LIBRARY mbedcrypto)
set(MBEDTLS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/include")
add_library(growl-thirdparty::mbedtls ALIAS mbedtls)
