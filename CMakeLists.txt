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

# Ensure bin directories exist
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Release")

# Set CMake policy CMP0135 to NEW for FetchContent downloads
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

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
    # Fetch GLEW 2.2.0 from Perlmint/glew-cmake
    message(STATUS "Fetching GLEW...")
    include(FetchContent)
    FetchContent_Declare(
        glew
        URL https://github.com/Perlmint/glew-cmake/archive/refs/tags/glew-cmake-2.2.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # Set GLEW build options
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "Build GLEW as a static library" FORCE)
    set(glew-cmake_BUILD_STATIC ON CACHE BOOL "Build GLEW static library" FORCE)
    set(glew-cmake_BUILD_UTILS OFF CACHE BOOL "Disable GLEW utils (glewinfo, visualinfo)" FORCE)

    FetchContent_MakeAvailable(glew)

    # Set MSVC runtime library for GLEW static library
    set_target_properties(libglew_static PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    # Platform-specific libraries
    if(WIN32)
        set(PLATFORM_LIBRARIES opengl32 user32)
    else()
        find_package(X11 REQUIRED)
        find_package(OpenGL REQUIRED)
        set(PLATFORM_LIBRARIES ${OPENGL_gl_LIBRARY} ${X11_LIBRARIES} GLX)
    endif()

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
            "${glew_SOURCE_DIR}/include"
        )

        # Link libraries
        if(BUILD_NOISEUTILS)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE noiseutils)
        else()
            target_link_libraries(${EXAMPLE_NAME} PRIVATE libnoise)
        endif()

        # Link GLEW and platform-specific libraries for all examples
        target_link_libraries(${EXAMPLE_NAME} PRIVATE libglew_static ${PLATFORM_LIBRARIES})
    endforeach()
endif()