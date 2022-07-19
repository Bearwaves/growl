cmake_minimum_required (VERSION 3.19)

project(growl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Note that the Desktop platform will turn on plugins according to its own logic.
# See src/platforms/desktop/CMakeLists.txt
option(GROWL_DESKTOP "Whether to build the included Desktop platform" ON)
option(GROWL_OPENGL "Whether to build the OpenGL plugin" OFF)
option(GROWL_METAL "Whether to build the Metal plugin" OFF)

add_subdirectory(thirdparty)
add_subdirectory(src)
