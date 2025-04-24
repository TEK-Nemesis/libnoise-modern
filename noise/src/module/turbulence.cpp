// turbulence.cpp
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
// - Added noexcept to SetSeed since it does not throw exceptions.
// - Used using declaration for namespace to modernize syntax.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Moved constructor and GetValue to header as inline.

#include "noise/module/turbulence.h"

using namespace noise::module;

void Turbulence::SetSeed(int seed) noexcept {
    m_xDistortModule.SetSeed(seed);
    m_yDistortModule.SetSeed(seed + 1);
    m_zDistortModule.SetSeed(seed + 2);
}