cmake_minimum_required (VERSION 3.19)

project(growl-scene)

set(SOURCES
	"src/node.cpp"
	)

add_library(growl-scene ${SOURCES})
target_include_directories(growl-scene PUBLIC "include")

target_link_libraries(growl-scene
	PRIVATE
	growl-core
	)
