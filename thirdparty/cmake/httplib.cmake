set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib)

set(INCLUDES
	"${SOURCE_PREFIX}"
	)

find_package(OpenSSL "3.0.0" COMPONENTS SSL Crypto QUIET)
unset (MESSAGE_QUIET)
if (TARGET OpenSSL::SSL AND TARGET OpenSSL::Crypto)
	set(USING_OPENSSL ON)
	message(STATUS "[Growl] Found OpenSSL, using it")
else ()
	set(USING_OPENSSL OFF)
	message(STATUS "[Growl] Did not find OpenSSL; https network support will not work")
endif ()
set (MESSAGE_QUIET ON)

growl_thirdparty_lib(httplib
	INCLUDES INTERFACE ${INCLUDES}
	)
target_link_libraries(httplib
	INTERFACE
	growl-thirdparty::zlib
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:crypt32>
	$<$<BOOL:${USING_OPENSSL}>:OpenSSL::SSL>
	$<$<BOOL:${USING_OPENSSL}>:OpenSSL::Crypto>
	)
target_compile_definitions(httplib
	INTERFACE
	"CPPHTTPLIB_ZLIB_SUPPORT"
	"CPPHTTPLIB_NO_EXCEPTIONS"
	"$<$<BOOL:${USING_OPENSSL}>:CPPHTTPLIB_OPENSSL_SUPPORT>"
	)
