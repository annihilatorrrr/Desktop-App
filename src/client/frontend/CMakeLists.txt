add_subdirectory(frontend-common)

if(UNIX AND (NOT APPLE) AND DEFINE_CLI_ONLY_MACRO)
    add_subdirectory(cli)
else()
    add_subdirectory(gui)
endif()
