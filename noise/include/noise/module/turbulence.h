// turbulence.h
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
#include "perlin.h"

namespace noise {

    namespace module {

        /// Default frequency for the Turbulence noise module.
        inline constexpr double DEFAULT_TURBULENCE_FREQUENCY = DEFAULT_PERLIN_FREQUENCY;

        /// Default power for the Turbulence noise module.
        inline constexpr double DEFAULT_TURBULENCE_POWER = 1.0;

        /// Default roughness for the Turbulence noise module.
        inline constexpr int DEFAULT_TURBULENCE_ROUGHNESS = 3;

        /// Default noise seed for the Turbulence noise module.
        inline constexpr int DEFAULT_TURBULENCE_SEED = DEFAULT_PERLIN_SEED;

        /// Noise module that randomly displaces the input value before returning
        /// the output value from a source module.
        ///
        /// @image html moduleturbulence.png
        ///
        /// The GetValue() method randomly displaces the (x, y, z) coordinates of the
        /// input value using three internal Perlin noise modules (one for each
        /// coordinate) before returning the output value from the source module.
        ///
        /// The frequency, power, and roughness of the turbulence can be adjusted:
        /// - Frequency (SetFrequency()) determines how rapidly displacement changes.
        /// - Power (SetPower()) scales the displacement amount.
        /// - Roughness (SetRoughness()) sets the number of octaves in the Perlin modules.
        ///
        /// Suggested initial settings include matching the frequency to the source
        /// module and setting the power to the reciprocal of the frequency. Adjusting
        /// these values can produce effects ranging from subtle distortions to noisy
        /// terrain or textures like lava rock or marble.
        ///
        /// This noise module requires one source module.
        class Turbulence : public Module {
        public:
            /// Constructor.
            Turbulence() noexcept
                : Module(GetSourceModuleCount()),
                m_power(DEFAULT_TURBULENCE_POWER),
                m_xDistortModule(),
                m_yDistortModule(),
                m_zDistortModule() {
                SetSeed(DEFAULT_TURBULENCE_SEED);
                SetFrequency(DEFAULT_TURBULENCE_FREQUENCY);
                SetRoughness(DEFAULT_TURBULENCE_ROUGHNESS);
            }

            /// Returns the frequency of the turbulence.
            ///
            /// @returns The frequency of the turbulence.
            ///
            /// The frequency determines how rapidly the displacement amount changes.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_xDistortModule.GetFrequency();
            }

            /// Returns the power of the turbulence.
            ///
            /// @returns The power of the turbulence.
            ///
            /// The power determines the scaling factor applied to the displacement amount.
            [[nodiscard]] inline double GetPower() const noexcept {
                return m_power;
            }

            /// Returns the roughness of the turbulence.
            ///
            /// @returns The roughness of the turbulence.
            ///
            /// The roughness determines the number of octaves in the internal Perlin
            /// noise modules, affecting the smoothness of displacement changes.
            [[nodiscard]] inline int GetRoughnessCount() const noexcept {
                return m_xDistortModule.GetOctaveCount();
            }

            /// Returns the seed value of the internal Perlin noise modules.
            ///
            /// @returns The seed value.
            ///
            /// The seed values for the internal Perlin modules are offset from this
            /// value: x uses seed, y uses seed + 1, and z uses seed + 2.
            [[nodiscard]] inline int GetSeed() const noexcept {
                return m_xDistortModule.GetSeed();
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");

                double x0 = x + (12414.0 / 65536.0);
                double y0 = y + (65124.0 / 65536.0);
                double z0 = z + (31337.0 / 65536.0);
                double x1 = x + (26519.0 / 65536.0);
                double y1 = y + (18128.0 / 65536.0);
                double z1 = z + (60493.0 / 65536.0);
                double x2 = x + (53820.0 / 65536.0);
                double y2 = y + (11213.0 / 65536.0);
                double z2 = z + (44845.0 / 65536.0);

                double xDistort = x + (m_xDistortModule.GetValue(x0, y0, z0) * m_power);
                double yDistort = y + (m_yDistortModule.GetValue(x1, y1, z1) * m_power);
                double zDistort = z + (m_zDistortModule.GetValue(x2, y2, z2) * m_power);

                return m_sourceModules[0]->GetValue(xDistort, yDistort, zDistort);
            }

            /// Sets the frequency of the turbulence.
            ///
            /// @param frequency The frequency of the turbulence.
            ///
            /// The frequency determines how rapidly the displacement amount changes.
            inline void SetFrequency(double frequency) noexcept {
                m_xDistortModule.SetFrequency(frequency);
                m_yDistortModule.SetFrequency(frequency);
                m_zDistortModule.SetFrequency(frequency);
            }

            /// Sets the power of the turbulence.
            ///
            /// @param power The power of the turbulence.
            ///
            /// The power determines the scaling factor applied to the displacement amount.
            inline void SetPower(double power) noexcept {
                m_power = power;
            }

            /// Sets the roughness of the turbulence.
            ///
            /// @param roughness The roughness of the turbulence.
            ///
            /// The roughness sets the number of octaves in the internal Perlin noise
            /// modules, affecting the smoothness of displacement changes.
            inline void SetRoughness(int roughness) {
                m_xDistortModule.SetOctaveCount(roughness);
                m_yDistortModule.SetOctaveCount(roughness);
                m_zDistortModule.SetOctaveCount(roughness);
            }

            void SetSeed(int seed) noexcept;

        protected:
            /// The power (scale) of the displacement.
            double m_power;

            /// Noise module that displaces the x coordinate.
            Perlin m_xDistortModule;

            /// Noise module that displaces the y coordinate.
            Perlin m_yDistortModule;

            /// Noise module that displaces the z coordinate.
            Perlin m_zDistortModule;
        };

    } // namespace module

} // namespace noise