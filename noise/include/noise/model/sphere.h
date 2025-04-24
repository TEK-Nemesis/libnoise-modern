// sphere.h
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 30, 2025:
// - Replaced include guard with #pragma once.
// - Added noexcept to member functions that do not throw exceptions.
// - Added [[nodiscard]] to GetModule to encourage proper use of return values.
// - Moved GetModule and SetModule to header as inline functions to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer lists in constructors for clarity and efficiency.
// - Added debug assertions in GetValue to enforce latitude and longitude ranges.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert> // For assert
#include "../module/modulebase.h"

namespace noise {

    namespace model {

        /// Models the surface of a sphere.
        ///
        /// @image html modelsphere.png
        ///
        /// This class returns an output value from a noise module given the coordinates
        /// of an input value located on the surface of a sphere (latitude, longitude).
        ///
        /// To generate an output value, pass the (latitude, longitude) coordinates of
        /// an input value to the GetValue() method.
        ///
        /// This model is useful for creating:
        /// - seamless textures that can be mapped onto a sphere
        /// - terrain height maps for entire planets
        ///
        /// This sphere has a radius of 1.0 unit and its center is located at the origin.
        class Sphere {
        public:
            /// Constructor.
            Sphere() noexcept : m_pModule(nullptr) {}

            /// Constructor.
            ///
            /// @param module The noise module used to generate the output values.
            explicit Sphere(const module::Module& module) noexcept : m_pModule(&module) {}

            /// Returns the noise module used to generate the output values.
            ///
            /// @returns A reference to the noise module.
            /// @pre A noise module was set using the SetModule() method.
            [[nodiscard]] const module::Module& GetModule() const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetModule");
                return *m_pModule;
            }

            /// Returns the output value from the noise module given the (latitude, longitude)
            /// coordinates of the specified input value on the surface of the sphere.
            ///
            /// @param lat The latitude of the input value, in degrees, must be in [-90, +90].
            /// @param lon The longitude of the input value, in degrees, must be in [-180, +180].
            ///
            /// @returns The output value from the noise module.
            /// @pre A noise module was set using the SetModule() method.
            ///
            /// Use a negative latitude for the southern hemisphere.
            /// Use a negative longitude for the western hemisphere.
            double GetValue(double lat, double lon) const noexcept;

            /// Sets the noise module used to generate the output values.
            ///
            /// @param module The noise module to use.
            ///
            /// This noise module must remain valid for the lifetime of this object
            /// until a new noise module is set.
            void SetModule(const module::Module& module) noexcept {
                m_pModule = &module;
            }

        private:
            /// Pointer to the noise module used to generate the output values.
            const module::Module* m_pModule;
        };

    } // namespace model

} // namespace noise