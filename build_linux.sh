#!/bin/bash

# build_linux.sh
#
# This script builds the libnoise-modern project on Linux using CMake.
# It provides a menu to select build options (shared/static library,
# include noiseutils, include examples) and builds both Debug and Release
# configurations.
#
# The script must be run in the same directory as CMakeLists.txt.
#
# Usage:
#   chmod +x build_linux.sh
#   ./build_linux.sh
#
# The script does not launch an IDE after the build (unlike the Windows
# equivalent, which optionally launches Visual Studio). If you need to open
# the project in an IDE, you can generate project files for your preferred
# IDE using CMake (e.g., cmake -G "CodeBlocks - Unix Makefiles").
#

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for cmake
echo "Searching for cmake..."
if ! command_exists cmake; then
    echo "ERROR: cmake not found in the system PATH."
    echo "Please ensure CMake is installed and added to the PATH."
    exit 1
fi
echo "Found cmake: $(cmake --version | head -n1)"

# Check for make
echo "Searching for make..."
if ! command_exists make; then
    echo "ERROR: make not found in the system PATH."
    echo "Please ensure make is installed and added to the PATH."
    exit 1
fi
echo "Found make: $(make --version | head -n1)"

# Check if CMakeLists.txt exists in the current directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "ERROR: CMakeLists.txt not found in the current directory."
    exit 1
fi

# Set the source directory to the current directory
SOURCE_DIR=$(pwd)
BUILD_DIR_DEBUG="$SOURCE_DIR/build/Debug"
BUILD_DIR_RELEASE="$SOURCE_DIR/build/Release"

# Function to display the menu
display_menu() {
    clear
    echo "==========================================================="
    echo "        Build Configuration Menu for libnoise-modern"
    echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
    echo "   Github: https://github.com/TEK-Nemesis/libnoise-modern"
    echo "==========================================================="
    echo
    echo "Shared Library Builds (Debug and Release):"
    echo "1. Build Shared Library, libnoise only"
    echo "2. Build Shared Library, libnoise with noiseutils only"
    echo "3. Build Shared Library, libnoise with noiseutils and examples"
    echo
    echo "Static Library Builds (Debug and Release):"
    echo "4. Build Static Library, libnoise only"
    echo "5. Build Static Library, libnoise with noiseutils only"
    echo "6. Build Static Library, libnoise with noiseutils and examples"
    echo
    echo "7. Exit"
    echo
}

# Function to handle the build process for a single configuration
build_config() {
    local BUILD_DIR=$1
    local CONFIG=$2
    local SHARED_LIBS=$3
    local NOISEUTILS=$4
    local EXAMPLES=$5

    echo "Creating build directory for $CONFIG..."
    mkdir -p "$BUILD_DIR"
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to create build directory: $BUILD_DIR"
        exit 1
    fi

    echo "Generating build system for $CONFIG in $BUILD_DIR..."
    cd "$BUILD_DIR"
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$CONFIG -DBUILD_SHARED_LIBS="$SHARED_LIBS" -DBUILD_NOISEUTILS="$NOISEUTILS" -DBUILD_EXAMPLES="$EXAMPLES" "$SOURCE_DIR"
    if [ $? -ne 0 ]; then
        echo "ERROR: CMake generation for $CONFIG failed. Please check the output above for details."
        cd "$SOURCE_DIR"
        exit 1
    fi

    echo "Building $CONFIG configuration..."
    make -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "ERROR: $CONFIG build failed. Please check the output above for details."
        cd "$SOURCE_DIR"
        exit 1
    fi

    cd "$SOURCE_DIR"
}

# Function to handle the build process for both Debug and Release
build_project() {
    local SHARED_LIBS=$1
    local NOISEUTILS=$2
    local EXAMPLES=$3

    # Delete old build directories in the root and subdirectories
    echo "Cleaning old build artifacts..."
    rm -rf "$SOURCE_DIR/build"
    for dir in noise examples noiseutils; do
        if [ -d "$SOURCE_DIR/$dir/build" ]; then
            rm -rf "$SOURCE_DIR/$dir/build"
        fi
    done

    # Build Debug configuration
    build_config "$BUILD_DIR_DEBUG" "Debug" "$SHARED_LIBS" "$NOISEUTILS" "$EXAMPLES"

    # Build Release configuration
    build_config "$BUILD_DIR_RELEASE" "Release" "$SHARED_LIBS" "$NOISEUTILS" "$EXAMPLES"

    # Ensure the bin directory exists and copy all artifacts to a unified bin directory
    echo "Copying build artifacts to $SOURCE_DIR/bin..."
    mkdir -p "$SOURCE_DIR/bin/Debug"
    mkdir -p "$SOURCE_DIR/bin/Release"

    # Copy shared libraries and executables
    cp -f "$BUILD_DIR_DEBUG/libnoise.so" "$SOURCE_DIR/bin/Debug/" 2>/dev/null || true
    cp -f "$BUILD_DIR_DEBUG/noiseutils.so" "$SOURCE_DIR/bin/Debug/" 2>/dev/null || true
    cp -f "$BUILD_DIR_DEBUG/"*.exe "$SOURCE_DIR/bin/Debug/" 2>/dev/null || true
    cp -f "$BUILD_DIR_RELEASE/libnoise.so" "$SOURCE_DIR/bin/Release/" 2>/dev/null || true
    cp -f "$BUILD_DIR_RELEASE/noiseutils.so" "$SOURCE_DIR/bin/Release/" 2>/dev/null || true
    cp -f "$BUILD_DIR_RELEASE/"*.exe "$SOURCE_DIR/bin/Release/" 2>/dev/null || true

    echo "Build completed successfully!"
}

# Main menu loop
while true; do
    display_menu
    read -p "Select an option (1-7): " CHOICE

    case $CHOICE in
        1)
            build_project "ON" "OFF" "OFF"
            break
            ;;
        2)
            build_project "ON" "ON" "OFF"
            break
            ;;
        3)
            build_project "ON" "ON" "ON"
            break
            ;;
        4)
            build_project "OFF" "OFF" "OFF"
            break
            ;;
        5)
            build_project "OFF" "ON" "OFF"
            break
            ;;
        6)
            build_project "OFF" "ON" "ON"
            break
            ;;
        7)
            echo "Exiting..."
            exit 0
            ;;
        *)
            echo "Invalid option. Please select a number between 1 and 7."
            read -p "Press Enter to continue..."
            ;;
    esac
done

# Pause to allow the user to see the output
read -p "Press Enter to exit..."