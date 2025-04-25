# NoiseUtils Library

## Overview

The `noiseutils` library, originally authored by **Jason Bevins** between 2003 and 2005, is a companion to the `libnoise` library, providing utilities for generating and rendering two-dimensional noise maps. It includes classes for creating noise maps, rendering them as images, and exporting them to file formats such as Windows Bitmap (BMP) and Terragen Terrain (TER). As part of the `libnoise-modern` repository, `noiseutils` has been updated for C++17 compliance, cross-platform compatibility, and improved performance while preserving its original functionality. This library is ideal for procedural content generation in applications like terrain visualization, texture creation, and game development.

The `noiseutils` library is designed to work seamlessly with the `libnoise` core library, which is housed in the `noise` folder of the `libnoise-modern` repository. For detailed information on noise modules and their usage, refer to the original `libnoise` documentation at libnoise.sourceforge.net.

## Files

- `noiseutils.h`: Header file containing class declarations and utility functions for noise map generation, image rendering, and file writing.
- `noiseutils.cpp`: Implementation file for the `noiseutils` classes and methods.

## Integration with libnoise-modern

The `noiseutils` library is a component of the `libnoise-modern` repository, which modernizes the original `libnoise` project. It depends on the `libnoise` core library, located in the `noise` folder, and is typically built alongside it using the repository’s build scripts. For the simplest build experience, use the Windows or Linux scripts in the main `libnoise-modern` folder, which handle the compilation of `libnoise`, `noiseutils`, and example applications.

## Changes for C++17 Compatibility

The `noiseutils` library was modernized by **TEK Nemesis** and **Grok** on April 22, 2025, to align with C++17 standards, enhance cross-platform compatibility (Windows and Linux), and address issues in the original code while preserving Jason Bevins’ intent. Key updates include:

### General Changes

- **Modern C++ Practices**:
  - Replaced raw pointer memory management with `std::vector` and `std::unique_ptr` for safer and more efficient memory handling.
  - Added `noexcept` specifiers to methods that do not throw exceptions, improving exception safety.
  - Replaced `typedef` with `using` for type aliases (e.g., `NoiseMapCallback`) for consistency with modern C++.
  - Removed unnecessary includes (e.g., `<string.h>`, `<stdlib.h>`) and added C++17-compliant includes (e.g., `<algorithm>`, `<numeric>`, `<filesystem>`).
- **Cross-Platform Compatibility**:
  - Adopted `std::filesystem` for file path handling to ensure compatibility across Windows and Linux.
- **Warnings and Errors**:
  - Removed `[[nodiscard]]` from utility functions (`UnpackFloat`, `UnpackLittle16`, `UnpackLittle32`) to resolve warnings about discarded return values, as these are often used for side effects.
  - Fixed linker warnings (e.g., VC7001) by implementing missing `Image` class methods.

### Specific Changes to `noiseutils.h`

- **File Path Handling**:
  - Updated the `Writer` base class to use `std::filesystem::path` for `m_destFilename` instead of `std::string`, ensuring robust cross-platform file operations.
  - Modified `SetDestFilename` to accept `std::string_view` for efficiency and assign to `std::filesystem::path`.
- **Class Modernization**:
  - **Image Class**:
    - Replaced `m_pImage` (raw pointer) with `m_values` (`std::vector<Color>`) for modern memory management.
    - Removed `m_memUsed` as it’s redundant with `std::vector`’s size tracking.
    - Added `noexcept` to methods like `GetValue`, `SetValue`, and `GetBorderValue` where appropriate.
  - **NoiseMap Class**:
    - Replaced raw pointer storage with `std::vector<float>` for `m_values`, eliminating `m_memUsed`.
  - **GradientColor Class**:
    - Changed `m_pGradientPoints` from a raw pointer to `std::unique_ptr<GradientPoint[]>` for safer memory management.

### Specific Changes to `noiseutils.cpp`

- **Cross-Platform File Operations**:
  - Updated `WriterBMP::WriteDestFile` and `WriterTER::WriteDestFile` to use `std::filesystem::path::string()` for file paths, ensuring compatibility on Linux and Windows.
- **Image Class Implementations**:
  - Added implementations for `Image` class methods to resolve linker issues, aligning with the original behavior:
    - **Default Constructor**: Uses member initializers for an empty state.
    - **Dimension Constructor**: Calls `SetSize` to initialize the image.
    - **Copy Constructor**: Performs a deep copy using `std::vector`, fixing the original’s incorrect `memcpy` size (`sizeof(float)` instead of `sizeof(Color)`).
    - **Destructor**: Relies on `std::vector` for cleanup.
    - **Assignment Operator**: Implements deep copy with `std::vector`.
    - **Clear**: Uses `std::fill` for efficiency.
    - **GetValue**: Returns the value or border value for out-of-bounds access.
    - **ReclaimMem**: Reallocates to remove stride padding, correcting `memcpy` size.
    - **SetSize**: Resizes using `std::vector::resize` with proper validation.
    - **SetValue**: Sets values within bounds.
    - **TakeOwnership**: Uses `std::move` for ownership transfer.
- **Bug Fixes**:
  - Fixed integer overflow in `BlendChannel` using `std::clamp` to ensure results stay within `uint8` range (0-255). Updated `LinearInterpColor` to use the fixed `BlendChannel`.
- **Performance Improvements**:
  - Used `std::move` in methods like `TakeOwnership` and `ReclaimMem` to optimize resource transfer.
  - Replaced manual loops with standard algorithms (e.g., `std::fill`) where applicable.

### Preservation of Original Intent

- All changes maintain Jason Bevins’ original functionality, verified by comparing implementations with the original `noiseutils-orig.cpp`.
- Fixed bugs (e.g., `memcpy` size in `Image::CopyImage`) while ensuring behavioral equivalence.
- Ensured `Image` class behavior aligns with `NoiseMap` class, as intended in the original design.

## Usage

### Dependencies

- **libnoise**: The core noise generation library, located in the `noise` folder. `noiseutils` requires `libnoise` to be built and linked.
- **C++17 Compiler**: Visual Studio 2022 (Windows) or GCC/Clang (Linux).
- **CMake**: For generating build files.

### Building

For the simplest build experience, use the build scripts in the main `libnoise-modern` folder:

- **Windows**: Run `build_windows.bat` from the main folder.
- **Linux**: Run `./build_linux.sh` from the main folder.

These scripts:

- Clean old build artifacts.
- Create a `build` directory.
- Generate build files using CMake (Visual Studio solution on Windows, makefiles on Linux).
- Compile `libnoise`, `noiseutils`, and examples (if selected) in Debug and Release configurations.
- Place compiled libraries and example executables in the `bin` folder.

Alternatively, you can build `noiseutils` manually by including it in a CMake project that links against `libnoise`. Refer to the main `CMakeLists.txt` in the `libnoise-modern` root or the `noise/README.md` for details on building `libnoise`.

### Example Executables

When building with the main folder’s scripts (e.g., option 3: Shared Library with `libnoise`, `noiseutils`, and examples), example executables are automatically generated in the `bin` folder:

- Windows: `.exe` files (e.g., `worms.exe`, `texturejade.exe`).
- Linux: ELF binaries (e.g., `worms`, `texturejade`).
- Examples include `worms`, `texturejade`, `texturegranite`, `texturewood`, `texturesky`, `textureslime`, and `complexplanet`, demonstrating noise map rendering and file output.

### Key Classes

- **NoiseMap**: Stores floating-point noise values for 2D maps.
- **Image**: Stores color values for rendering noise maps as images.
- **NoiseMapBuilder** (and derived classes like `NoiseMapBuilderPlane`): Generates noise maps from `libnoise` modules.
- **RendererImage** and **RendererNormalMap**: Renders noise maps into images with customizable colors and lighting.
- **WriterBMP** and **WriterTER**: Exports noise maps to BMP and TER file formats.

## Documentation

- **Original libnoise Documentation**: For noise module usage and concepts, refer to libnoise.sourceforge.net.
- **noise Folder README**: See `noise/README.md` for details on the `libnoise` core library, including build instructions and modernization changes.
- **Source Code Comments**: The `noiseutils.h` and `noiseutils.cpp` files include updated Doxygen comments detailing class and method functionality.

## Credits

The `noiseutils` library was originally created by **Jason Bevins**, whose pioneering work on `libnoise` and its utilities laid the foundation for procedural noise generation. The modernization effort, led by **TEK Nemesis**, brings `noiseutils` into the C++17 era, with assistance from **Grok**. Thanks to **Keith Davies** and other contributors to the original `libnoise` project, whose efforts indirectly supported `noiseutils`. The `libnoise-modern` community appreciates all contributions, including bug fixes and optimizations found in the source files.

## Contributing

Contributions to `noiseutils` are welcome as part of the `libnoise-modern` repository:

1. Fork the repository at github.com/TEK-Nemesis/libnoise-modern.
2. Create a branch for your changes.
3. Submit a pull request with a clear description of your updates.

Ensure code adheres to C++17 standards and includes tests or examples demonstrating functionality.

## License

`noiseutils` is licensed under the GNU Lesser General Public License (LGPL) version 2.1 or later, consistent with `libnoise`. See `COPYING.txt` in the `libnoise-modern` root for details.

## Contact

For questions about the original `noiseutils` library, contact **Jason Bevins** at jlbezigvins@gmzigail.com (remove every 'zig' for the correct email). For inquiries about `libnoise-modern` or these updates, contact **TEK Nemesis** via GitHub or open an issue at github.com/TEK-Nemesis/libnoise-modern.

---

Happy noise mapping!
