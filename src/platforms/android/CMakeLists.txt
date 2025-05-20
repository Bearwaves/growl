cmake_minimum_required (VERSION 3.19)

project(growl-platform-android)

message(STATUS "[Growl] Enabled Android platform.")

set(SOURCES
	"src/main/cpp/main.cpp"
)

add_library(growl-platform-android ${SOURCES})

set(GROWL_OPENGL ON)
set(GROWL_OPENGL ON PARENT_SCOPE)

find_package(game-activity REQUIRED CONFIG)

target_link_libraries(growl-platform-android
	PUBLIC
	growl-core
	growl-ui
	PRIVATE
	growl-plugin-android
	growl-plugin-opengl
	growl-plugin-soloud
	growl-plugin-lua
	growl-plugin-curl
	android
	log
	game-activity::game-activity_static
)
target_compile_definitions(growl-platform-android INTERFACE "GROWL_ANDROID")
target_link_options(growl-platform-android
	PUBLIC
	-u Java_com_google_androidgamesdk_GameActivity_initializeNativeCode
)
