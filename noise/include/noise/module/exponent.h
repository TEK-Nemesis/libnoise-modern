// exponent.h
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
// - Added [[nodiscard]] to GetExponent to encourage proper use of return values.
// - Moved GetExponent, GetSourceModuleCount, GetValue, and SetExponent to header as inline to improve efficiency.
// - Used inline constexpr for DEFAULT_EXPONENT to ensure proper linkage.
// - Used std::pow and std::fabs instead of pow and fabs for modern C++ style.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with mathematical formulas and consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert> // For assert
#include <cmath>   // For std::pow, std::fabs
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default exponent for the Exponent noise module.
        inline constexpr double DEFAULT_EXPONENT = 1.0;

        /// A noise module that maps the output value from a source module onto an exponential curve.
        ///
        /// @image html moduleexponent.png
        ///
        /// This noise module normalizes the source module's output from the range [-1.0, 1.0] to
        /// [0.0, 1.0], applies an exponential curve using the specified exponent, and rescales the
        /// result back to [-1.0, 1.0].
        ///
        /// The mapping process is as follows:
        /// - Normalize: \( \text{normalized} = (\text{value} + 1) / 2 \)
        /// - Apply exponent: \( \text{exponentiated} = \text{normalized}^{\text{exponent}} \)
        /// - Rescale: \( \text{output} = \text{exponentiated} \cdot 2 - 1 \)
        ///
        /// This noise module requires one source module.
        class Exponent : public Module {
        public:
            /// Constructor.
            ///
            /// The default exponent is set to DEFAULT_EXPONENT.
            Exponent() noexcept : Module(GetSourceModuleCount()), m_exponent(DEFAULT_EXPONENT) {}

            /// Returns the exponent value used for the exponential curve.
            ///
            /// @returns The exponent value.
            [[nodiscard]] inline double GetExponent() const noexcept {
                return m_exponent;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires exactly one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Maps the source module's output value onto an exponential curve.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The mapped output value in the range [-1.0, 1.0].
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");

                const double value = m_sourceModules[0]->GetValue(x, y, z);
                const double normalized = (value + 1.0) / 2.0;
                const double exponentiated = std::pow(std::fabs(normalized), m_exponent);
                return exponentiated * 2.0 - 1.0;
            }

            /// Sets the exponent value for the exponential curve.
            ///
            /// @param exponent The exponent value to set.
            inline void SetExponent(double exponent) noexcept {
                m_exponent = exponent;
            }

        protected:
            /// Exponent to apply to the normalized output value from the source module.
            double m_exponent;
        };

    } // namespace module

} // namespace noise