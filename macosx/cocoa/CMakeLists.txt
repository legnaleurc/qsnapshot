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

# must use Clang
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
string(REPLACE "-std=c++0x" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE QSNAPSHOT_COCOA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)

include_directories("include" "src")

add_library(cocoa ${QSNAPSHOT_COCOA_SOURCES})
