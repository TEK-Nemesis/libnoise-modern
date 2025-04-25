# libnoise-modern

Welcome to **libnoise-modern**, a modernized fork of the classic `libnoise` library, updated to leverage C++17 features and improve cross-platform compatibility. This repository maintains the core functionality of `libnoise` for generating coherent noise, while introducing significant enhancements for performance, usability, and maintainability. The library is ideal for procedural content generation in games, simulations, and visualizations, supporting applications like terrain generation, texture creation, and more.

The original `libnoise` documentation is available at libnoise.sourceforge.net and provides detailed insights into the library's noise modules and usage patterns.

## Overview

`libnoise-modern` builds upon the foundational work of the `libnoise` library, originally developed by **Jason Bevins**. This fork, led by **TEK Nemesis**, modernizes the codebase to align with contemporary C++ standards, enhances build processes, and ensures compatibility with both Windows and Linux platforms. The library retains its core capability to generate Perlin, Voronoi, and other noise types, while introducing optimizations and new features.

### Key Features

- **C++17 Compliance**: The codebase has been fully updated to use C++17 features for improved safety, performance, and readability.
- **Cross-Platform Support**: Build scripts and CMake configurations support Windows and Linux, with simplified execution from the main folder.
- **Multithreading**: Noise generation is thread-safe, allowing parallel processing for performance-critical applications (e.g., terrain generation).
- **Example Executables**: Pre-built examples (e.g., `worms`, `texturejade`, `complexplanet`) are automatically generated in the `bin` folder, showcasing practical applications.
- **Modern Build System**: Separate `CMakeLists.txt` for `libnoise` core and comprehensive build scripts for ease of use.
- **Documentation Reference**: Retains compatibility with the original `libnoise` documentation for module usage and concepts.

## Modernization to C++17

The `libnoise-modern` repository has undergone extensive updates to ensure C++17 compliance and improve code quality. Key changes include:

- **Memory Management**: Replaced raw pointer arrays (`m_pSourceModule`) with `std::vector` for safer and more flexible source module management.
- **Inline Methods**: Moved critical methods like `GetValue` and `GetSourceModuleCount` to headers as inline functions to optimize performance.
- **Modern Syntax**:
  - Used `constexpr` for constants (e.g., `DEFAULT_FREQUENCY`) to ensure compile-time evaluation.
  - Replaced `NULL` with `nullptr` for type safety.
  - Adopted `noexcept` specifiers for functions that do not throw exceptions.
  - Added `override` to virtual functions for clarity and safety.
  - Used `[[nodiscard]]` for getter methods to encourage proper use of return values.
- **Standard Library Usage**: Replaced custom utilities with standard C++ equivalents (e.g., `std::sqrt`, `std::floor`, `std::min`, `std::max`, `std::clamp`).
- **Memory Safety**: Eliminated manual memory management in modules like `Terrace` by using `std::vector` for control points.
- **Error Handling**: Improved assertions and exception handling for robustness (e.g., checking source module pointers).
- **Macro Conflict Resolution**: Added `#undef min` and `#undef max` to avoid Windows macro conflicts with `std::min` and `std::max`.
- **Documentation**: Updated Doxygen comments for consistency and removed redundant group tags.

These changes enhance the library’s reliability, maintainability, and performance while preserving its core functionality.

## Multithreading

`libnoise-modern` is designed to be thread-safe, allowing multiple threads to generate noise simultaneously without data races. This is particularly useful for applications requiring large-scale procedural content generation, such as terrain or texture maps. Key considerations:

- Noise modules are stateless or use immutable data, ensuring safe concurrent access.
- Users can parallelize noise generation across multiple threads (e.g., dividing a terrain grid into chunks).
- Example applications demonstrate thread-safe usage, and the library’s design supports integration with multithreaded game engines or simulations.

## Getting Started

This section guides you through downloading or cloning the `libnoise-modern` repository and preparing your Linux or Windows environment to run the build scripts (`build_linux.sh` for Linux, `build_windows.bat` for Windows). Once set up, you can proceed to the "Build System" section to compile and run the library.

### Step 1: Download or Clone the Repository

- **Download the Release**:
  - Go to [github.com/TEK-Nemesis/libnoise-modern/releases](https://github.com/TEK-Nemesis/libnoise-modern/releases).
  - Find the most recent release and download the source code (e.g. `libnoise-modern-1.1.1.zip`).
  - Extract the archive to a working directory (e.g., `~/projects` on Linux, `C:\Projects` on Windows):
    - **Linux**:
      ```bash
      mkdir -p ~/projects
      mv ~/Downloads/libnoise-modern-1.1.1.zip ~/projects/
      cd ~/projects
      unzip libnoise-modern-1.1.1.zip
      cd libnoise-modern-1.1.1
      ```
    - **Windows**:
      - Use File Explorer to extract `libnoise-modern-1.1.1.zip` to `C:\Projects`.
      - Open Command Prompt and navigate to the folder:
        ```cmd
        cd C:\Projects\libnoise-modern-1.1.1
        ```
- **Clone the Repository** (Alternative):
  - Ensure Git is installed (`git --version`).
  - Clone the repository:
    ```bash
    git clone https://github.com/TEK-Nemesis/libnoise-modern.git
    cd libnoise-modern
    git checkout v1.1.1
    ```

### Step 2: Prepare Your Environment

#### Linux
- **Install Dependencies**:
  - Install a C++17-compliant compiler (e.g., GCC 9+), CMake (3.5+), build tools, and libraries for OpenGL and X11 (required by `freeglut` for examples):
    - **Ubuntu/Debian**:
      ```bash
      sudo apt update
      sudo apt install -y build-essential cmake g++ libgl1-mesa-dev libx11-dev libxrandr-dev
      ```
    - **Fedora**:
      ```bash
      sudo dnf install -y gcc-c++ cmake make mesa-libGL-devel libX11-devel libXrandr-devel
      ```
    - **Arch Linux**:
      ```bash
      sudo pacman -Syu base-devel cmake gcc mesa-libgl libx11 libxrandr
      ```
  - Verify installations:
    ```bash
    g++ --version
    cmake --version
    make --version
    ```
- **Make the Script Executable**:
  - Ensure `build_linux.sh` has execute permissions:
    ```bash
    chmod +x build_linux.sh
    ```

#### Windows
- **Install Visual Studio 2022**:
  - Download and install Visual Studio 2022 Community Edition from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/vs/community/).
  - During installation, select the “Desktop development with C++” workload to include the C++17 compiler (MSVC) and CMake integration.
  - Verify installation by opening Command Prompt and checking:
    ```cmd
    cl
    ```
    You should see the MSVC compiler version (e.g., `cl : Microsoft (R) C/C++ Optimizing Compiler Version 19.XX`).
- **Install CMake**:
  - Download CMake from [cmake.org/download](https://cmake.org/download/) (e.g., Windows win64-x64 Installer).
  - Install CMake and add it to the system PATH during setup.
  - Verify:
    ```cmd
    cmake --version
    ```
- **Install Git (Optional, for Cloning)**:
  - If cloning the repository, download Git for Windows from [git-scm.com](https://git-scm.com/download/win).
  - Install and verify:
    ```cmd
    git --version
    ```
- **Install OpenGL Libraries**:
  - The examples require OpenGL, which Visual Studio typically supports via `opengl32.lib` (included). No additional installation is needed for OpenGL itself.
  - However, the `freeglut` dependency (used by examples) is fetched by CMake and built automatically, so no manual installation is required.

### Step 3: Verify Setup
- Ensure you’re in the `libnoise-modern` root directory:
  - Linux: `~/projects/libnoise-modern-1.1.1`
  - Windows: `C:\Projects\libnoise-modern-1.1.1`
- Confirm the build scripts are present:
  - Linux:
    ```bash
    ls build_linux.sh
    ```
  - Windows:
    ```cmd
    dir build_windows.bat
    ```
- You’re now ready to proceed to the "Build System" section to compile and run the library.

## Build System

The repository includes a robust build system based on CMake, with separate configurations for the core library and utilities:

- **Core Library (**`noise` **folder)**: The `noise` folder contains its own `CMakeLists.txt` for building the `libnoise` library independently. Refer to `noise/README.md` for detailed instructions on building `libnoise`, including dependencies and configuration options.
- **Full Project**: The main folder’s `CMakeLists.txt` supports building `libnoise`, `noiseutils`, and example applications together.

### Build Scripts

To simplify the build process, cross-platform scripts are provided which will display a menu of output options (e.g. creation of DLL, in the main folder:

- **Windows**: `build_windows.bat`
  - Cleans old build artifacts.
  - Creates a `build` directory.
  - Generates a Visual Studio solution using CMake.
  - Builds Debug and Release configurations for the selected target (e.g., `libnoise`, `noiseutils`, examples).
- **Linux**: `build_linux.sh`
  - Performs similar steps to the Windows script, tailored for Linux environments.
  - Uses CMake to generate makefiles and builds the project with `make`.
  - Supports Debug and Release configurations.

<img src="images/script_menu.jpg" alt="Example of the Script Menu" width="600">

**Usage**:

1. Navigate to the main folder (`libnoise-modern`).
2. Run the appropriate script:
   - Windows: `build_windows.bat`
   - Linux: `./build_linux.sh`
3. Select the desired build option (e.g., option 3 for Shared Library with `libnoise`, `noiseutils`, and examples).
4. The script handles configuration and compilation automatically.

### Example Executables

After running the build script, example executables (`.exe` on Windows, ELF binaries on Linux) are automatically generated in the `bin` folder. Examples include:

- `worms`: Demonstrates an unusual application of libnoise to render and animate worm-like creatures in real time.
- `texturejade`, `texturegranite`, `texturewood`, `texturesky`, `textureslime`: Create procedural textures generated by libnoise.
- `complexplanet`: Demonstrates a complex planetary terrain.

These examples showcase the library’s capabilities and serve as starting points for custom applications.

## Documentation

For detailed information on noise modules, their usage, and mathematical foundations, refer to the original `libnoise` documentation at libnoise.sourceforge.net. The `libnoise-modern` library maintains compatibility with these modules, so the documentation remains relevant. Additional details specific to modernization changes and build instructions are available in `noise/README.md`.

## Credits

`libnoise-modern` builds on the foundational work of the original `libnoise` library, created by **Jason Bevins**, whose innovative design made procedural noise generation accessible to developers. Significant contributions to the original library were also made by **Keith Davies**, whose efforts helped refine and expand its capabilities. The modernization effort, led by **TEK Nemesis**, brings the library into the C++17 era, with contributions from the open-source community.

Special thanks to all contributors whose names appear in the source files, including those who provided bug fixes, optimizations, and documentation. Your hard work has made `libnoise` and `libnoise-modern` enduring tools for procedural content generation.

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository at github.com/TEK-Nemesis/libnoise-modern.
2. Create a branch for your feature or bug fix.
3. Submit a pull request with a clear description of your changes.

Please ensure your code adheres to C++17 standards and includes appropriate tests or examples.

## License

`libnoise-modern` is licensed under the GNU Lesser General Public License (LGPL) version 2.1 or later, consistent with the original `libnoise` library. See `COPYING.txt` for details.

## Contact

For issues, feature requests, or questions, please open an issue on the GitHub repository or contact **TEK Nemesis** via GitHub.

---

Happy noise generation!
