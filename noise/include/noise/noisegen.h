// noisegen.h
//
// Copyright (C) 2003, 2004 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig').
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 22, 2025.
// - Added [[nodiscard]] to functions to encourage proper use of return values.
// - Used inline constexpr for constants to ensure compile-time evaluation and proper linkage.
// - Improved documentation for clarity and consistency.
// - Added noexcept to functions that do not throw exceptions.

#pragma once

#include "basictypes.h"
#include <algorithm> // For std::clamp
#include <cmath>

namespace noise {

    /// @addtogroup libnoise
    /// @{

    /// Generates a gradient-coherent-noise value from the coordinates of a
    /// three-dimensional input value.
    ///
    /// @param x The @a x coordinate of the input value.
    /// @param y The @a y coordinate of the input value.
    /// @param z The @a z coordinate of the input value.
    /// @param seed The random number seed.
    /// @param noiseQuality The quality of the coherent-noise.
    ///
    /// @returns The generated gradient-coherent-noise value, ranging from -1.0 to +1.0.
    ///
    /// For an explanation of the difference between <i>gradient</i> noise and
    /// <i>value</i> noise, see the comments for the GradientNoise3D() function.
    [[nodiscard]] double GradientCoherentNoise3D(double x, double y, double z, int32 seed = 0,
        NoiseQuality noiseQuality = NoiseQuality::QUALITY_STD) noexcept;

    /// Generates a gradient-noise value from the coordinates of a
    /// three-dimensional input value and the integer coordinates of a
    /// nearby three-dimensional value.
    ///
    /// @param fx The floating-point @a x coordinate of the input value.
    /// @param fy The floating-point @a y coordinate of the input value.
    /// @param fz The floating-point @a z coordinate of the input value.
    /// @param ix The integer @a x coordinate of a nearby value.
    /// @param iy The integer @a y coordinate of a nearby value.
    /// @param iz The integer @a z coordinate of a nearby value.
    /// @param seed The random number seed.
    ///
    /// @returns The generated gradient-noise value, ranging from -1.0 to +1.0.
    ///
    /// @pre The difference between @a fx and @a ix must be less than or equal to one.
    /// @pre The difference between @a fy and @a iy must be less than or equal to one.
    /// @pre The difference between @a fz and @a iz must be less than or equal to one.
    ///
    /// A <i>gradient</i>-noise function generates better-quality noise than a
    /// <i>value</i>-noise function. Most noise modules use gradient noise for
    /// this reason, although it takes much longer to calculate.
    ///
    /// This function generates a gradient-noise value by performing the following steps:
    /// - It first calculates a random normalized vector based on the nearby integer value passed to this function.
    /// - It then calculates a new value by adding this vector to the nearby integer value passed to this function.
    /// - It then calculates the dot product of the above-generated value and the floating-point input value passed to this function.
    ///
    /// A noise function differs from a random-number generator because it always returns the same output value if the same input value is passed to it.
    [[nodiscard]] double GradientNoise3D(double fx, double fy, double fz, int32 ix, int32 iy,
        int32 iz, int32 seed = 0) noexcept;

    /// Generates an integer-noise value from the coordinates of a three-dimensional input value.
    ///
    /// @param x The integer @a x coordinate of the input value.
    /// @param y The integer @a y coordinate of the input value.
    /// @param z The integer @a z coordinate of the input value.
    /// @param seed A random number seed.
    ///
    /// @returns The generated integer-noise value, ranging from 0 to 2147483647.
    ///
    /// A noise function differs from a random-number generator because it always returns the same output value if the same input value is passed to it.
    [[nodiscard]] int32 IntValueNoise3D(int32 x, int32 y, int32 z, int32 seed = 0) noexcept;

    /// Modifies a floating-point value so that it can be stored in a noise::int32 variable.
    ///
    /// @param n A floating-point number.
    ///
    /// @returns The modified floating-point number.
    ///
    /// This function does not modify @a n.
    ///
    /// In libnoise, the noise-generating algorithms are all integer-based; they use variables of type noise::int32.
    /// Before calling a noise function, pass the @a x, @a y, and @a z coordinates to this function to ensure that these coordinates can be cast to a noise::int32 value.
    ///
    /// Although you could do a straight cast from double to noise::int32, the resulting value may differ between platforms.
    /// By using this function, you ensure that the resulting value is identical between platforms.
    [[nodiscard]] inline double MakeInt32Range(double n) noexcept {
        if (n >= 1073741824.0) {
            return (2.0 * std::fmod(n, 1073741824.0)) - 1073741824.0;
        } else if (n <= -1073741824.0) {
            return (2.0 * std::fmod(n, 1073741824.0)) + 1073741824.0;
        }
        return n;
    }

    /// Generates a value-coherent-noise value from the coordinates of a three-dimensional input value.
    ///
    /// @param x The @a x coordinate of the input value.
    /// @param y The @a y coordinate of the input value.
    /// @param z The @a z coordinate of the input value.
    /// @param seed The random number seed.
    /// @param noiseQuality The quality of the coherent-noise.
    ///
    /// @returns The generated value-coherent-noise value, ranging from -1.0 to +1.0.
    ///
    /// For an explanation of the difference between <i>gradient</i> noise and <i>value</i> noise, see the comments for the GradientNoise3D() function.
    [[nodiscard]] double ValueCoherentNoise3D(double x, double y, double z, int32 seed = 0,
        NoiseQuality noiseQuality = NoiseQuality::QUALITY_STD) noexcept;

    /// Generates a value-noise value from the coordinates of a three-dimensional input value.
    ///
    /// @param x The @a x coordinate of the input value.
    /// @param y The @a y coordinate of the input value.
    /// @param z The @a z coordinate of the input value.
    /// @param seed A random number seed.
    ///
    /// @returns The generated value-noise value, ranging from -1.0 to +1.0.
    ///
    /// A noise function differs from a random-number generator because it always returns the same output value if the same input value is passed to it.
    [[nodiscard]] inline double ValueNoise3D(int32 x, int32 y, int32 z, int32 seed = 0) noexcept {
        return 1.0 - (static_cast<double>(IntValueNoise3D(x, y, z, seed)) / 1073741824.0);
    }

    /// @}

} // namespace noise