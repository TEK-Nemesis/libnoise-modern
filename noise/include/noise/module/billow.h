// billow.h
//
// Copyright (C) 2004 Jason Bevins
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
// - Moved simple member functions to header as inline to improve efficiency.
// - Used inline constexpr for default constants to ensure proper linkage.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting and mathematical details.
// - Removed redundant Doxygen group tags.
// - Removed constexpr from GetSourceModuleCount as virtual functions cannot be constexpr.
// - Added noexcept to GetValue declaration to match definition, fixing redefinition error.

#pragma once

#include "../noisegen.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default frequency for the Billow noise module.
        inline constexpr double DEFAULT_BILLOW_FREQUENCY = 1.0;

        /// Default lacunarity for the Billow noise module.
        inline constexpr double DEFAULT_BILLOW_LACUNARITY = 2.0;

        /// Default number of octaves for the Billow noise module.
        inline constexpr int DEFAULT_BILLOW_OCTAVE_COUNT = 6;

        /// Default persistence value for the Billow noise module.
        inline constexpr double DEFAULT_BILLOW_PERSISTENCE = 0.5;

        /// Default noise quality for the Billow noise module.
        inline constexpr NoiseQuality DEFAULT_BILLOW_QUALITY = NoiseQuality::QUALITY_STD;

        /// Default noise seed for the Billow noise module.
        inline constexpr int DEFAULT_BILLOW_SEED = 0;

        /// Maximum number of octaves for the Billow noise module.
        inline constexpr int BILLOW_MAX_OCTAVE = 30;

        /// A noise module that outputs three-dimensional "billowy" noise.
        ///
        /// @image html modulebillow.png
        ///
        /// This noise module generates "billowy" noise suitable for clouds and rocks.
        ///
        /// This noise module is nearly identical to Perlin noise, except it modifies each
        /// octave with an absolute-value function: \( \text{signal} = 2 \cdot |\text{signal}| - 1 \).
        /// See the documentation of Perlin for more information.
        ///
        /// This noise module does not require any source modules.
        class Billow final : public Module {
        public:
            /// Constructor.
            ///
            /// The default frequency is set to DEFAULT_BILLOW_FREQUENCY.
            /// The default lacunarity is set to DEFAULT_BILLOW_LACUNARITY.
            /// The default number of octaves is set to DEFAULT_BILLOW_OCTAVE_COUNT.
            /// The default persistence value is set to DEFAULT_BILLOW_PERSISTENCE.
            /// The default noise quality is set to DEFAULT_BILLOW_QUALITY.
            /// The default seed value is set to DEFAULT_BILLOW_SEED.
            Billow() noexcept
                : Module(GetSourceModuleCount()),
                m_frequency(DEFAULT_BILLOW_FREQUENCY),
                m_lacunarity(DEFAULT_BILLOW_LACUNARITY),
                m_noiseQuality(DEFAULT_BILLOW_QUALITY),
                m_octaveCount(DEFAULT_BILLOW_OCTAVE_COUNT),
                m_persistence(DEFAULT_BILLOW_PERSISTENCE),
                m_seed(DEFAULT_BILLOW_SEED) {
            }

            /// Returns the frequency of the first octave.
            ///
            /// @returns The frequency of the first octave.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the lacunarity of the billowy noise.
            ///
            /// @returns The lacunarity of the billowy noise.
            ///
            /// The lacunarity is the frequency multiplier between successive octaves.
            [[nodiscard]] inline double GetLacunarity() const noexcept {
                return m_lacunarity;
            }

            /// Returns the quality of the billowy noise.
            ///
            /// @returns The quality of the billowy noise.
            ///
            /// See NoiseQuality for definitions of the various coherent-noise qualities.
            [[nodiscard]] inline NoiseQuality GetNoiseQuality() const noexcept {
                return m_noiseQuality;
            }

            /// Returns the number of octaves that generate the billowy noise.
            ///
            /// @returns The number of octaves that generate the billowy noise.
            ///
            /// The number of octaves controls the amount of detail in the billowy noise.
            [[nodiscard]] inline int GetOctaveCount() const noexcept {
                return m_octaveCount;
            }

            /// Returns the persistence value of the billowy noise.
            ///
            /// @returns The persistence value of the billowy noise.
            ///
            /// The persistence value controls the roughness of the billowy noise.
            [[nodiscard]] inline double GetPersistence() const noexcept {
                return m_persistence;
            }

            /// Returns the seed value used by the billowy noise function.
            ///
            /// @returns The seed value.
            [[nodiscard]] inline int GetSeed() const noexcept {
                return m_seed;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Generates an output value given the coordinates of the input value.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The output value generated by the billowy noise function.
            double GetValue(double x, double y, double z) const noexcept override;

            /// Sets the frequency of the first octave.
            ///
            /// @param frequency The frequency of the first octave.
            inline void SetFrequency(double frequency) noexcept {
                m_frequency = frequency;
            }

            /// Sets the lacunarity of the billowy noise.
            ///
            /// @param lacunarity The lacunarity of the billowy noise.
            ///
            /// The lacunarity is the frequency multiplier between successive octaves.
            /// For best results, set the lacunarity to a number between 1.5 and 3.5.
            inline void SetLacunarity(double lacunarity) noexcept {
                m_lacunarity = lacunarity;
            }

            /// Sets the quality of the billowy noise.
            ///
            /// @param noiseQuality The quality of the billowy noise.
            ///
            /// See NoiseQuality for definitions of the various coherent-noise qualities.
            inline void SetNoiseQuality(NoiseQuality noiseQuality) noexcept {
                m_noiseQuality = noiseQuality;
            }

            /// Sets the number of octaves that generate the billowy noise.
            ///
            /// @param octaveCount The number of octaves that generate the billowy noise.
            ///
            /// @pre The number of octaves ranges from 1 to BILLOW_MAX_OCTAVE.
            ///
            /// @throw noise::ExceptionInvalidParam An invalid parameter was specified;
            /// see the preconditions for more information.
            ///
            /// The number of octaves controls the amount of detail in the billowy noise.
            /// The larger the number of octaves, the more time required to calculate the
            /// billowy noise value.
            void SetOctaveCount(int octaveCount);

            /// Sets the persistence value of the billowy noise.
            ///
            /// @param persistence The persistence value of the billowy noise.
            ///
            /// The persistence value controls the roughness of the billowy noise.
            /// For best results, set the persistence value to a number between 0.0 and 1.0.
            inline void SetPersistence(double persistence) noexcept {
                m_persistence = persistence;
            }

            /// Sets the seed value used by the billowy noise function.
            ///
            /// @param seed The seed value.
            inline void SetSeed(int seed) noexcept {
                m_seed = seed;
            }

        private:
            /// Frequency of the first octave.
            double m_frequency;

            /// Frequency multiplier between successive octaves.
            double m_lacunarity;

            /// Quality of the billowy noise.
            NoiseQuality m_noiseQuality;

            /// Total number of octaves that generate the billowy noise.
            int m_octaveCount;

            /// Persistence value of the billowy noise.
            double m_persistence;

            /// Seed value used by the billowy noise function.
            int m_seed;
        };

    } // namespace module

} // namespace noise