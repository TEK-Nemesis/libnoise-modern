// basictypes.h
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 30, 2025:
// - Replaced uint alias with std::uint32_t for consistency and portability.
// - Specified underlying type for NoiseQuality enum as int.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cstdint>

namespace noise {

    /// Unsigned 32-bit integer type.
    using uint = std::uint32_t;

    /// 32-bit unsigned integer type.
    using uint32 = std::uint32_t;

    /// 16-bit unsigned integer type.
    using uint16 = std::uint16_t;

    /// 8-bit unsigned integer type.
    using uint8 = std::uint8_t;

    /// 32-bit signed integer type.
    using int32 = std::int32_t;

    /// 16-bit signed integer type.
    using int16 = std::int16_t;

    /// 8-bit signed integer type.
    using int8 = std::int8_t;

    /// Enumerates the quality levels for coherent noise generation.
    enum class NoiseQuality : int {
        /// Generates coherent noise quickly with noticeable "creasing" artifacts in bump-map images.
        /// The derivative is discontinuous at integer boundaries.
        QUALITY_FAST = 0,

        /// Generates standard-quality coherent noise with minor "creasing" artifacts in bump-map images.
        /// The second derivative is discontinuous at integer boundaries.
        QUALITY_STD = 1,

        /// Generates the best-quality coherent noise with no "creasing" artifacts in bump-map images.
        /// The first and second derivatives are continuous at integer boundaries.
        QUALITY_BEST = 2
    };

} // namespace noise