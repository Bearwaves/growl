add_subdirectory(core)
add_subdirectory(platforms)
add_subdirectory(plugins)
add_subdirectory(version)

if (GROWL_TOOLS)
	add_subdirectory(tools)
endif()
