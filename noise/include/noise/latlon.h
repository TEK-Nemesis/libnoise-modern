// latlon.h
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
// - Replaced include guard with #pragma once.
// - Added noexcept to LatLonToXYZ since it does not throw exceptions.
// - Moved LatLonToXYZ implementation to header as inline function to improve efficiency.
// - Added debug assertions to enforce latitude and longitude ranges.
// - Improved documentation for clarity and consistency.
// - Removed redundant Doxygen group tags.

#pragma once

#include "mathconsts.h"
#include <cassert> // For assert
#include <cmath>   // For cos, sin

namespace noise {

    /// Converts latitude/longitude coordinates on a unit sphere into 3D Cartesian coordinates.
    ///
    /// @param lat The latitude, in degrees, must be in the range [-90, +90].
    /// @param lon The longitude, in degrees, must be in the range [-180, +180].
    /// @param[out] x The computed x-coordinate.
    /// @param[out] y The computed y-coordinate.
    /// @param[out] z The computed z-coordinate.
    ///
    /// This function converts spherical coordinates (latitude and longitude) on a unit sphere
    /// (radius = 1) to Cartesian coordinates using the following formulas:
    /// - r = cos(lat)
    /// - x = r * cos(lon)
    /// - y = sin(lat)
    /// - z = r * sin(lon)
    /// where lat and lon are in radians.
    inline void LatLonToXYZ(double lat, double lon, double& x, double& y, double& z) noexcept {
        assert(lat >= -90.0 && lat <= 90.0 && "Latitude must be in the range [-90, +90] degrees");
        assert(lon >= -180.0 && lon <= 180.0 && "Longitude must be in the range [-180, +180] degrees");

        const double latRad = lat * DEG_TO_RAD;
        const double lonRad = lon * DEG_TO_RAD;
        const double r = std::cos(latRad);
        x = r * std::cos(lonRad);
        y = std::sin(latRad);
        z = r * std::sin(lonRad);
    }

} // namespace noise