// voronoi.h
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
// - Moved GetSourceModuleCount to header as inline to improve efficiency.
// - Used inline constexpr for default constants to ensure proper linkage.
// - Used std::sqrt and std::floor for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Added necessary includes for std::sqrt, std::floor (<cmath>) and ValueNoise3D (../noisegen.h).

#pragma once

#include <cassert>   // For assert
#include <cmath>     // For std::sqrt, std::floor
#include "../noisegen.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default displacement to apply to each cell for the Voronoi noise module.
        inline constexpr double DEFAULT_VORONOI_DISPLACEMENT = 1.0;

        /// Default frequency of the seed points for the Voronoi noise module.
        inline constexpr double DEFAULT_VORONOI_FREQUENCY = 1.0;

        /// Default seed of the noise function for the Voronoi noise module.
        inline constexpr int DEFAULT_VORONOI_SEED = 0;

        /// Noise module that outputs Voronoi cells.
        ///
        /// @image html modulevoronoi.png
        ///
        /// A Voronoi cell is a region containing all points closer to a specific seed
        /// point than to any other seed point, forming polygon-like structures.
        ///
        /// By default, seed points are randomly placed within each unit cube. The
        /// frequency, set via SetFrequency(), adjusts the distance between seed points:
        /// higher frequencies reduce cell size. Each cell is assigned a random constant
        /// value from a coherent-noise function, with the range controlled by the
        /// displacement value (set via SetDisplacement()), which spans +/- the
        /// displacement value.
        ///
        /// The seed value, set via SetSeed(), modifies the random positions of seed
        /// points. Enabling distance application (via EnableDistance()) adds the
        /// distance from the nearest seed point to the output, increasing values
        /// further from the seed.
        ///
        /// This module is useful for generating cracked-mud terrain or crystal-like
        /// textures and requires no source modules.
        class Voronoi : public Module {
        public:
            /// Constructor.
            Voronoi() noexcept
                : Module(GetSourceModuleCount()),
                m_displacement(DEFAULT_VORONOI_DISPLACEMENT),
                m_enableDistance(false),
                m_frequency(DEFAULT_VORONOI_FREQUENCY),
                m_seed(DEFAULT_VORONOI_SEED) {
            }

            /// Enables or disables applying the distance from the nearest seed point
            /// to the output value.
            ///
            /// @param enable Specifies whether to apply the distance (default: true).
            ///
            /// Enabling distance increases output values further from the nearest seed
            /// point. With a near-zero displacement, this can produce cracked-mud formations.
            inline void EnableDistance(bool enable = true) noexcept {
                m_enableDistance = enable;
            }

            /// Returns the displacement value of the Voronoi cells.
            ///
            /// @returns The displacement value.
            ///
            /// The displacement controls the range of random values assigned to each
            /// cell, spanning +/- the displacement value.
            [[nodiscard]] inline double GetDisplacement() const noexcept {
                return m_displacement;
            }

            /// Returns the frequency of the seed points.
            ///
            /// @returns The frequency of the seed points.
            ///
            /// The frequency determines the size of the Voronoi cells and the distance
            /// between them.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Returns the seed value used by the Voronoi cells.
            ///
            /// @returns The seed value.
            ///
            /// The seed value modifies the random positions of seed points via a
            /// coherent-noise function.
            [[nodiscard]] inline int GetSeed() const noexcept {
                return m_seed;
            }

            /// Determines if the distance from the nearest seed point is applied to
            /// the output value.
            ///
            /// @returns True if distance is applied, false otherwise.
            [[nodiscard]] inline bool IsDistanceEnabled() const noexcept {
                return m_enableDistance;
            }

            double GetValue(double x, double y, double z) const noexcept override;

            /// Sets the displacement value of the Voronoi cells.
            ///
            /// @param displacement The displacement value.
            ///
            /// The displacement controls the range of random values assigned to each
            /// cell, spanning +/- the displacement value.
            inline void SetDisplacement(double displacement) noexcept {
                m_displacement = displacement;
            }

            /// Sets the frequency of the seed points.
            ///
            /// @param frequency The frequency of the seed points.
            ///
            /// The frequency determines the size of the Voronoi cells and the distance
            /// between them.
            inline void SetFrequency(double frequency) noexcept {
                m_frequency = frequency;
            }

            /// Sets the seed value used by the Voronoi cells.
            ///
            /// @param seed The seed value.
            ///
            /// The seed value modifies the random positions of seed points via a
            /// coherent-noise function.
            inline void SetSeed(int seed) noexcept {
                m_seed = seed;
            }

        protected:
            /// Scale of the random displacement to apply to each Voronoi cell.
            double m_displacement;

            /// Determines if the distance from the nearest seed point is applied to
            /// the output value.
            bool m_enableDistance;

            /// Frequency of the seed points.
            double m_frequency;

            /// Seed value used by the coherent-noise function to determine the
            /// positions of the seed points.
            int m_seed;
        };

    } // namespace module

} // namespace noise