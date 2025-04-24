# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Define the project name
project(libnoise-modern
    VERSION 1.0.0
    DESCRIPTION "Modernized libnoise library with noiseutils and examples"
    LANGUAGES CXX
)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories for all targets to ensure consistent placement
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# For multi-config generators (like Visual Studio), set per-config directories
foreach(CONFIG_TYPE Debug Release)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${CMAKE_BINARY_DIR}/lib/${CONFIG_TYPE}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${CMAKE_BINARY_DIR}/lib/${CONFIG_TYPE}")
endforeach()

# Options for build configuration
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(BUILD_NOISEUTILS "Build the noiseutils library" ON)
option(BUILD_EXAMPLES "Build example applications" ON)

# Add the noise subdirectory (libnoise library)
add_subdirectory(noise)

# Define libnoise target alias (correct target name from noise/CMakeLists.txt)
add_library(libnoise ALIAS libnoise)

# Define the noiseutils library if enabled
if(BUILD_NOISEUTILS)
    # Define the noiseutils library
    add_library(noiseutils
        "${CMAKE_SOURCE_DIR}/noiseutils/noiseutils.cpp"
    )
    target_include_directories(noiseutils PUBLIC
        "${CMAKE_SOURCE_DIR}/noiseutils"
        "${CMAKE_SOURCE_DIR}/noise/include"
    )
    target_link_libraries(noiseutils PUBLIC libnoise)

    # Set properties for shared library if applicable
    if(BUILD_SHARED_LIBS AND WIN32)
        set_target_properties(noiseutils PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()
endif()

# Add the examples if enabled
if(BUILD_EXAMPLES)
    # Fetch FreeGLUT dependency for worms example
    include(FetchContent)
    FetchContent_Declare(
        freeglut
        GIT_REPOSITORY https://github.com/freeglut/freeglut.git
        GIT_TAG v3.6.0
    )

    # Set FreeGLUT options before making it available
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "Build FreeGLUT as a shared library" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "Build FreeGLUT as a static library" FORCE)
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "Disable FreeGLUT demos" FORCE)

    FetchContent_MakeAvailable(freeglut)

    # FreeGLUT requires OpenGL
    find_package(OpenGL REQUIRED)

    # Define source files for examples
    file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/*.cpp")

    # Create an executable for each example source file
    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
        
        # Set include directories
        target_include_directories(${EXAMPLE_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/noise/include"
            "${CMAKE_SOURCE_DIR}/noiseutils"
        )

        # Link libraries
        if(BUILD_NOISEUTILS)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE noiseutils)
        else()
            target_link_libraries(${EXAMPLE_NAME} PRIVATE libnoise)
        endif()

        # Link FreeGLUT and OpenGL for worms example
        if(EXAMPLE_NAME STREQUAL "worms")
            target_include_directories(${EXAMPLE_NAME} PRIVATE
                "${freeglut_SOURCE_DIR}/include"
            )
            target_link_libraries(${EXAMPLE_NAME} PRIVATE
                freeglut_static
                OpenGL::GL
            )
        endif()

        # If building shared libraries, copy DLLs to the executable's output directory
        if(BUILD_SHARED_LIBS)
            # Determine the configuration-specific output directory
            if(CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_GENERATOR MATCHES "Ninja Multi-Config")
                set(EXECUTABLE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
                set(DLL_SOURCE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
            else()
                set(EXECUTABLE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
                set(DLL_SOURCE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
            endif()

            # Copy libnoise.dll and noiseutils.dll to the executable's directory
            add_custom_command(TARGET ${EXAMPLE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_SOURCE_DIR}/libnoise.dll"
                    "${EXECUTABLE_OUTPUT_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_SOURCE_DIR}/noiseutils.dll"
                    "${EXECUTABLE_OUTPUT_DIR}"
                COMMENT "Copying libnoise.dll and noiseutils.dll to ${EXECUTABLE_OUTPUT_DIR}"
            )
        endif()
    endforeach()
endif()