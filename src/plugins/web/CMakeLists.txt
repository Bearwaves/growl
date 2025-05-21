cmake_minimum_required (VERSION 3.19)

project(growl-plugin-web)

message(STATUS "[Growl] Enabled Web plugin.")

set(SOURCES
	"src/web_network.cpp"
	"src/web_http.cpp"
)

set(HEADERS
	"src/web_network.h"
	"src/web_http.h"
	"src/web_error.h"
)

add_library(growl-plugin-web ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-web PRIVATE growl-core)
target_link_options(growl-plugin-web
	INTERFACE
	-sFETCH
)
