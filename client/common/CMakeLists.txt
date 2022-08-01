cmake_minimum_required(VERSION 3.23)

project(common)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core5Compat)

set(COMMON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../common)
set(WINDSCRIBE_BUILD_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../build-libs)

set(PROJECT_SOURCES
    ${COMMON_PATH}/ipc/command.h
    ${COMMON_PATH}/ipc/commandfactory.cpp
    ${COMMON_PATH}/ipc/commandfactory.h
    ${COMMON_PATH}/ipc/connection.cpp
    ${COMMON_PATH}/ipc/connection.h
    ${COMMON_PATH}/ipc/generated_proto/apiinfo.pb.cc
    ${COMMON_PATH}/ipc/generated_proto/apiinfo.pb.h
    ${COMMON_PATH}/ipc/generated_proto/cli.pb.cc
    ${COMMON_PATH}/ipc/generated_proto/cli.pb.h
    ${COMMON_PATH}/ipc/generated_proto/clientcommands.pb.cc
    ${COMMON_PATH}/ipc/generated_proto/clientcommands.pb.h
    ${COMMON_PATH}/ipc/generated_proto/servercommands.pb.cc
    ${COMMON_PATH}/ipc/generated_proto/servercommands.pb.h
    ${COMMON_PATH}/ipc/generated_proto/types.pb.cc
    ${COMMON_PATH}/ipc/generated_proto/types.pb.h
    ${COMMON_PATH}/ipc/iconnection.h
    ${COMMON_PATH}/ipc/iserver.h
    ${COMMON_PATH}/ipc/protobufcommand.h
    ${COMMON_PATH}/ipc/server.cpp
    ${COMMON_PATH}/ipc/server.h
    ${COMMON_PATH}/names.h
    ${COMMON_PATH}/types/locationid.cpp
    ${COMMON_PATH}/types/locationid.h
    ${COMMON_PATH}/types/pingtime.cpp
    ${COMMON_PATH}/types/pingtime.h
    ${COMMON_PATH}/utils/clean_sensitive_info.cpp
    ${COMMON_PATH}/utils/clean_sensitive_info.h
    ${COMMON_PATH}/utils/executable_signature/executable_signature.cpp
    ${COMMON_PATH}/utils/executable_signature/executable_signature.h
    ${COMMON_PATH}/utils/extraconfig.cpp
    ${COMMON_PATH}/utils/extraconfig.h
    ${COMMON_PATH}/utils/hardcodedsettings.cpp
    ${COMMON_PATH}/utils/hardcodedsettings.h
    ${COMMON_PATH}/utils/ipvalidation.cpp
    ${COMMON_PATH}/utils/ipvalidation.h
    ${COMMON_PATH}/utils/languagesutil.cpp
    ${COMMON_PATH}/utils/languagesutil.h
    ${COMMON_PATH}/utils/logger.cpp
    ${COMMON_PATH}/utils/logger.h
    ${COMMON_PATH}/utils/mergelog.cpp
    ${COMMON_PATH}/utils/mergelog.h
    ${COMMON_PATH}/utils/multiline_message_logger.h
    ${COMMON_PATH}/utils/protobuf_includes.h
    ${COMMON_PATH}/utils/simplecrypt.cpp
    ${COMMON_PATH}/utils/simplecrypt.h
    ${COMMON_PATH}/utils/utils.cpp
    ${COMMON_PATH}/utils/utils.h
    ${COMMON_PATH}/utils/widgetutils.cpp
    ${COMMON_PATH}/utils/widgetutils.h
    ${COMMON_PATH}/version/appversion.cpp
    ${COMMON_PATH}/version/appversion.h
    ${COMMON_PATH}/version/windscribe_version.h
)


if (WIN32)
    set(PROJECT_SOURCES_WIN
        ${COMMON_PATH}/utils/crashdump.cpp
        ${COMMON_PATH}/utils/crashhandler.cpp
        ${COMMON_PATH}/utils/winutils.cpp
        ${COMMON_PATH}/utils/widgetutils_win.cpp
        ${COMMON_PATH}/utils/executable_signature/executable_signature_win.cpp
        ${COMMON_PATH}/utils/servicecontrolmanager.cpp
    )
elseif(APPLE)
    set(PROJECT_SOURCES_MAC
        ${COMMON_PATH}/exithandler_mac.mm
        ${COMMON_PATH}/exithandler_mac.h
        ${COMMON_PATH}/utils/executable_signature/executable_signature_mac.h
        ${COMMON_PATH}/utils/executable_signature/executable_signature_mac.mm
        ${COMMON_PATH}/utils/macutils.h
        ${COMMON_PATH}/utils/macutils.mm
        ${COMMON_PATH}/utils/network_utils/network_utils_mac.cpp
        ${COMMON_PATH}/utils/network_utils/network_utils_mac.h
        ${COMMON_PATH}/utils/widgetutils_mac.h
        ${COMMON_PATH}/utils/widgetutils_mac.mm
    )

elseif(UNIX)
    set(PROJECT_SOURCES_LINUX
        ${COMMON_PATH}/utils/executable_signature/executablesignature_linux.cpp
        ${COMMON_PATH}/utils/executable_signature/executablesignature_linux.h
        ${COMMON_PATH}/utils/linuxutils.cpp
        ${COMMON_PATH}/utils/linuxutils.h
    )
endif()


add_library(common STATIC ${PROJECT_SOURCES} ${PROJECT_SOURCES_WIN} ${PROJECT_SOURCES_MAC} ${PROJECT_SOURCES_LINUX})


target_link_libraries(common PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Core5Compat)
target_compile_definitions(common PRIVATE CMAKE_LIBRARY_LIBRARY
                                  WINVER=0x0601
                                  _WIN32_WINNT=0x0601
                                  WIN32_LEAN_AND_MEAN
                                  PIO_APC_ROUTINE_DEFINED)

target_include_directories(common PRIVATE
    ${COMMON_PATH}
    ${WINDSCRIBE_BUILD_LIBS_PATH}/protobuf/release/include
    ${WINDSCRIBE_BUILD_LIBS_PATH}/protobuf/include
    ${WINDSCRIBE_BUILD_LIBS_PATH}/openssl/include
    ${WINDSCRIBE_BUILD_LIBS_PATH}/boost/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../gui
)

