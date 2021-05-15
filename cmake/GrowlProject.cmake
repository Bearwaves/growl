set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DEBUG_POSTFIX "_d")

set(GROWL_INCLUDE_DIRS
	${GROWL_PATH}/include
	${GROWL_PATH}/src/core/include
	)

set(GROWL_LIBS
	optimized growl-core
	debug growl-core_d
	)

include_directories(${GROWL_INCLUDE_DIRS})
