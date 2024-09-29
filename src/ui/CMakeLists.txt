cmake_minimum_required (VERSION 3.19)

project(growl-ui)

set(SOURCES
	"src/widget.cpp"
	"src/list.cpp"
	)

add_library(growl-ui ${SOURCES})
target_include_directories(growl-ui PUBLIC "include")

target_link_libraries(growl-ui
	PUBLIC
	growl-scene
	PRIVATE
	growl-core
	)
