add_subdirectory(core)
add_subdirectory(scene)
add_subdirectory(platforms)
add_subdirectory(plugins)
add_subdirectory(version)

if (GROWL_TOOLS)
	add_subdirectory(tools)
endif()
