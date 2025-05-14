cmake_minimum_required (VERSION 3.19)

project(growl-plugin-httplib)

set(SOURCES
	"src/httplib_network.cpp"
	"src/httplib_http.cpp"
	)

set(HEADERS
	"src/httplib_network.h"
	"src/httplib_http.h"
	"src/httplib_error.h"
	)

add_library(growl-plugin-httplib ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-httplib
	PRIVATE growl-core growl-thirdparty::httplib
	)
