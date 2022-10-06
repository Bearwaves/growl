cmake_minimum_required (VERSION 3.19)

project(growl-platform-ios LANGUAGES OBJCXX)

message(STATUS "[Growl] Enabled iOS platform.")

set(SOURCES
	"src/app_delegate.mm"
	"src/view_controller.mm"
	"src/metal_view.mm"
	)

add_library(growl-platform-ios ${SOURCES})

set(GROWL_METAL ON)
set(GROWL_METAL ON PARENT_SCOPE)

target_link_libraries(growl-platform-ios
	PUBLIC
	growl-core
	PRIVATE
	growl-plugin-metal
	growl-plugin-soloud
	growl-plugin-ios
	"-framework UIKit"
	"-framework Foundation"
	)
target_include_directories(growl-platform-ios PUBLIC "include")
target_compile_definitions(growl-platform-ios INTERFACE "GROWL_IOS")
