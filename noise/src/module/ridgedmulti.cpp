// ridgedmulti.cpp
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
// - Added noexcept to GetValue and CalcSpectralWeights since they do not throw exceptions.
// - Used std::pow and std::fabs for modern C++ style.
// - Used using declaration for namespace to modernize syntax.
// - Moved constructor to header as inline.

#include "noise/module/ridgedmulti.h"

using namespace noise::module;

void RidgedMulti::CalcSpectralWeights() noexcept {
    double h = 1.0;
    double frequency = 1.0;
    for (int i = 0; i < RIDGED_MAX_OCTAVE; i++) {
        m_pSpectralWeights[i] = std::pow(frequency, -h);
        frequency *= m_lacunarity;
    }
}

double RidgedMulti::GetValue(double x, double y, double z) const noexcept {
    x *= m_frequency;
    y *= m_frequency;
    z *= m_frequency;

    double signal = 0.0;
    double value = 0.0;
    double weight = 1.0;

    double offset = 1.0;
    double gain = 2.0;

    for (int curOctave = 0; curOctave < m_octaveCount; curOctave++) {
        double nx = MakeInt32Range(x);
        double ny = MakeInt32Range(y);
        double nz = MakeInt32Range(z);

        int seed = (m_seed + curOctave) & 0x7fffffff;
        signal = GradientCoherentNoise3D(nx, ny, nz, seed, m_noiseQuality);
        signal = std::fabs(signal);
        signal = offset - signal;
        signal *= signal;
        signal *= weight;

        weight = signal * gain;
        if (weight > 1.0) {
            weight = 1.0;
        }
        if (weight < 0.0) {
            weight = 0.0;
        }

        value += (signal * m_pSpectralWeights[curOctave]);

        x *= m_lacunarity;
        y *= m_lacunarity;
        z *= m_lacunarity;
    }

    return (value * 1.25) - 1.0;
}