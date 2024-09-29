cmake_minimum_required (VERSION 3.19)

project(growl-scene)

set(SOURCES
	"src/scene.cpp"
	"src/node.cpp"
	"src/sprite.cpp"
	"src/rectangle.cpp"
	)

add_library(growl-scene ${SOURCES})
target_include_directories(growl-scene PUBLIC "include")

target_link_libraries(growl-scene
	PRIVATE
	growl-core
	)
