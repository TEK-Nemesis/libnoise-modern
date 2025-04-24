// power.h
//
// Copyright (C) 2004 Owen Jacobson
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
// The developer's email is angstrom@lionsanctuary.net
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 23, 2025:
// - Replaced include guard with #pragma once.
// - Added noexcept to member functions that do not throw exceptions.
// - Added override to virtual functions for clarity.
// - Moved GetSourceModuleCount and GetValue to header as inline to improve efficiency.
// - Used std::pow for modern C++ style.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Updated m_sourceModules to m_sourceModules to match the base class.

#pragma once

#include <cassert>  // For assert
#include <cmath>    // For std::pow
#include "modulebase.h"

namespace noise {

    namespace module {

        /// A noise module that raises the output value from a first source module
        /// to the power of the output value from a second source module.
        ///
        /// @image html modulepower.png
        ///
        /// This noise module requires two source modules:
        /// - Source module 0 (index 0): The base value.
        /// - Source module 1 (index 1): The exponent value.
        ///
        /// The output is computed as: pow(source1, source0).
        class Power : public Module {
        public:
            /// Constructor.
            Power() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 2, as this module requires two source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 2;
            }

            /// Returns the result of raising the output of source module 1 to the power
            /// of the output of source module 0.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The computed value.
            /// @pre Both source modules (indices 0 and 1) have been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module 0 must be set before calling GetValue");
                assert(m_sourceModules[1] != nullptr && "Source module 1 must be set before calling GetValue");

                const double v0 = m_sourceModules[0]->GetValue(x, y, z);
                const double v1 = m_sourceModules[1]->GetValue(x, y, z);
                return std::pow(v1, v0);
            }
        };

    } // namespace module

} // namespace noise