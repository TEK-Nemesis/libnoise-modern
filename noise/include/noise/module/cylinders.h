// cylinders.h
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
// - Added [[nodiscard]] to GetFrequency to encourage proper use of return values.
// - Moved GetFrequency, GetSourceModuleCount, GetValue, and SetFrequency to header as inline to improve efficiency.
// - Used inline constexpr for DEFAULT_CYLINDERS_FREQUENCY to ensure proper linkage.
// - Used std::sqrt and std::floor instead of sqrt and floor for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with mathematical formulas and consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cmath> // For std::sqrt, std::floor
#include "../misc.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default frequency value for the Cylinders noise module.
        inline constexpr double DEFAULT_CYLINDERS_FREQUENCY = 1.0;

        /// A noise module that outputs a pattern of concentric cylinders along the y-axis.
        ///
        /// @image html modulecylinders.png
        ///
        /// This noise module generates concentric cylinders centered on the origin, oriented
        /// along the y-axis (like the rings of a tree). Each cylinder extends infinitely along
        /// the y-axis. The first cylinder has a radius of 1.0, and each subsequent cylinder
        /// has a radius 1.0 unit larger than the previous one.
        ///
        /// The output value is determined by the distance from the input point to the nearest
        /// cylinder surface in the xz-plane:
        /// - Points on a cylinder surface (distance 0) output 1.0.
        /// - Points equidistant from two cylinder surfaces (distance 0.5) output -1.0.
        /// - Values are linearly interpolated between these extremes, ranging from -1.0 to 1.0.
        ///
        /// The frequency of the cylinders can be adjusted using SetFrequency(). Increasing the
        /// frequency reduces the spacing between cylinders.
        ///
        /// When modified with low-frequency, low-power turbulence, this module can generate
        /// wood-like textures.
        ///
        /// This noise module does not require any source modules.
        class Cylinders : public Module {
        public:
            /// Constructor.
            ///
            /// The default frequency is set to DEFAULT_CYLINDERS_FREQUENCY.
            Cylinders() noexcept
                : Module(GetSourceModuleCount()),
                m_frequency(DEFAULT_CYLINDERS_FREQUENCY) {
            }

            /// Returns the frequency of the concentric cylinders.
            ///
            /// @returns The frequency of the concentric cylinders.
            ///
            /// Higher frequency values reduce the spacing between cylinders.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Generates the output value based on the distance to the nearest cylinder surface.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value (ignored).
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns A value in the range [-1.0, 1.0], where:
            /// - 1.0 is output for points on a cylinder surface.
            /// - -1.0 is output for points equidistant from two cylinder surfaces.
            ///
            /// The output is computed as:
            /// - \( \text{distFromCenter} = \sqrt{x^2 + z^2} \cdot \text{frequency} \)
            /// - \( \text{nearestDist} = \min(\text{distFromCenter} - \lfloor \text{distFromCenter} \rfloor, 1.0 - (\text{distFromCenter} - \lfloor \text{distFromCenter} \rfloor)) \)
            /// - Output = \( 1.0 - (\text{nearestDist} \cdot 4.0) \)
            inline double GetValue(double x, double y, double z) const noexcept override {
                x *= m_frequency;
                z *= m_frequency;

                const double distFromCenter = std::sqrt(x * x + z * z);
                const double distFromSmallerSphere = distFromCenter - std::floor(distFromCenter);
                const double distFromLargerSphere = 1.0 - distFromSmallerSphere;
                const double nearestDist = GetMin(distFromSmallerSphere, distFromLargerSphere);
                return 1.0 - (nearestDist * 4.0); // Maps to [-1.0, 1.0] range.
            }

            /// Sets the frequency of the concentric cylinders.
            ///
            /// @param frequency The frequency of the concentric cylinders.
            ///
            /// Higher frequency values reduce the spacing between cylinders.
            inline void SetFrequency(double frequency) noexcept {
                m_frequency = frequency;
            }

        protected:
            /// Frequency of the concentric cylinders.
            double m_frequency;
        };

    } // namespace module

} // namespace noise