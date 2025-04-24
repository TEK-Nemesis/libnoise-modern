// invert.h
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
// - Moved GetSourceModuleCount and GetValue to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert> // For assert
#include "modulebase.h"

namespace noise {

    namespace module {

        /// A noise module that inverts the output value from a source module.
        ///
        /// @image html moduleinvert.png
        ///
        /// This noise module negates the output value of its source module, effectively
        /// flipping the sign of the output. For example, if the source module outputs
        /// 0.5, this module outputs -0.5.
        ///
        /// This noise module requires one source module.
        class Invert : public Module {
        public:
            /// Constructor.
            Invert() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires exactly one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Returns the negated output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The negated output value.
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
                return -(m_sourceModules[0]->GetValue(x, y, z));
            }
        };

    } // namespace module

} // namespace noise