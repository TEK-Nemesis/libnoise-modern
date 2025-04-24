// noisegen.cpp
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 25, 2025.
// - Optimized FastFloor using std::floor for better performance.
// - Removed unused thread_local std::mt19937 generator to reduce overhead.
// - Used inline constexpr for noise constants to ensure compile-time evaluation.
// - Optimized index computation in GradientNoise3D for better performance.
// - Used const variables where applicable to enable compiler optimizations.
// - Removed MakeInt32Range implementation since it is now inline in noisegen.h.
// - Removed ValueNoise3D implementation since it is now inline in noisegen.h.

#include <noise/noisegen.h>
#include <noise/interp.h>
#include <noise/vectortable.h>

namespace noise {

    // Specifies the version of the coherent-noise functions to use.
    // - Set to 2 to use the current version.
    // - Set to 1 to use the flawed version from the original version of libnoise.
    inline constexpr int NOISE_VERSION = 2;

    // These constants control certain parameters that all coherent-noise functions require.
#if (NOISE_VERSION == 1)
    // Constants used by the original version of libnoise.
    // Because X_NOISE_GEN is not relatively prime to the other values, and
    // Z_NOISE_GEN is close to 256 (the number of random gradient vectors),
    // patterns show up in high-frequency coherent noise.
    inline constexpr int X_NOISE_GEN = 1;
    inline constexpr int Y_NOISE_GEN = 31337;
    inline constexpr int Z_NOISE_GEN = 263;
    inline constexpr int SEED_NOISE_GEN = 1013;
    inline constexpr int SHIFT_NOISE_GEN = 13;
#else
    // Constants used by the current version of libnoise.
    inline constexpr int X_NOISE_GEN = 1619;
    inline constexpr int Y_NOISE_GEN = 31337;
    inline constexpr int Z_NOISE_GEN = 6971;
    inline constexpr int SEED_NOISE_GEN = 1013;
    inline constexpr int SHIFT_NOISE_GEN = 8;
#endif

    namespace {

        // Optimized FastFloor function using std::floor to avoid branching
        inline int32 FastFloor(double x) noexcept {
            return static_cast<int32>(std::floor(x));
        }

    }

    double GradientCoherentNoise3D(double x, double y, double z, int32 seed,
        NoiseQuality noiseQuality) noexcept {
        // Create a unit-length cube aligned along an integer boundary. This cube surrounds the input point.
        const int32 x0 = FastFloor(x);
        const int32 x1 = x0 + 1;
        const int32 y0 = FastFloor(y);
        const int32 y1 = y0 + 1;
        const int32 z0 = FastFloor(z);
        const int32 z1 = z0 + 1;

        // Map the difference between the coordinates of the input value and the
        // coordinates of the cube's outer-lower-left vertex onto an S-curve.
        double xs = 0, ys = 0, zs = 0;
        switch (noiseQuality) {
            case NoiseQuality::QUALITY_FAST:
                xs = (x - static_cast<double>(x0));
                ys = (y - static_cast<double>(y0));
                zs = (z - static_cast<double>(z0));
                break;
            case NoiseQuality::QUALITY_STD:
                xs = SCurve3(x - static_cast<double>(x0));
                ys = SCurve3(y - static_cast<double>(y0));
                zs = SCurve3(z - static_cast<double>(z0));
                break;
            case NoiseQuality::QUALITY_BEST:
                xs = SCurve5(x - static_cast<double>(x0));
                ys = SCurve5(y - static_cast<double>(y0));
                zs = SCurve5(z - static_cast<double>(z0));
                break;
        }

        // Now calculate the noise values at each vertex of the cube. Interpolate these eight
        // noise values using the S-curve value as the interpolant (trilinear interpolation).
        double n0 = GradientNoise3D(x, y, z, x0, y0, z0, seed);
        double n1 = GradientNoise3D(x, y, z, x1, y0, z0, seed);
        const double ix0 = LinearInterp(n0, n1, xs);
        n0 = GradientNoise3D(x, y, z, x0, y1, z0, seed);
        n1 = GradientNoise3D(x, y, z, x1, y1, z0, seed);
        const double ix1 = LinearInterp(n0, n1, xs);
        const double iy0 = LinearInterp(ix0, ix1, ys);
        n0 = GradientNoise3D(x, y, z, x0, y0, z1, seed);
        n1 = GradientNoise3D(x, y, z, x1, y0, z1, seed);
        const double ix2 = LinearInterp(n0, n1, xs);
        n0 = GradientNoise3D(x, y, z, x0, y1, z1, seed);
        n1 = GradientNoise3D(x, y, z, x1, y1, z1, seed);
        const double ix3 = LinearInterp(n0, n1, xs);
        const double iy1 = LinearInterp(ix2, ix3, ys);

        return LinearInterp(iy0, iy1, zs);
    }

    double GradientNoise3D(double fx, double fy, double fz, int32 ix,
        int32 iy, int32 iz, int32 seed) noexcept {
        // Randomly generate a gradient vector given the integer coordinates of the
        // input value. This implementation generates a random number and uses it
        // as an index into a normalized-vector lookup table.
        uint32 vectorIndex = (
            X_NOISE_GEN * static_cast<uint32>(ix) +
            Y_NOISE_GEN * static_cast<uint32>(iy) +
            Z_NOISE_GEN * static_cast<uint32>(iz) +
            SEED_NOISE_GEN * static_cast<uint32>(seed)
        );

        // Optimized index computation: Combine shift and mask in one step
        vectorIndex = (vectorIndex ^ (vectorIndex >> SHIFT_NOISE_GEN)) & 0xff;

        const double xvGradient = g_randomVectors[vectorIndex * 4];
        const double yvGradient = g_randomVectors[vectorIndex * 4 + 1];
        const double zvGradient = g_randomVectors[vectorIndex * 4 + 2];

        // Set up another vector equal to the distance between the two vectors passed to this function.
        const double xvPoint = (fx - static_cast<double>(ix));
        const double yvPoint = (fy - static_cast<double>(iy));
        const double zvPoint = (fz - static_cast<double>(iz));

        // Compute the dot product of the gradient vector with the distance vector.
        // Apply a scaling value so that this noise value ranges from -1.0 to 1.0.
        return (xvGradient * xvPoint + yvGradient * yvPoint + zvGradient * zvPoint) * 2.12;
    }

    int32 IntValueNoise3D(int32 x, int32 y, int32 z, int32 seed) noexcept {
        // All constants are primes and must remain prime for this noise function to work correctly.
        int32 n = (
            X_NOISE_GEN * x +
            Y_NOISE_GEN * y +
            Z_NOISE_GEN * z +
            SEED_NOISE_GEN * seed
        ) & 0x7fffffff;

        n = (n >> 13) ^ n;
        return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    }

    double ValueCoherentNoise3D(double x, double y, double z, int32 seed,
        NoiseQuality noiseQuality) noexcept {
        // Create a unit-length cube aligned along an integer boundary. This cube surrounds the input point.
        const int32 x0 = FastFloor(x);
        const int32 x1 = x0 + 1;
        const int32 y0 = FastFloor(y);
        const int32 y1 = y0 + 1;
        const int32 z0 = FastFloor(z);
        const int32 z1 = z0 + 1;

        // Map the difference between the coordinates of the input value and the
        // coordinates of the cube's outer-lower-left vertex onto an S-curve.
        double xs = 0, ys = 0, zs = 0;
        switch (noiseQuality) {
            case NoiseQuality::QUALITY_FAST:
                xs = (x - static_cast<double>(x0));
                ys = (y - static_cast<double>(y0));
                zs = (z - static_cast<double>(z0));
                break;
            case NoiseQuality::QUALITY_STD:
                xs = SCurve3(x - static_cast<double>(x0));
                ys = SCurve3(y - static_cast<double>(y0));
                zs = SCurve3(z - static_cast<double>(z0));
                break;
            case NoiseQuality::QUALITY_BEST:
                xs = SCurve5(x - static_cast<double>(x0));
                ys = SCurve5(y - static_cast<double>(y0));
                zs = SCurve5(z - static_cast<double>(z0));
                break;
        }

        // Calculate the noise values at each vertex of the cube and interpolate.
        double n0 = ValueNoise3D(x0, y0, z0, seed);
        double n1 = ValueNoise3D(x1, y0, z0, seed);
        const double ix0 = LinearInterp(n0, n1, xs);
        n0 = ValueNoise3D(x0, y1, z0, seed);
        n1 = ValueNoise3D(x1, y1, z0, seed);
        const double ix1 = LinearInterp(n0, n1, xs);
        const double iy0 = LinearInterp(ix0, ix1, ys);
        n0 = ValueNoise3D(x0, y0, z1, seed);
        n1 = ValueNoise3D(x1, y0, z1, seed);
        const double ix2 = LinearInterp(n0, n1, xs);
        n0 = ValueNoise3D(x0, y1, z1, seed);
        n1 = ValueNoise3D(x1, y1, z1, seed);
        const double ix3 = LinearInterp(n0, n1, xs);
        const double iy1 = LinearInterp(ix2, ix3, ys);
        return LinearInterp(iy0, iy1, zs);
    }

} // namespace noise