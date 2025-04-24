// rotatepoint.cpp
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
// - Added noexcept to SetAngles since it does not throw exceptions.
// - Used std::sin and std::cos for modern C++ style.
// - Used using declaration for namespace to modernize syntax.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Moved constructor and GetValue to header as inline.

#include "noise/mathconsts.h"
#include "noise/module/rotatepoint.h"

using namespace noise::module;

void RotatePoint::SetAngles(double xAngle, double yAngle, double zAngle) noexcept {
    double xCos = std::cos(xAngle * DEG_TO_RAD);
    double yCos = std::cos(yAngle * DEG_TO_RAD);
    double zCos = std::cos(zAngle * DEG_TO_RAD);
    double xSin = std::sin(xAngle * DEG_TO_RAD);
    double ySin = std::sin(yAngle * DEG_TO_RAD);
    double zSin = std::sin(zAngle * DEG_TO_RAD);

    m_x1Matrix = ySin * xSin * zSin + yCos * zCos;
    m_y1Matrix = xCos * zSin;
    m_z1Matrix = ySin * zCos - yCos * xSin * zSin;
    m_x2Matrix = ySin * xSin * zCos - yCos * zSin;
    m_y2Matrix = xCos * zCos;
    m_z2Matrix = -yCos * xSin * zCos - ySin * zSin;
    m_x3Matrix = -ySin * xCos;
    m_y3Matrix = xSin;
    m_z3Matrix = yCos * xCos;

    m_xAngle = xAngle;
    m_yAngle = yAngle;
    m_zAngle = zAngle;
}