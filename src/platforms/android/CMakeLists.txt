cmake_minimum_required (VERSION 3.19)

project(growl-platform-android)

message(STATUS "[Growl] Enabled Android platform.")

set(SOURCES
	"src/main.cpp"
	)

add_library(growl-platform-android ${SOURCES})

set(GROWL_OPENGL ON)
set(GROWL_OPENGL ON PARENT_SCOPE)

add_library(android-native-app-glue
	STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
	)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(growl-platform-android
	PUBLIC
	growl-core
	PRIVATE
	growl-plugin-android
	growl-plugin-opengl
	growl-plugin-soloud
	android-native-app-glue
	android
	log
	)
target_include_directories(growl-platform-android
	PRIVATE
	${ANDROID_NDK}/sources/android/native_app_glue
	)
target_compile_definitions(growl-platform-android INTERFACE "GROWL_ANDROID")
target_link_options(growl-platform-android
	PUBLIC
	-u ANativeActivity_onCreate
	)
