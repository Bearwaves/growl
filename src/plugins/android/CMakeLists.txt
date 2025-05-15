cmake_minimum_required(VERSION 3.19)

project(growl-plugin-android)

message(STATUS "[Growl] Enabled Android plugin.")

set(SOURCES
	"src/android_system.cpp"
	"src/android_file.cpp"
	"src/android_window.cpp"
	"src/android_preferences.cpp"
	"src/android_haptics.cpp"

	"src/android_network.cpp"
	"src/android_http.cpp"
	)

set(HEADERS
	"src/android_system.h"
	"src/android_file.h"
	"src/android_window.h"
	"src/android_error.h"
	"src/android_preferences.h"
	"src/android_haptics.h"

	"src/android_network.h"
	"src/android_http.h"
	)

add_library(growl-plugin-android ${SOURCES} ${HEADERS})

find_library(EGL_LIBRARY libEGL.so HINTS ${ANDROID_NDK})
find_package(game-activity REQUIRED CONFIG)
find_package(games-controller REQUIRED CONFIG)
find_package(curl REQUIRED CONFIG)

target_link_libraries(growl-plugin-android
	PRIVATE
	android
	log
	game-activity::game-activity_static
	games-controller::paddleboat_static
	growl-core
	curl::curl_static
	${EGL_LIBRARY}
	)
target_compile_definitions(growl-plugin-android INTERFACE "GROWL_ANDROID")
