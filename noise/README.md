# libnoise Core Library

## Overview

The `noise` folder contains the core `libnoise` library, the heart of the `libnoise-modern` repository (version 1.1.0). Originally developed by **Jason Bevins**, `libnoise` is a powerful library for generating coherent noise, widely used for procedural content creation in games, simulations, and visualizations. This modernized version, updated by **TEK Nemesis**, leverages C++17 features to enhance performance, safety, and cross-platform compatibility while preserving the original functionality.

For detailed information on noise modules, their usage, and mathematical foundations, refer to the original `libnoise` documentation at libnoise.sourceforge.net. The main `libnoise-modern` README (`../README.markdown`) provides a comprehensive overview of the repository, including modernization details and features.

## Purpose

The `noise` folder houses the standalone `libnoise` library, which includes noise generation modules (e.g., Perlin, Voronoi, RidgedMulti) and supporting utilities. It is designed to be built independently or as part of the broader `libnoise-modern` project, which includes `noiseutils` and example applications.

## Build Instructions

The `noise` folder includes its own `CMakeLists.txt` for building the `libnoise` library independently. However, for the easiest and most streamlined experience, we recommend using the build scripts in the main `libnoise-modern` folder (`../`), which can isolate the `libnoise` build process while also supporting `noiseutils` and examples.

### Using Main Folder Build Scripts (Recommended)

To build `libnoise` with minimal effort, use the cross-platform scripts in the main folder:

- **Windows**: Run `build_windows.bat` from the `libnoise-modern` root.
- **Linux**: Run `./build_linux.sh` from the `libnoise-modern` root.

These scripts:
- Clean old build artifacts.
- Create a `build` directory.
- Generate build files using CMake (Visual Studio solution on Windows, makefiles on Linux).
- Compile `libnoise` (and optionally `noiseutils` and examples) in Debug and Release configurations.
- Place the compiled library (`libnoise.dll` or `libnoise.so`) and example executables in the `bin` folder.

To build only `libnoise`, select option 1 (Shared Library, libnoise only) when prompted by the script.

### Manual Build with CMake

If you prefer to build `libnoise` independently using the `CMakeLists.txt` in this folder:

1. **Navigate to the noise folder**:
   ```bash
   cd noise
   ```

2. **Create a build directory**:
   ```bash
   mkdir build && cd build
   ```

3. **Generate build files**:
   - Windows (Visual Studio):
     ```bash
     cmake -G "Visual Studio 17 2022" -A x64 ..
     ```
   - Linux:
     ```bash
     cmake ..
     ```

4. **Build the library**:
   - Windows:
     ```bash
     cmake --build . --config Debug
     cmake --build . --config Release
     ```
   - Linux:
     ```bash
     make
     ```

5. **Output**:
   - The compiled library will be in `noise/build/lib` (e.g., `libnoise.lib` or `libnoise.a`).
   - The shared library (if enabled) will be in `noise/build/bin` (e.g., `libnoise.dll` or `libnoise.so`).

**Note**: The main folder’s scripts automate these steps and provide additional options, making them the preferred approach for most users.

## Dependencies

- **C++17 Compiler**: Visual Studio 2022 (Windows) or GCC/Clang (Linux).
- **CMake**: Version 3.5 or higher for generating build files.

No external libraries are required for `libnoise` itself, but `noiseutils` and examples may depend on `libnoise` and additional libraries (e.g., `freeglut` for examples).

## License

`libnoise` is licensed under the GNU Lesser General Public License (LGPL) version 2.1 or later. See `../COPYING.txt` in the `libnoise-modern` root for details.

## Documentation

- **Original libnoise Documentation**: Explore libnoise.sourceforge.net for detailed explanations of noise modules, their applications, and usage examples.
- **Main Repository README**: Refer to `../README.markdown` for an overview of `libnoise-modern`, including C++17 modernization details, multithreading support, and build script usage.
- **Source Code Comments**: The `noise` folder’s header files (e.g., `module/perlin.h`, `module/voronoi.h`) include updated Doxygen comments for class and method details.

## Credits

The `libnoise` library was created by **Jason Bevins**, whose visionary work made procedural noise generation accessible to developers worldwide. **Keith Davies** contributed significantly to the original project, enhancing its capabilities. The modernization effort, led by **TEK Nemesis**, brings `libnoise` into the C++17 era, with support from the open-source community. Thanks to all contributors whose efforts are reflected in the source files.

## Contributing

Contributions to `libnoise` are welcome as part of the `libnoise-modern` repository:

1. Fork the repository at github.com/TEK-Nemesis/libnoise-modern.
2. Create a branch for your feature or bug fix.
3. Submit a pull request with a clear description of your changes.

Ensure code adheres to C++17 standards and aligns with the modernization guidelines in the main README.

## Contact

For questions about the original `libnoise` library, contact **Jason Bevins** at jlbezigvins@gmzigail.com (remove every 'zig' for the correct email). For inquiries about `libnoise-modern`, contact **TEK Nemesis** via GitHub or open an issue at github.com/TEK-Nemesis/libnoise-modern.

---

Happy noise generation!