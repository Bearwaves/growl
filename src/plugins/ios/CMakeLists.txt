cmake_minimum_required(VERSION 3.19)

project(growl-plugin-ios LANGUAGES OBJCXX)

message(STATUS "[Growl] Enabled iOS plugin.")

set(SOURCES
	"src/ios_system.mm"
	"src/ios_window.mm"
	"src/ios_preferences.mm"
	"src/ios_haptics.mm"

	"src/ios_network.mm"
	"src/ios_http.mm"
	)

set(HEADERS
	"src/ios_system.h"
	"src/ios_window.h"
	"src/ios_error.h"
	"src/ios_preferences.h"
	"src/ios_haptics.h"

	"src/ios_network.h"
	"src/ios_http.h"
)

add_library(growl-plugin-ios ${SOURCES} ${HEADERS})

target_link_libraries(growl-plugin-ios
	PRIVATE
	growl-core
	"-framework GameController"
	"-framework CoreHaptics"
	)
target_compile_definitions(growl-plugin-ios INTERFACE "GROWL_IOS")
