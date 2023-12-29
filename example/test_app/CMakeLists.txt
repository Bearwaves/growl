cmake_minimum_required (VERSION 3.19)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(growl-test-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
	"src/test_app.cpp"
	"src/test_app_game.cpp"
	"src/input.cpp"
	)

set(HEADERS
	"src/test_app_game.h"
	"src/input.h"
	)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
	set(CMAKE_OBJCXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

set(GROWL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_subdirectory(${GROWL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/growl)

if (GROWL_ANDROID)
	add_library(growl-test-app SHARED ${SOURCES} ${HEADERS})
else ()
	add_executable(growl-test-app ${SOURCES} ${HEADERS})
endif ()

if (GROWL_DESKTOP)
	target_link_libraries(growl-test-app PRIVATE growl-platform-desktop)
elseif (GROWL_IOS)
	target_link_libraries(growl-test-app PRIVATE growl-platform-ios)
	set(IOS_ASSETS res/ios/Assets.xcassets)
	target_sources(growl-test-app PRIVATE ${IOS_ASSETS})
	set_target_properties(growl-test-app
		PROPERTIES
		MACOSX_BUNDLE YES
		XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.bearwaves.growl.testapp"
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.bearwaves.growl.testapp"
		XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
		XCODE_ATTRIBUTE_PRODUCT_NAME "Growl Test App"
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
		XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "Icon"
		RESOURCE "${IOS_ASSETS}"
		)
	target_compile_options(growl-test-app PRIVATE -x objective-c++)
elseif (GROWL_ANDROID)
	target_link_libraries(growl-test-app PUBLIC growl-platform-android)
elseif (GROWL_WEB)
	target_link_libraries(growl-test-app PRIVATE growl-platform-web)
	target_link_options(growl-test-app PRIVATE --preload-file assets.growl)
	set(CMAKE_EXECUTABLE_SUFFIX .html)
	# We need to use this over the more modern target_link_options as CMake
	# currently struggles with dollar signs in linker flags.
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$ccall"
		)
endif()
