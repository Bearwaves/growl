cmake_minimum_required (VERSION 3.19)

project(growl-imgui)

set(SOURCES
	"src/imgui.cpp"
	"src/api/api.cpp"
)

add_library(growl-imgui ${SOURCES})
target_include_directories(growl-imgui PUBLIC "include")

target_link_libraries(growl-imgui
	PUBLIC
	growl-thirdparty::imgui
	PRIVATE
	growl-core
)
