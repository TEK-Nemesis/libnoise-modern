// terrace.cpp
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
// - Used using declaration for namespace to modernize syntax.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Moved constructor to header as inline.
// - Used std::vector for control points, simplifying memory management.
// - Updated methods to work with std::vector instead of raw pointer array.
// - Removed destructor since std::vector handles cleanup.
// - Removed dependency on misc.h since ClampValue is no longer used.

#include "noise/interp.h"
#include "noise/module/terrace.h"

using namespace noise::module;

void Terrace::AddControlPoint(double value) {
    int insertionPos = FindInsertionPos(value);
    InsertAtPos(insertionPos, value);
}

double Terrace::GetValue(double x, double y, double z) const noexcept {
    assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
    assert(m_controlPoints.size() >= 2 && "At least two control points are required");

    // Get the output value from the source module.
    double sourceModuleValue = m_sourceModules[0]->GetValue(x, y, z);

    // Find the first element in the control point array that has a value
    // larger than the output value from the source module.
    int indexPos;
    for (indexPos = 0; indexPos < static_cast<int>(m_controlPoints.size()); indexPos++) {
        if (sourceModuleValue < m_controlPoints[indexPos]) {
            break;
        }
    }

    // Find the two nearest control points so that we can map their values
    // onto a quadratic curve.
    int index0 = indexPos - 1;
    int index1 = indexPos;
    if (index0 < 0) {
        index0 = 0;
    }
    if (index1 >= static_cast<int>(m_controlPoints.size())) {
        index1 = static_cast<int>(m_controlPoints.size()) - 1;
    }

    // If some control points are missing (which occurs if the output value from
    // the source module is greater than the largest value or less than the
    // smallest value of the control point array), get the value of the nearest
    // control point and exit now.
    if (index0 == index1) {
        return m_controlPoints[index1];
    }

    // Compute the alpha value used for linear interpolation.
    double value0 = m_controlPoints[index0];
    double value1 = m_controlPoints[index1];
    double alpha = (sourceModuleValue - value0) / (value1 - value0);
    if (m_invertTerraces) {
        alpha = 1.0 - alpha;
        std::swap(value0, value1);
    }

    // Squaring the alpha produces the terrace effect.
    alpha *= alpha;

    // Now perform the linear interpolation given the alpha value.
    return LinearInterp(value0, value1, alpha);
}

int Terrace::FindInsertionPos(double value) const {
    int insertionPos;
    for (insertionPos = 0; insertionPos < static_cast<int>(m_controlPoints.size()); insertionPos++) {
        if (value < m_controlPoints[insertionPos]) {
            break;
        } else if (value == m_controlPoints[insertionPos]) {
            throw noise::ExceptionInvalidParam();
        }
    }
    return insertionPos;
}

void Terrace::InsertAtPos(int insertionPos, double value) {
    m_controlPoints.insert(m_controlPoints.begin() + insertionPos, value);
}

void Terrace::MakeControlPoints(int controlPointCount) {
    if (controlPointCount < 2) {
        throw noise::ExceptionInvalidParam();
    }

    ClearAllControlPoints();

    double terraceStep = 2.0 / (static_cast<double>(controlPointCount) - 1.0);
    double curValue = -1.0;
    for (int i = 0; i < controlPointCount; i++) {
        AddControlPoint(curValue);
        curValue += terraceStep;
    }
}