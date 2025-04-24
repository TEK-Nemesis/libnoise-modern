// select.cpp
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
// - Added noexcept to GetValue and SetEdgeFalloff since they do not throw exceptions.
// - Used using declaration for namespace to modernize syntax.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Moved constructor to header as inline.
// - Moved GetValue inline implementation to header.

#include "noise/interp.h"
#include "noise/module/select.h"

using namespace noise::module;

const Module& Select::GetControlModule() const {
    if (m_sourceModules[2] == nullptr) {
        throw noise::ExceptionNoModule();
    }
    return *(m_sourceModules[2]);
}

double Select::GetValue(double x, double y, double z) const noexcept {
    assert(m_sourceModules[0] != nullptr && "Source module 0 must be set before calling GetValue");
    assert(m_sourceModules[1] != nullptr && "Source module 1 must be set before calling GetValue");
    assert(m_sourceModules[2] != nullptr && "Control module (source module 2) must be set before calling GetValue");

    double controlValue = m_sourceModules[2]->GetValue(x, y, z);
    if (m_edgeFalloff > 0.0) {
        if (controlValue < (m_lowerBound - m_edgeFalloff)) {
            return m_sourceModules[0]->GetValue(x, y, z);
        } else if (controlValue < (m_lowerBound + m_edgeFalloff)) {
            double lowerCurve = (m_lowerBound - m_edgeFalloff);
            double upperCurve = (m_lowerBound + m_edgeFalloff);
            double alpha = SCurve3((controlValue - lowerCurve) / (upperCurve - lowerCurve));
            return LinearInterp(m_sourceModules[0]->GetValue(x, y, z),
                m_sourceModules[1]->GetValue(x, y, z),
                alpha);
        } else if (controlValue < (m_upperBound - m_edgeFalloff)) {
            return m_sourceModules[1]->GetValue(x, y, z);
        } else if (controlValue < (m_upperBound + m_edgeFalloff)) {
            double lowerCurve = (m_upperBound - m_edgeFalloff);
            double upperCurve = (m_upperBound + m_edgeFalloff);
            double alpha = SCurve3((controlValue - lowerCurve) / (upperCurve - lowerCurve));
            return LinearInterp(m_sourceModules[1]->GetValue(x, y, z),
                m_sourceModules[0]->GetValue(x, y, z),
                alpha);
        } else {
            return m_sourceModules[0]->GetValue(x, y, z);
        }
    } else {
        if (controlValue < m_lowerBound || controlValue > m_upperBound) {
            return m_sourceModules[0]->GetValue(x, y, z);
        } else {
            return m_sourceModules[1]->GetValue(x, y, z);
        }
    }
}

void Select::SetBounds(double lowerBound, double upperBound) {
    assert(lowerBound < upperBound);

    m_lowerBound = lowerBound;
    m_upperBound = upperBound;

    // Make sure that the edge falloff curves do not overlap.
    SetEdgeFalloff(m_edgeFalloff);
}

void Select::SetEdgeFalloff(double edgeFalloff) noexcept {
    // Make sure that the edge falloff curves do not overlap.
    double boundSize = m_upperBound - m_lowerBound;
    m_edgeFalloff = (edgeFalloff > boundSize / 2) ? boundSize / 2 : edgeFalloff;
}