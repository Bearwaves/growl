cmake_minimum_required(VERSION 3.19)

project(growl-plugin-android)

message(STATUS "[Growl] Enabled Android plugin.")

set(SOURCES
	"src/android_plugin.cpp"
	"src/android_system.cpp"
	"src/android_file.cpp"
	)

set(HEADERS
	"src/android_system.h"
	"src/android_file.h"
	"src/android_window.h"
	)

add_library(growl-plugin-android ${SOURCES} ${HEADERS})

target_link_libraries(growl-plugin-android
	PRIVATE
	android
	android-native-app-glue
	log
	growl-core
	)
target_include_directories(growl-plugin-android
	PRIVATE
	${ANDROID_NDK}/sources/android/native_app_glue
	)
target_compile_definitions(growl-plugin-android INTERFACE "GROWL_ANDROID")
