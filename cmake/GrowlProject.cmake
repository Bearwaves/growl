set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -ggdb3")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(version)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(GROWL_METAL 1)
	add_definitions(-DGROWL_METAL)
else ()
	set(GROWL_OPENGL 1)
	add_definitions(-DGROWL_OPENGL)
endif()

set(GROWL_INCLUDE_DIRS
	${GROWL_PATH}/src/core/include
	${GROWL_PATH}/src/util/include
	)

set(GROWL_LIBS
	growl-core
	growl-util
	)

set(GROWL_INCLUDE_DIRS
	${GROWL_INCLUDE_DIRS}
	${GROWL_PATH}/src/platforms/desktop/include
	)
set(GROWL_LIBS
	${GROWL_LIBS}
	growl-platform-desktop
	)

include_directories(${GROWL_INCLUDE_DIRS})
