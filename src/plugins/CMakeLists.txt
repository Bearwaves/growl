if (GROWL_SDL3)
	add_subdirectory(sdl3)
endif()

if (GROWL_SOLOUD)
	add_subdirectory(soloud)
endif()

if (GROWL_METAL)
	add_subdirectory(metal)
endif()

if (GROWL_OPENGL)
	add_subdirectory(opengl)
endif()

if (GROWL_IOS)
	add_subdirectory(ios)
endif()

if (GROWL_ANDROID)
	add_subdirectory(android)
endif()

if (GROWL_LUA)
	add_subdirectory(lua)
endif()

if (GROWL_CURL)
	add_subdirectory(curl)
endif()
