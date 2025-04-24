// checkerboard.h
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
// - Replaced include guard with #pragma once.
// - Added noexcept to member functions that do not throw exceptions.
// - Added override to virtual functions for clarity.
// - Added constexpr to GetSourceModuleCount since it is a constant expression.
// - Moved GetValue to header as inline to improve efficiency.
// - Used std::floor instead of floor for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cmath> // For std::floor
#include "../noisegen.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// A noise module that outputs a 3D checkerboard pattern.
        ///
        /// @image html modulecheckerboard.png
        ///
        /// This noise module generates unit-sized blocks of alternating values, switching
        /// between -1.0 and 1.0 based on the integer coordinates of the input point.
        /// It is primarily used for debugging purposes.
        ///
        /// This noise module does not require any source modules.
        class Checkerboard : public Module {
        public:
            /// Constructor.
            Checkerboard() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Generates the output value for the given input coordinates.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns -1.0 or 1.0, alternating based on the integer coordinates.
            ///
            /// The output alternates between -1.0 and 1.0 based on the parity of the integer
            /// coordinates after applying MakeInt32Range and flooring. The pattern is determined
            /// by XORing the least significant bits of the integer coordinates:
            /// - If the result is 0, returns 1.0.
            /// - If the result is 1, returns -1.0.
            inline double GetValue(double x, double y, double z) const noexcept override {
                const int ix = static_cast<int>(std::floor(MakeInt32Range(x)));
                const int iy = static_cast<int>(std::floor(MakeInt32Range(y)));
                const int iz = static_cast<int>(std::floor(MakeInt32Range(z)));
                return (ix & 1 ^ iy & 1 ^ iz & 1) ? -1.0 : 1.0;
            }
        };

    } // namespace module

} // namespace noise