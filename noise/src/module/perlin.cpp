// perlin.cpp
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
// - Removed redundant constructor definition since it is already inline in the header.

#include "noise/module/perlin.h"

using namespace noise::module;

double Perlin::GetValue(double x, double y, double z) const noexcept {
    double value = 0.0;
    double signal = 0.0;
    double curPersistence = 1.0;

    x *= m_frequency;
    y *= m_frequency;
    z *= m_frequency;

    for (int curOctave = 0; curOctave < m_octaveCount; ++curOctave) {
        double nx = MakeInt32Range(x);
        double ny = MakeInt32Range(y);
        double nz = MakeInt32Range(z);

        int32 seed = (m_seed + curOctave) & 0xffffffff;
        signal = GradientCoherentNoise3D(nx, ny, nz, seed, m_noiseQuality);
        value += signal * curPersistence;

        x *= m_lacunarity;
        y *= m_lacunarity;
        z *= m_lacunarity;
        curPersistence *= m_persistence;
    }

    return value;
}