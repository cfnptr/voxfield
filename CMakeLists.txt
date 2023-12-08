#---------------------------------------------------------------------------------------------------
# Voxfield - An open source voxel based multiplayer sandbox game.
# Copyright (C) 2022-2023  Nikita Fediuchin
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#---------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "Prevented in-tree build")
endif()

if(TARGET voxfield-client)
	return()
endif()

project(Voxfield VERSION 0.1.0 LANGUAGES CXX
	DESCRIPTION "An open source voxel based multiplayer sandbox game"
	HOMEPAGE_URL "https://github.com/cfnptr/voxfield")

option(VOXFIELD_BUILD_SERVER "Build Voxfield server executable." ON)
option(VOXFIELD_BUILD_LAUNCHER "Build Voxfield launcher executable." ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(BUILD_SHARED_LIBS OFF)

include(libraries/garden/cmake/compile-options.cmake)
include(libraries/garden/cmake/publish-functions.cmake)

set(VOXFIELD_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/include
	libraries/garden/libraries/voxy/include)
set(VOXFIELD_LINK_LIBS garden)

set(GARDEN_APP_NAME "Voxfield")
set(GARDEN_APP_VERSION_MAJOR ${Voxfield_VERSION_MAJOR})
set(GARDEN_APP_VERSION_MINOR ${Voxfield_VERSION_MINOR})
set(GARDEN_APP_VERSION_PATCH ${Voxfield_VERSION_PATCH})
set(GARDEN_APP_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
set(GARDEN_APP_CACHES_DIR ${PROJECT_SOURCE_DIR}/.caches)
add_subdirectory(libraries/garden)

file(GLOB_RECURSE VOXFIELD_SHARED_SOURCES source/shared/*.cpp)
file(GLOB_RECURSE VOXFIELD_CLIENT_SOURCES source/client/*.cpp)
add_executable(voxfield-client source/client.cpp
	${VOXFIELD_CLIENT_SOURCES} ${VOXFIELD_SHARED_SOURCES})
target_include_directories(voxfield-client PUBLIC ${VOXFIELD_INCLUDE_DIRS})
target_link_libraries(voxfield-client ${VOXFIELD_LINK_LIBS})
stripExecutable(voxfield-client)

if(VOXFIELD_BUILD_SERVER)
	file(GLOB_RECURSE VOXFIELD_SERVER_SOURCES source/server/*.cpp)
	add_executable(voxfield-server source/server.cpp ${VOXFIELD_SERVER_SOURCES})
	target_include_directories(voxfield-server PUBLIC ${VOXFIELD_INCLUDE_DIRS})
	target_link_libraries(voxfield-server ${VOXFIELD_LINK_LIBS})
	stripExecutable(voxfield-server)
endif()