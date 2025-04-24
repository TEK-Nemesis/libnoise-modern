// plane.h
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
// The developer's email is ojacobson@lionsanctuary.net
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on May 1, 2025:
// - Replaced include guard with #pragma once.
// - Added noexcept to member functions that do not throw exceptions.
// - Added [[nodiscard]] to GetModule to encourage proper use of return values.
// - Moved all member functions to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer lists in constructors for clarity and efficiency.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert> // For assert
#include "../module/modulebase.h"

namespace noise {

    namespace model {

        /// Models the surface of an infinite plane.
        ///
        /// This class returns an output value from a noise module given the coordinates
        /// of an input value located on the surface of an (x, z) plane (y = 0).
        ///
        /// To generate an output value, pass the (x, z) coordinates of an input value
        /// to the GetValue() method.
        ///
        /// This model is useful for creating:
        /// - two-dimensional textures
        /// - terrain height maps for local areas
        ///
        /// This plane extends infinitely in both directions.
        class Plane {
        public:
            /// Constructor.
            Plane() noexcept : m_pModule(nullptr) {}

            /// Constructor.
            ///
            /// @param module The noise module used to generate the output values.
            explicit Plane(const module::Module& module) noexcept : m_pModule(&module) {}

            /// Returns the noise module used to generate the output values.
            ///
            /// @returns A reference to the noise module.
            /// @pre A noise module was set using the SetModule() method.
            [[nodiscard]] const module::Module& GetModule() const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetModule");
                return *m_pModule;
            }

            /// Returns the output value from the noise module given the (x, z) coordinates
            /// of the specified input value on the surface of the plane (y = 0).
            ///
            /// @param x The x-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The output value from the noise module.
            /// @pre A noise module was set using the SetModule() method.
            inline double GetValue(double x, double z) const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetValue");
                return m_pModule->GetValue(x, 0, z);
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