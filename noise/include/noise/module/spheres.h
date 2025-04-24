// spheres.h
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
// - Used std::sqrt, std::floor, and std::min for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Added necessary include for std::sqrt, std::floor, and std::min (<cmath>).

#pragma once

#include <algorithm>  // For std::min
#include <cmath>      // For std::sqrt, std::floor
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default frequency value for the Spheres noise module.
        inline constexpr double DEFAULT_SPHERES_FREQUENCY = 1.0;

        /// Noise module that outputs concentric spheres.
        ///
        /// @image html modulespheres.png
        ///
        /// This noise module outputs concentric spheres centered on the origin, like
        /// the layers of an onion. The first sphere has a radius of 1.0, with each
        /// subsequent sphere having a radius 1.0 unit larger than the previous one.
        ///
        /// The output value is determined by the distance between the input value and
        /// the nearest spherical surface: points on a spherical surface yield 1.0,
        /// while points equidistant from two surfaces yield -1.0.
        ///
        /// The frequency of the spheres can be adjusted with SetFrequency(), which
        /// affects the density of the spheres. This module, when combined with
        /// low-frequency, low-power turbulence, can generate agate-like textures.
        ///
        /// This noise module does not require any source modules.
        class Spheres : public Module {
        public:
            /// Constructor.
            Spheres() noexcept
                : Module(GetSourceModuleCount()),
                m_frequency(DEFAULT_SPHERES_FREQUENCY) {
            }

            /// Returns the frequency of the concentric spheres.
            ///
            /// @returns The frequency of the concentric spheres.
            ///
            /// Increasing the frequency reduces the distances between spheres.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Returns the output value based on the distance to the nearest spherical surface.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The output value, ranging from -1.0 to 1.0.
            inline double GetValue(double x, double y, double z) const noexcept override {
                x *= m_frequency;
                y *= m_frequency;
                z *= m_frequency;

                double distFromCenter = std::sqrt(x * x + y * y + z * z);
                double distFromSmallerSphere = distFromCenter - std::floor(distFromCenter);
                double distFromLargerSphere = 1.0 - distFromSmallerSphere;

#ifdef min
#undef min
#endif
                double nearestDist = std::min(distFromSmallerSphere, distFromLargerSphere);
                return 1.0 - (nearestDist * 4.0);
            }

            /// Sets the frequency of the concentric spheres.
            ///
            /// @param frequency The frequency of the concentric spheres.
            ///
            /// Increasing the frequency reduces the distances between spheres.
            inline void SetFrequency(double frequency) noexcept {
                m_frequency = frequency;
            }

        protected:
            /// Frequency of the concentric spheres.
            double m_frequency;
        };

    } // namespace module

} // namespace noise