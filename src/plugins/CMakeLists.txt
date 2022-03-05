add_subdirectory(sdl2)

if (GROWL_METAL)
	add_subdirectory(metal)
endif ()

if (GROWL_OPENGL)
	add_subdirectory(opengl)
endif ()
