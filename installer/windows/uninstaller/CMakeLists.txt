cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    set(VCPKG_TARGET_TRIPLET arm64-windows-static)
else()
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif()

add_definitions(-DUNICODE -D_UNICODE)

project(uninstaller)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(SOURCES
    arguments_parser.cpp
    copy_and_run.cpp
    main.cpp
    registry.cpp
    remove_directory.cpp
    uninstall.cpp
    ../../../client/common/utils/servicecontrolmanager.cpp
    ../utils/applicationinfo.cpp
    ../utils/path.cpp
    ../utils/utils.cpp
    uninstall.rc
)

add_executable(uninstall WIN32 ${SOURCES})
set_property(TARGET uninstall PROPERTY LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /DELAYLOAD:dwmapi.dll /DELAYLOAD:uxtheme.dll /DELAYLOAD:wldp.dll")


target_include_directories(uninstall
    PRIVATE
    ../../../client/common
    ../../../client/common/types
    ../../../client/common/utils
)

target_link_libraries(uninstall
                      PRIVATE
                      delayimp
                      shlwapi
                      fmt::fmt-header-only
                      spdlog::spdlog_header_only
)

install(TARGETS uninstall
    RUNTIME DESTINATION .
)
