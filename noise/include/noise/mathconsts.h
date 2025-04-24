// mathconsts.h
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
// - Added inline to constants to ensure proper linkage in header-only files.
// - Improved documentation with precision notes for clarity.
// - Removed redundant Doxygen group tags for simplicity.

#pragma once

namespace noise {

    /// The mathematical constant pi, approximately 3.1415926535897932385.
    inline constexpr double PI = 3.1415926535897932385;

    /// The square root of 2, approximately 1.4142135623730950488.
    inline constexpr double SQRT_2 = 1.4142135623730950488;

    /// The square root of 3, approximately 1.7320508075688772935.
    inline constexpr double SQRT_3 = 1.7320508075688772935;

    /// Conversion factor from degrees to radians 
    inline constexpr double DEG_TO_RAD = PI / 180.0;

    /// Conversion factor from radians to degrees 
    inline constexpr double RAD_TO_DEG = 180.0 / PI;

} // namespace noise