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

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(QSnapshot)
set(QSNAPSHOT_VERSION_MAJOR "0")
set(QSNAPSHOT_VERSION_MINOR "1")
set(QSNAPSHOT_VERSION_PATCH "1")
set(QSNAPSHOT_VERSION "${QSNAPSHOT_VERSION_MAJOR}.${QSNAPSHOT_VERSION_MINOR}.${QSNAPSHOT_VERSION_PATCH}")

find_package(Qt4 4.6 REQUIRED)
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

include(${QT_USE_FILE})
set(HAVE_X11_EXTENSIONS_SHAPE_H ${X11_Xshape_FOUND})
set(HAVE_X11_EXTENSIONS_XFIXES_H ${X11_Xfixes_FOUND})
include(InstallRequiredSystemLibraries)

configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/qsnapshot_config.h.in" "qsnapshot_config.h")

set(CMAKE_DEBUG_POSTFIX d)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pedantic -Wextra -Wall -pipe")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
    add_definitions(/D_UNICODE)
endif()

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

set(QSNAPSHOT_MOC_HEADERS "")
foreach(header ${QSNAPSHOT_HEADERS})
    file(STRINGS ${header} moc_token REGEX "Q_OBJECT")
    if(moc_token)
        list(APPEND QSNAPSHOT_MOC_HEADERS ${header})
    endif()
endforeach()

qt4_wrap_cpp(QSNAPSHOT_MOC_SOURCES ${QSNAPSHOT_MOC_HEADERS})
qt4_wrap_ui(QSNAPSHOT_UIC_HEADERS ${QSNAPSHOT_FORMS})
qt4_add_resources(QSNAPSHOT_RCC_SOURCES "qsnapshot.qrc")

source_group("Generated Files" FILES ${QSNAPSHOT_MOC_SOURCES} ${QSNAPSHOT_RCC_SOURCES} ${QSNAPSHOT_UIC_HEADERS})
source_group("Resource Files" REGULAR_EXPRESSION .*\\.rc)
source_group("Form Files" FILES ${QSNAPSHOT_FORMS})

set(QSNAPSHOT_COMPILE_UNITS ${QSNAPSHOT_HEADERS} ${QSNAPSHOT_SOURCES} ${QSNAPSHOT_PLATFORM_SOURCES} ${QSNAPSHOT_PLATFORM_HEADERS} ${QSNAPSHOT_MOC_SOURCES} ${QSNAPSHOT_UIC_HEADERS} ${QSNAPSHOT_RCC_SOURCES})
set(QSNAPSHOT_LINKING_UNITS ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

if(MSVC)
    add_executable(QSnapshot WIN32 "qsnapshot.rc" ${QSNAPSHOT_COMPILE_UNITS})
    target_link_libraries(QSnapshot ${QSNAPSHOT_LINKING_UNITS} ${QT_QTMAIN_LIBRARY})
elseif(APPLE)
    add_executable(QSnapshot MACOSX_BUNDLE "share/pixmap/logo.icns" ${QSNAPSHOT_COMPILE_UNITS})
    add_dependencies(QSnapshot cocoa)
    target_link_libraries(QSnapshot cocoa ${QSNAPSHOT_LINKING_UNITS} ${COCOA_LIBRARY})
    set_target_properties(QSnapshot PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "logo.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "QSnapshot"
        MACOSX_BUNDLE_LONG_VERSION_STRING "version ${QSNAPSHOT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_NAME "QSnapshot"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "v${QSNAPSHOT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${QSNAPSHOT_VERSION}")
else()
    add_executable(QSnapshot ${QSNAPSHOT_COMPILE_UNITS})
    target_link_libraries(QSnapshot ${QSNAPSHOT_LINKING_UNITS})
endif()
set_target_properties(QSnapshot PROPERTIES VERSION ${QSNAPSHOT_VERSION})

# install part
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
