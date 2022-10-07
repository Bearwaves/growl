cmake_minimum_required(VERSION 3.19)

project(growl-plugin-ios LANGUAGES OBJCXX)

message(STATUS "[Growl] Enabled iOS plugin.")

set(SOURCES
	"src/ios_plugin.mm"
	"src/ios_system.mm"
	)

set(HEADERS
	"src/ios_system.h"
	"src/ios_window.h"
	)

add_library(growl-plugin-ios ${SOURCES} ${HEADERS})

target_link_libraries(growl-plugin-ios
	PRIVATE
	growl-core
	"-framework GameController"
	)
target_compile_definitions(growl-plugin-ios INTERFACE "GROWL_IOS")
