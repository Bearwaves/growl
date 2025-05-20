cmake_minimum_required (VERSION 3.19)

project(growl-plugin-curl)

message(STATUS "[Growl] Enabled Curl plugin.")

set(SOURCES
	"src/curl_network.cpp"
	"src/curl_http.cpp"
)

set(HEADERS
	"src/curl_network.h"
	"src/curl_http.h"
	"src/curl_error.h"
)

add_library(growl-plugin-curl ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-curl
	PRIVATE growl-core growl-thirdparty::curl
)
