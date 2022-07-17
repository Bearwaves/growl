# growl_project provides a utility function to detect and link the
# Growl modules needed for the platform the game is being built for.

function (growl_project target)
	if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
		set(GROWL_METAL 1)
		add_definitions(-DGROWL_METAL)
	else ()
		set(GROWL_OPENGL 1)
		add_definitions(-DGROWL_OPENGL)
	endif()
	set(GROWL_DESKTOP 1)

	add_subdirectory(${GROWL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/growl)

	if (GROWL_DESKTOP)
		target_link_libraries(${target} PRIVATE growl-platform-desktop)
	endif ()
endfunction ()
