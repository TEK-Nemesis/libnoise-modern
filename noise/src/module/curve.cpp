// curve.cpp
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
// - Used noexcept for GetValue since it does not throw exceptions.
// - Used nullptr instead of NULL for modern C++ style.
// - Replaced raw pointer array with std::vector for control points.
// - Optimized GetValue by using references to control points.
// - Improved documentation with consistent formatting.

#include "noise/module/curve.h"

using namespace noise::module;

void Curve::AddControlPoint(double inputValue, double outputValue) {
    // Find the insertion point and insert the new control point.
    int insertionPos = FindInsertionPos(inputValue);
    InsertAtPos(insertionPos, inputValue, outputValue);
}

int Curve::FindInsertionPos(double inputValue) const {
    for (int i = 0; i < static_cast<int>(m_controlPoints.size()); ++i) {
        if (inputValue < m_controlPoints[i].inputValue) {
            return i;
        }
        if (inputValue == m_controlPoints[i].inputValue) {
            throw noise::ExceptionInvalidParam();
        }
    }
    return static_cast<int>(m_controlPoints.size());
}

double Curve::GetValue(double x, double y, double z) const noexcept {
    assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
    assert(m_controlPoints.size() >= 4 && "At least four control points are required for cubic interpolation");

    double sourceValue = m_sourceModules[0]->GetValue(x, y, z);

    int indexPos;
    for (indexPos = 0; indexPos < static_cast<int>(m_controlPoints.size()); ++indexPos) {
        if (sourceValue < m_controlPoints[indexPos].inputValue) {
            break;
        }
    }

    int index0 = ClampValue(indexPos - 2, 0, static_cast<int>(m_controlPoints.size()) - 1);
    int index1 = ClampValue(indexPos - 1, 0, static_cast<int>(m_controlPoints.size()) - 1);
    int index2 = ClampValue(indexPos, 0, static_cast<int>(m_controlPoints.size()) - 1);
    int index3 = ClampValue(indexPos + 1, 0, static_cast<int>(m_controlPoints.size()) - 1);

    if (index1 == index2) {
        return m_controlPoints[index1].outputValue;
    }

    const double input0 = m_controlPoints[index1].inputValue;
    const double input1 = m_controlPoints[index2].inputValue;
    const double alpha = (sourceValue - input0) / (input1 - input0);

    return CubicInterp(
        m_controlPoints[index0].outputValue,
        m_controlPoints[index1].outputValue,
        m_controlPoints[index2].outputValue,
        m_controlPoints[index3].outputValue,
        alpha);
}

void Curve::InsertAtPos(int insertionPos, double inputValue, double outputValue) {
    ControlPoint newPoint;
    newPoint.inputValue = inputValue;
    newPoint.outputValue = outputValue;
    m_controlPoints.insert(m_controlPoints.begin() + insertionPos, newPoint);
}