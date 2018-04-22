#    QSnapshot, a screen capture tool.
#    Copyright (C)  2012 Wei Cheng Pan <legnaleurc@gmail>
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

cmake_minimum_required(VERSION 3.8)

project(QSnapshot)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_DEBUG_POSTFIX d)

set(QSNAPSHOT_VERSION_MAJOR "0")
set(QSNAPSHOT_VERSION_MINOR "1")
set(QSNAPSHOT_VERSION_PATCH "1")
set(QSNAPSHOT_VERSION "${QSNAPSHOT_VERSION_MAJOR}.${QSNAPSHOT_VERSION_MINOR}.${QSNAPSHOT_VERSION_PATCH}")

set(HAVE_X11_EXTENSIONS_SHAPE_H ${X11_Xshape_FOUND})
set(HAVE_X11_EXTENSIONS_XFIXES_H ${X11_Xfixes_FOUND})

configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/qsnapshot_config.h.in" "qsnapshot_config.h")

# hack MSVC path
if(MSVC OR APPLE)
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
else()
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
endif()

file(GLOB_RECURSE QSNAPSHOT_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.hpp)
file(GLOB_RECURSE QSNAPSHOT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE QSNAPSHOT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.ui)
if(APPLE)
    file(GLOB_RECURSE QSNAPSHOT_PLATFORM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} macosx/utility/*)
    add_subdirectory("macosx/cocoa")
    include_directories("macosx/cocoa/include")
    find_library(COCOA_LIBRARY Cocoa)
    set_source_files_properties(
        "share/pixmap/logo.icns"
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
elseif(UNIX)
    file(GLOB_RECURSE QSNAPSHOT_PLATFORM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} x11/*.cpp)
    file(GLOB_RECURSE QSNAPSHOT_PLATFORM_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} x11/*.hpp)
elseif(WIN32)
    file(GLOB_RECURSE QSNAPSHOT_PLATFORM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} windows/*.cpp)
    file(GLOB_RECURSE QSNAPSHOT_PLATFORM_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} windows/*.hpp)
endif()
if(UNIX)
    file(GLOB_RECURSE tmp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "unix/*.cpp")
    set(QSNAPSHOT_PLATFORM_SOURCES ${QSNAPSHOT_PLATFORM_SOURCES} ${tmp})
    file(GLOB_RECURSE tmp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "unix/*.hpp")
    set(QSNAPSHOT_PLATFORM_HEADERS ${QSNAPSHOT_PLATFORM_HEADERS} ${tmp})
endif()

include_directories(
    "src"
    ${CMAKE_CURRENT_BINARY_DIR})

source_group("Resource Files" REGULAR_EXPRESSION .*\\.rc)
source_group("Form Files" FILES ${QSNAPSHOT_FORMS})

set(QSNAPSHOT_COMPILE_UNITS ${QSNAPSHOT_HEADERS} ${QSNAPSHOT_SOURCES} ${QSNAPSHOT_PLATFORM_SOURCES} ${QSNAPSHOT_PLATFORM_HEADERS})

if(MSVC)
    add_executable(QSnapshot WIN32 "qsnapshot.rc" ${QSNAPSHOT_COMPILE_UNITS})
    target_link_libraries(QSnapshot Qt5::Core Qt5::Widgets Qt5::Main)
elseif(APPLE)
    add_executable(QSnapshot MACOSX_BUNDLE "share/pixmap/logo.icns" ${QSNAPSHOT_COMPILE_UNITS})
    add_dependencies(QSnapshot cocoa)
    target_link_libraries(QSnapshot cocoa Qt5::Core Qt5::Widgets ${COCOA_LIBRARY})
    set_target_properties(QSnapshot PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "logo.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "QSnapshot"
        MACOSX_BUNDLE_LONG_VERSION_STRING "version ${QSNAPSHOT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_NAME "QSnapshot"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "v${QSNAPSHOT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${QSNAPSHOT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in")
else()
    add_executable(QSnapshot ${QSNAPSHOT_COMPILE_UNITS})
    target_link_libraries(QSnapshot Qt5::Core Qt5::Widgets)
endif()
set_target_properties(QSnapshot PROPERTIES VERSION ${QSNAPSHOT_VERSION})
target_compile_features(QSnapshot PRIVATE cxx_std_11)

# install part
include(InstallRequiredSystemLibraries)
install(TARGETS QSnapshot
    RUNTIME DESTINATION "bin" COMPONENT Runtime
    BUNDLE DESTINATION "." COMPONENT Runtime)
if(APPLE OR WIN32)
    configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/install.cmake.in" "${CMAKE_BINARY_DIR}/install.cmake" @ONLY)
    install(SCRIPT "${CMAKE_BINARY_DIR}/install.cmake" COMPONENT Runtime)
endif()

# package part
set(CPACK_PACKAGE_VERSION_MAJOR ${QSNAPSHOT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${QSNAPSHOT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${QSNAPSHOT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
if(APPLE)
    set(CPACK_GENERATOR DragNDrop)
elseif(WIN32)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "QSnapshot")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  CreateShortCut \\\"\\\$SMPROGRAMS\\\\\\\$STARTMENU_FOLDER\\\\QSnapshot.lnk\\\" \\\"\\\$INSTDIR\\\\bin\\\\QSnapshot.exe\\\"
  CreateShortCut \\\"\\\$DESKTOP\\\\QSnapshot.lnk\\\" \\\"\\\$INSTDIR\\\\bin\\\\QSnapshot.exe\\\"
    ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  Delete \\\"\\\$DESKTOP\\\\QSnapshot.lnk\\\"
  !insertmacro MUI_STARTMENU_GETFOLDER Application \\\$MUI_TEMP
  Delete \\\"\\\$SMPROGRAMS\\\\\\\$MUI_TEMP\\\\QSnapshot.lnk\\\"
    ")
endif()

include(CPack)
