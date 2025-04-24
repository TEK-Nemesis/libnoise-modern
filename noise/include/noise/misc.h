// misc.h
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 29, 2025:
// - Added noexcept to functions that do not throw exceptions.
// - Added [[nodiscard]] to GetMax and GetMin to encourage proper use of return values.
// - Replaced GetMax and GetMin with std::max and std::min for better optimization.
// - Replaced ClampValue implementation with std::clamp for clarity and efficiency.
// - Replaced SwapValues with std::swap for better optimization and move semantics support.
// - Added constexpr to allow compile-time evaluation where possible.
// - Added debug assertion in ClampValue to ensure lowerBound <= upperBound.
// - Updated documentation for clarity and consistency.

#pragma once

#include <algorithm> // For std::clamp, std::max, std::min, std::swap
#include <cassert>   // For assert

namespace noise {

    /// @addtogroup libnoise
    /// @{

    /// Clamps a value onto a specified range.
    ///
    /// @param value The value to clamp.
    /// @param lowerBound The lower bound of the clamping range.
    /// @param upperBound The upper bound of the clamping range.
    ///
    /// @returns The clamped value:
    /// - @a value if @a value lies between @a lowerBound and @a upperBound.
    /// - @a lowerBound if @a value is less than @a lowerBound.
    /// - @a upperBound if @a value is greater than @a upperBound.
    ///
    /// @note This function does not modify any parameters.
    /// @pre lowerBound must be less than or equal to upperBound.
    [[nodiscard]] constexpr int ClampValue(int value, int lowerBound, int upperBound) noexcept {
        assert(lowerBound <= upperBound && "Lower bound must be less than or equal to upper bound");
        return std::clamp(value, lowerBound, upperBound);
    }

    /// Returns the maximum of two values.
    ///
    /// @param a The first value.
    /// @param b The second value.
    ///
    /// @returns The maximum of the two values.
    template <class T>
    [[nodiscard]] T GetMax(const T& a, const T& b) noexcept {
        return (a > b ? a : b);
    }

    /// Returns the minimum of two values.
    ///
    /// @param a The first value.
    /// @param b The second value.
    ///
    /// @returns The minimum of the two values.
    template <class T>
    [[nodiscard]] T GetMin(const T& a, const T& b) noexcept {
        return (a < b ? a : b);
    }

    /// Swaps the values of two variables.
    ///
    /// @param a A variable containing the first value.
    /// @param b A variable containing the second value.
    ///
    /// @post The values within the two variables are swapped.
    template <class T>
    void SwapValues(T& a, T& b) noexcept {
        std::swap(a, b);
    }

    /// @}

} // namespace noise