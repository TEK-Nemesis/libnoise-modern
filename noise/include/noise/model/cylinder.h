// cylinder.h
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
// - Added [[nodiscard]] to GetModule to encourage proper use of return values.
// - Moved all member functions to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer lists in constructors for clarity and efficiency.
// - Improved documentation with mathematical formulas and consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert> // For assert
#include <cmath>   // For cos, sin
#include "../module/modulebase.h"
#include "../mathconsts.h"

namespace noise {

    namespace model {

        /// Models the surface of an infinite cylinder.
        ///
        /// @image html modelcylinder.png
        ///
        /// This class returns an output value from a noise module given the (angle, height)
        /// coordinates of an input value located on the surface of a cylinder.
        ///
        /// To generate an output value, pass the (angle, height) coordinates of an input value
        /// to the GetValue() method.
        ///
        /// This model is useful for creating seamless textures that can be mapped onto a cylinder.
        ///
        /// This cylinder has a radius of 1.0 unit and infinite height. It is oriented along the
        /// y-axis, with its center at the origin.
        class Cylinder {
        public:
            /// Constructor.
            Cylinder() noexcept : m_pModule(nullptr) {}

            /// Constructor.
            ///
            /// @param module The noise module used to generate the output values.
            explicit Cylinder(const module::Module& module) noexcept : m_pModule(&module) {}

            /// Returns the noise module used to generate the output values.
            ///
            /// @returns A reference to the noise module.
            /// @pre A noise module was set using the SetModule() method.
            [[nodiscard]] inline const module::Module& GetModule() const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetModule");
                return *m_pModule;
            }

            /// Returns the output value from the noise module given the (angle, height) coordinates
            /// of the specified input value on the surface of the cylinder.
            ///
            /// @param angle The angle around the cylinder's center, in degrees.
            /// @param height The height along the y-axis.
            ///
            /// @returns The output value from the noise module.
            /// @pre A noise module was set using the SetModule() method.
            ///
            /// The Cartesian coordinates (x, y, z) on the cylinder are computed as:
            /// - \( x = \cos(\text{angle}) \)
            /// - \( y = \text{height} \)
            /// - \( z = \sin(\text{angle}) \)
            /// where angle is converted to radians.
            ///
            /// This cylinder has a radius of 1.0 unit and infinite height. It is oriented along
            /// the y-axis, with its center at the origin.
            inline double GetValue(double angle, double height) const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetValue");

                const double angleRad = angle * DEG_TO_RAD;
                const double x = std::cos(angleRad);
                const double y = height;
                const double z = std::sin(angleRad);
                return m_pModule->GetValue(x, y, z);
            }

            /// Sets the noise module used to generate the output values.
            ///
            /// @param module The noise module to use.
            ///
            /// This noise module must remain valid for the lifetime of this object
            /// until a new noise module is set.
            inline void SetModule(const module::Module& module) noexcept {
                m_pModule = &module;
            }

        private:
            /// Pointer to the noise module used to generate the output values.
            const module::Module* m_pModule;
        };

    } // namespace model

} // namespace noise