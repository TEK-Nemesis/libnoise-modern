// line.cpp
//
// Copyright (C) 2004 Keith Davies
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
// The developer's email is not specified.
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 23, 2025:
// - Used noexcept for GetValue since it does not throw exceptions.
// - Used nullptr instead of NULL for modern C++ style.
// - Added debug assertion to enforce p range when attenuation is enabled.
// - Improved documentation with consistent formatting.

#include "noise/model/line.h"

using namespace noise;
using namespace noise::model;

double Line::GetValue(double p) const noexcept {
    assert(m_pModule != nullptr && "Noise module must be set before calling GetValue");
    assert(!m_attenuate || (p >= 0.0 && p <= 1.0) && "Parameter p must be in [0, 1] when attenuation is enabled");

    const double x = (m_x1 - m_x0) * p + m_x0;
    const double y = (m_y1 - m_y0) * p + m_y0;
    const double z = (m_z1 - m_z0) * p + m_z0;
    const double value = m_pModule->GetValue(x, y, z);

    if (m_attenuate) {
        const double attFactor = p * (1.0 - p) * 4.0;
        return value * attFactor;
    }
    return value;
}