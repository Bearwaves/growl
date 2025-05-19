set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib)

set(INCLUDES
	"${SOURCE_PREFIX}"
)

find_package(OpenSSL "3.0.0" COMPONENTS SSL Crypto REQUIRED)

growl_thirdparty_lib(httplib
	INCLUDES INTERFACE ${INCLUDES}
)
target_link_libraries(httplib
	INTERFACE
	growl-thirdparty::zlib
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:crypt32>
	OpenSSL::SSL
	OpenSSL::Crypto
)
target_compile_definitions(httplib
	INTERFACE
	"CPPHTTPLIB_ZLIB_SUPPORT"
	"CPPHTTPLIB_NO_EXCEPTIONS"
	"CPPHTTPLIB_OPENSSL_SUPPORT>"
)
