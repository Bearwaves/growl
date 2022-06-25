add_subdirectory(sdl2)
add_subdirectory(soloud)

if (GROWL_METAL)
	add_subdirectory(metal)
endif ()

if (GROWL_OPENGL)
	add_subdirectory(opengl)
endif ()
