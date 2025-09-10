set(ENABLE_TESTING OFF CACHE INTERNAL "Do not build tests")
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "Do not build tools")
add_subdirectory(mbedtls)

if (APPLE)
	set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

set(MBEDTLS_LIBRARY mbedtls)
set(MBEDX509_LIBRARY mbedx509)
set(MBEDCRYPTO_LIBRARY mbedcrypto)
set(MBEDTLS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/include")
add_library(growl-thirdparty::mbedtls ALIAS mbedtls)
