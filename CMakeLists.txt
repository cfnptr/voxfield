# Voxfield - An open source voxel based multiplayer sandbox game.
# Copyright (C) 2022-2024  Nikita Fediuchin
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

#***********************************************************************************************************************
cmake_minimum_required(VERSION 3.22)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "Prevented in-tree build")
endif()

if(TARGET voxfield-client)
	return()
endif()

project(voxfield VERSION 0.1.0 LANGUAGES CXX
	DESCRIPTION "An open source voxel based multiplayer sandbox game"
	HOMEPAGE_URL "https://github.com/cfnptr/voxfield")

option(VOXFIELD_BUILD_SERVER "Build Voxfield server executable." ON)
option(VOXFIELD_BUILD_LAUNCHER "Build Voxfield launcher executable." ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(BUILD_SHARED_LIBS OFF)

include(libraries/garden/cmake/compile-options.cmake)
include(libraries/garden/cmake/publish-functions.cmake)

string(TIMESTAMP VOXFIELD_CURRENT_YEAR "%Y")
set(VOXFIELD_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/include
	libraries/garden/libraries/voxy/include)
set(VOXFIELD_LINK_LIBS garden)

set(GARDEN_APP_NAME "Voxfield")
set(GARDEN_APP_DESCRIPTION ${Voxfield_DESCRIPTION})
set(GARDEN_APP_CREATOR "Nikita Fediuchin")
set(GARDEN_APP_COPYRIGHT "2022-${VOXFIELD_CURRENT_YEAR} Nikita Fediuchin. All rights reserved.")
set(GARDEN_APP_VERSION_MAJOR ${Voxfield_VERSION_MAJOR})
set(GARDEN_APP_VERSION_MINOR ${Voxfield_VERSION_MINOR})
set(GARDEN_APP_VERSION_PATCH ${Voxfield_VERSION_PATCH})
set(GARDEN_APP_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
set(GARDEN_APP_CACHES_DIR ${PROJECT_SOURCE_DIR}/.caches)

# set(GARDEN_RELEASE_EDITOR ON CACHE BOOL "" FORCE)
# set(GARDEN_RELEASE_DEBUGGING ON CACHE BOOL "" FORCE)
set(GARDEN_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libraries/garden)

configure_file(libraries/garden/cmake/defines.hpp.in include/garden/app-defines.hpp)

#***********************************************************************************************************************
file(GLOB_RECURSE VOXFIELD_CORE_SOURCES source/core/*.cpp)
file(GLOB_RECURSE VOXFIELD_CLIENT_SOURCES source/client/*.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# configure_file(examples/platformer/resources/images/platformer.ico platformer.ico COPYONLY)
	configure_file(libraries/garden/cmake/windows.rc.in client.rc)
	add_executable(voxfield-client $<$<CONFIG:Release>:WIN32> $<$<CONFIG:RelWithDebInfo>:WIN32>
		source/client.cpp ${VOXFIELD_CLIENT_SOURCES} ${VOXFIELD_CORE_SOURCES} ${Voxfield_BINARY_DIR}/client.rc)
	else()
		add_executable(voxfield-client source/client.cpp
			${VOXFIELD_CLIENT_SOURCES} ${VOXFIELD_CORE_SOURCES})
	endif()
endif()

target_include_directories(voxfield-client PUBLIC ${VOXFIELD_INCLUDE_DIRS})
target_link_libraries(voxfield-client ${VOXFIELD_LINK_LIBS})
set_target_properties(voxfield-client PROPERTIES OUTPUT_NAME ${GARDEN_APP_NAME})
add_dependencies(voxfield-client packer gslc equi2cube)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	stripExecutable(voxfield-client)
endif()
if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
	packResources(voxfield-client ${GARDEN_APP_CACHES_DIR}
		${GARDEN_APP_RESOURCES_DIR} ${Garden_SOURCE_DIR}/resources
		${GARDEN_RELEASE_EDITOR} ${GARDEN_RELEASE_DEBUGGING})
endif()

if(VOXFIELD_BUILD_SERVER)
	file(GLOB_RECURSE VOXFIELD_SERVER_SOURCES source/server/*.cpp)
	add_executable(voxfield-server source/server.cpp ${VOXFIELD_SERVER_SOURCES})
	target_include_directories(voxfield-server PUBLIC ${VOXFIELD_INCLUDE_DIRS})
	target_link_libraries(voxfield-server ${VOXFIELD_LINK_LIBS})

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		stripExecutable(voxfield-server)
	endif()
endif()