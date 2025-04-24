// multiply.h
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
// - Updated m_sourceModules to m_sourceModules to match the base class.

#pragma once

#include <cassert>  // For assert
#include "modulebase.h"

namespace noise {

    namespace module {

        /// A noise module that outputs the product of the output values from two source modules.
        ///
        /// @image html modulemultiply.png
        ///
        /// This noise module requires two source modules:
        /// - Source module 0 (index 0): The first value.
        /// - Source module 1 (index 1): The second value.
        ///
        /// The output is computed as: source0 * source1.
        class Multiply : public Module {
        public:
            /// Constructor.
            Multiply() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 2, as this module requires two source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 2;
            }

            /// Returns the product of the output values from the two source modules.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The computed product.
            /// @pre Both source modules (indices 0 and 1) have been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module 0 must be set before calling GetValue");
                assert(m_sourceModules[1] != nullptr && "Source module 1 must be set before calling GetValue");

                return m_sourceModules[0]->GetValue(x, y, z) * m_sourceModules[1]->GetValue(x, y, z);
            }
        };

    } // namespace module

} // namespace noise