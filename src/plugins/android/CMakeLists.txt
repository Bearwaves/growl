cmake_minimum_required(VERSION 3.19)

project(growl-plugin-android)

message(STATUS "[Growl] Enabled Android plugin.")

set(SOURCES
	"src/android_plugin.cpp"
	"src/android_system.cpp"
	"src/android_file.cpp"
	"src/android_window.cpp"
	)

set(HEADERS
	"src/android_system.h"
	"src/android_file.h"
	"src/android_window.h"
	"src/android_error.h"
	)

add_library(growl-plugin-android ${SOURCES} ${HEADERS})

find_library(EGL_LIBRARY libEGL.so HINTS ${ANDROID_NDK})

target_link_libraries(growl-plugin-android
	PRIVATE
	android
	android-native-app-glue
	log
	growl-core
	${EGL_LIBRARY}
	)
target_include_directories(growl-plugin-android
	PRIVATE
	${ANDROID_NDK}/sources/android/native_app_glue
	)
target_compile_definitions(growl-plugin-android INTERFACE "GROWL_ANDROID")
