// voronoi.cpp
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
// - Added noexcept to GetValue since it does not throw exceptions.
// - Used std::sqrt and std::floor for modern C++ style.
// - Used using declaration for namespace to modernize syntax.
// - Moved constructor to header as inline.
// - Added include for mathconsts.h (../mathconsts.h) for SQRT_3.

#include <cmath>
#include "noise/mathconsts.h"
#include "noise/module/voronoi.h"

using namespace noise::module;

double Voronoi::GetValue(double x, double y, double z) const noexcept {
    x *= m_frequency;
    y *= m_frequency;
    z *= m_frequency;

    int xInt = (x > 0.0 ? static_cast<int>(x) : static_cast<int>(x) - 1);
    int yInt = (y > 0.0 ? static_cast<int>(y) : static_cast<int>(y) - 1);
    int zInt = (z > 0.0 ? static_cast<int>(z) : static_cast<int>(z) - 1);

    double minDist = std::numeric_limits<double>::max();
    double xCandidate = 0.0;
    double yCandidate = 0.0;
    double zCandidate = 0.0;

    // Search nearby cubes for the closest seed point.
    for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
        for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
            for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {
                double xPos = xCur + ValueNoise3D(xCur, yCur, zCur, m_seed);
                double yPos = yCur + ValueNoise3D(xCur, yCur, zCur, m_seed + 1);
                double zPos = zCur + ValueNoise3D(xCur, yCur, zCur, m_seed + 2);
                double xDist = xPos - x;
                double yDist = yPos - y;
                double zDist = zPos - z;
                double dist = xDist * xDist + yDist * yDist + zDist * zDist;

                if (dist < minDist) {
                    minDist = dist;
                    xCandidate = xPos;
                    yCandidate = yPos;
                    zCandidate = zPos;
                }
            }
        }
    }

    double value;
    if (m_enableDistance) {
        double xDist = xCandidate - x;
        double yDist = yCandidate - y;
        double zDist = zCandidate - z;
        value = std::sqrt(xDist * xDist + yDist * yDist + zDist * zDist) * SQRT_3 - 1.0;
    } else {
        value = 0.0;
    }

    // Apply the displacement value.
    return value + (m_displacement * ValueNoise3D(
        static_cast<int>(std::floor(xCandidate)),
        static_cast<int>(std::floor(yCandidate)),
        static_cast<int>(std::floor(zCandidate))));
}