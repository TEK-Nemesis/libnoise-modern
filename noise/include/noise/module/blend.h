// blend.h
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
// - Updated m_pSourceModule to m_sourceModules to match the base class.
// - Fixed null checks to use m_sourceModules[index] instead of comparing vector with nullptr.

#pragma once

#include <cassert>  // For assert
#include "../interp.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Noise module that outputs a weighted blend of the output values from two
        /// source modules, controlled by a third source module.
        ///
        /// @image html moduleblend.png
        ///
        /// This noise module requires three source modules:
        /// - Source module 0 (index 0): First output value.
        /// - Source module 1 (index 1): Second output value.
        /// - Source module 2 (index 2): Control module, which determines the blend weight.
        ///
        /// The control module's output, ranging from -1.0 to +1.0, is scaled to 0.0 to
        /// 1.0 and used as the interpolation factor between the two source modules.
        class Blend : public Module {
        public:
            /// Constructor.
            Blend() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 3, as this module requires three source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 3;
            }

            /// Returns the blended output value from the two source modules based on
            /// the control module's output.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The blended output value.
            /// @pre All source modules (indices 0, 1, and 2) have been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module 0 must be set before calling GetValue");
                assert(m_sourceModules[1] != nullptr && "Source module 1 must be set before calling GetValue");
                assert(m_sourceModules[2] != nullptr && "Control module (source module 2) must be set before calling GetValue");

                const double v0 = m_sourceModules[0]->GetValue(x, y, z);
                const double v1 = m_sourceModules[1]->GetValue(x, y, z);
                const double alpha = (m_sourceModules[2]->GetValue(x, y, z) + 1.0) / 2.0;
                return LinearInterp(v0, v1, alpha);
            }

            /// Sets the control module.
            ///
            /// @param controlModule The control module.
            ///
            /// This method assigns the control module to index 2, equivalent to calling
            /// SetSourceModule(2, controlModule).
            ///
            /// The control module must remain valid for the lifetime of this object
            /// until replaced.
            inline void SetControlModule(const Module& controlModule) noexcept {
                assert(m_sourceModules.size() >= 3);
                m_sourceModules[2] = &controlModule;
            }
        };

    } // namespace module

} // namespace noise