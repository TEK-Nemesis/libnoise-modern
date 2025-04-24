// scalebias.h
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
// - Added [[nodiscard]] to getter methods to encourage proper use of return values.
// - Moved GetSourceModuleCount and GetValue to header as inline to improve efficiency.
// - Used inline constexpr for default constants to ensure proper linkage.
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

        /// Default bias for the ScaleBias noise module.
        inline constexpr double DEFAULT_BIAS = 0.0;

        /// Default scale for the ScaleBias noise module.
        inline constexpr double DEFAULT_SCALE = 1.0;

        /// Noise module that applies a scaling factor and a bias to the output value
        /// from a source module.
        ///
        /// @image html modulescalebias.png
        ///
        /// The GetValue() method retrieves the output value from the source module,
        /// multiplies it by a scaling factor, adds a bias, and outputs the result.
        ///
        /// The output is computed as: source * scale + bias.
        ///
        /// This noise module requires one source module.
        class ScaleBias : public Module {
        public:
            /// Constructor.
            ScaleBias() noexcept
                : Module(GetSourceModuleCount()),
                m_bias(DEFAULT_BIAS),
                m_scale(DEFAULT_SCALE) {
            }

            /// Returns the bias to apply to the scaled output value.
            ///
            /// @returns The bias value.
            [[nodiscard]] inline double GetBias() const noexcept {
                return m_bias;
            }

            /// Returns the scaling factor to apply to the output value.
            ///
            /// @returns The scaling factor.
            [[nodiscard]] inline double GetScale() const noexcept {
                return m_scale;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Returns the scaled and biased output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The scaled and biased output value.
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
                return m_sourceModules[0]->GetValue(x, y, z) * m_scale + m_bias;
            }

            /// Sets the bias to apply to the scaled output value.
            ///
            /// @param bias The bias to apply.
            inline void SetBias(double bias) noexcept {
                m_bias = bias;
            }

            /// Sets the scaling factor to apply to the output value.
            ///
            /// @param scale The scaling factor to apply.
            inline void SetScale(double scale) noexcept {
                m_scale = scale;
            }

        protected:
            /// Bias to apply to the scaled output value from the source module.
            double m_bias;

            /// Scaling factor to apply to the output value from the source module.
            double m_scale;
        };

    } // namespace module

} // namespace noise