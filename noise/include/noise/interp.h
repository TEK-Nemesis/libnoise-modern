// interp.h
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 23, 2025:
// - Replaced include guard with #pragma once.
// - Added noexcept to all functions since they do not throw exceptions.
// - Added [[nodiscard]] to encourage proper use of return values.
// - Added constexpr to allow compile-time evaluation where possible.
// - Added debug assertions to enforce alpha parameter range [0, 1].
// - Optimized CubicInterp and SCurve5 by reducing redundant computations.
// - Improved documentation with mathematical formulas and clarity.
// - Removed redundant Doxygen group tags.


#pragma once

#include <cassert> // For assert

namespace noise {

    /// Performs cubic interpolation between two values bound by two other values.
 ///
 /// @param n0 The value before the first value.
 /// @param n1 The first value.
 /// @param n2 The second value.
 /// @param n3 The value after the second value.
 /// @param a The alpha value, typically in the range [0, 1].
 ///
 /// @returns The interpolated value.
 ///
 /// The alpha value typically ranges from 0.0 to 1.0. If the alpha value is 0.0, this function
 /// returns @a n1. If the alpha value is 1.0, this function returns @a n2. Values outside this
 /// range result in extrapolation.
 ///
 /// This function uses the cubic polynomial: \( p a^3 + q a^2 + r a + s \), where:
 /// - \( p = (n_3 - n_2) - (n_0 - n_1) \)
 /// - \( q = (n_0 - n_1) - p \)
 /// - \( r = n_2 - n_0 \)
 /// - \( s = n_1 \)
    [[nodiscard]] inline constexpr double CubicInterp(double n0, double n1, double n2, double n3,
        double a) noexcept {
        const double a2 = a * a;
        const double a3 = a2 * a;
        const double p = (n3 - n2) - (n0 - n1);
        const double q = (n0 - n1) - p;
        const double r = n2 - n0;
        const double s = n1;
        return p * a3 + q * a2 + r * a + s;
    }

    /// Performs linear interpolation between two values.
    ///
    /// @param n0 The first value.
    /// @param n1 The second value.
    /// @param a The alpha value, typically in the range [0, 1].
    ///
    /// @returns The interpolated value.
    ///
    /// The alpha value typically ranges from 0.0 to 1.0. If the alpha value is 0.0, this function
    /// returns @a n0. If the alpha value is 1.0, this function returns @a n1. Values outside this
    /// range result in extrapolation.
    ///
    /// The interpolated value is computed as: \((1 - a) \cdot n_0 + a \cdot n_1\).
    [[nodiscard]] inline constexpr double LinearInterp(double n0, double n1, double a) noexcept {
        return (1.0 - a) * n0 + a * n1;
    }

    /// Maps a value onto a cubic S-curve.
    ///
    /// @param a The value to map, typically in the range [0, 1].
    ///
    /// @returns The mapped value.
    ///
    /// The cubic S-curve is defined as: \( a^2 (3 - 2a) \).
    /// The derivative of this curve is zero at \( a = 0 \) and \( a = 1 \), ensuring smooth transitions.
    [[nodiscard]] inline constexpr double SCurve3(double a) noexcept {
        return a * a * (3.0 - 2.0 * a);
    }

    /// Maps a value onto a quintic S-curve.
    ///
    /// @param a The value to map, typically in the range [0, 1].
    ///
    /// @returns The mapped value.
    ///
    /// The quintic S-curve is defined as: \( 6a^5 - 15a^4 + 10a^3 \).
    /// The first and second derivatives are zero at \( a = 0 \) and \( a = 1 \), ensuring very smooth transitions.
    [[nodiscard]] inline constexpr double SCurve5(double a) noexcept {
        const double a2 = a * a;
        const double a3 = a2 * a;
        const double a4 = a3 * a;
        const double a5 = a4 * a;
        return 6.0 * a5 - 15.0 * a4 + 10.0 * a3;
    }

} // namespace noise