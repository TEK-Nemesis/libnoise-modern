# CMakeLists.txt for libnoise library
#
# This file configures the build for the libnoise library, which provides
# procedural noise generation functions for terrain generation and other applications.
#
# Copyright (C) 2003, 2004 Jason Bevins
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at
# your option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# (COPYING.txt) for more details.
#
# Updated by Grok on April 26, 2025:
# - Enforced C++17 standard compliance with target_compile_features.
# - Organized source and header files into groups for better IDE integration.
# - Made the file self-contained while allowing use as a subdirectory by a parent CMakeLists.txt.
# Updated by Grok on April 27, 2025:
# - Added output directory settings for consistency with the root CMakeLists.txt.

cmake_minimum_required(VERSION 3.15)

# Define the project (only if not already defined by a parent CMakeLists.txt)
if(NOT PROJECT_NAME)
    project(libnoise
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "A library for procedural noise generation"
    )
endif()

# Define source and header files for libnoise
# Note: If you add or remove files, re-run CMake to update the file list
file(GLOB_RECURSE LIBNOISE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/module/*.cpp"
)

file(GLOB_RECURSE LIBNOISE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/noise/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/noise/model/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/noise/module/*.h"
)

# Create libnoise library
add_library(libnoise ${LIBNOISE_SOURCES} ${LIBNOISE_HEADERS})

# Enforce C++17 standard
target_compile_features(libnoise PUBLIC cxx_std_17)

# Set include directories for libnoise
target_include_directories(libnoise
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Automatically export all symbols on Windows for shared libraries
if(BUILD_SHARED_LIBS AND WIN32)
    set_target_properties(libnoise PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# Set properties for libnoise (only if VERSION is defined)
if(DEFINED PROJECT_VERSION)
    set_target_properties(libnoise PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# Set output directories for consistency
set_target_properties(libnoise PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
)

# Source group for IDEs (e.g., Visual Studio)
source_group("Source Files" FILES ${LIBNOISE_SOURCES})
source_group("Header Files" FILES ${LIBNOISE_HEADERS})