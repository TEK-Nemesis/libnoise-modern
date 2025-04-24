// sphere.cpp
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
// - Used noexcept for GetValue since it does not throw exceptions.
// - Used nullptr instead of NULL for modern C++ style.
// - Added debug assertions to enforce latitude and longitude ranges.

#include "noise/latlon.h"
#include "noise/model/sphere.h"

using namespace noise;
using namespace noise::model;

double Sphere::GetValue(double lat, double lon) const noexcept {
    assert(m_pModule != nullptr && "Noise module must be set before calling GetValue");
    assert(lat >= -90.0 && lat <= 90.0 && "Latitude must be in the range [-90, +90] degrees");
    assert(lon >= -180.0 && lon <= 180.0 && "Longitude must be in the range [-180, +180] degrees");

    double x, y, z;
    LatLonToXYZ(lat, lon, x, y, z);
    return m_pModule->GetValue(x, y, z);
}