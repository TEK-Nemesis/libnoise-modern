// perlin.h
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
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Added necessary include for NoiseQuality (../noisegen.h).

#pragma once

#include <cassert>   // For assert
#include "../noisegen.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default frequency for the Perlin noise module.
        inline constexpr double DEFAULT_PERLIN_FREQUENCY = 1.0;

        /// Default lacunarity for the Perlin noise module.
        inline constexpr double DEFAULT_PERLIN_LACUNARITY = 2.0;

        /// Default number of octaves for the Perlin noise module.
        inline constexpr int DEFAULT_PERLIN_OCTAVE_COUNT = 6;

        /// Default persistence value for the Perlin noise module.
        inline constexpr double DEFAULT_PERLIN_PERSISTENCE = 0.5;

        /// Default noise quality for the Perlin noise module.
        inline constexpr noise::NoiseQuality DEFAULT_PERLIN_QUALITY = noise::NoiseQuality::QUALITY_STD;

        /// Default noise seed for the Perlin noise module.
        inline constexpr int DEFAULT_PERLIN_SEED = 0;

        /// Maximum number of octaves for the Perlin noise module.
        inline constexpr int PERLIN_MAX_OCTAVE = 30;

        /// Noise module that outputs 3-dimensional Perlin noise.
        ///
        /// @image html moduleperlin.png
        ///
        /// Perlin noise is the sum of several coherent-noise functions of
        /// ever-increasing frequencies and ever-decreasing amplitudes. A small change
        /// in the input value produces a small change in the output, while a large
        /// change produces a random change.
        ///
        /// Output values typically range from -1.0 to +1.0, but this is not guaranteed.
        ///
        /// This noise module does not require any source modules.
        ///
        /// ### Octaves
        /// The number of octaves controls the amount of detail in the Perlin noise.
        /// More octaves increase detail but also computation time. Each octave has
        /// double the frequency of the previous one by default.
        ///
        /// ### Frequency
        /// The frequency of the first octave can be set using SetFrequency().
        ///
        /// ### Persistence
        /// The persistence value controls the roughness of the noise. Larger values
        /// produce rougher noise. The amplitude of each octave is the previous
        /// octave's amplitude multiplied by the persistence value.
        ///
        /// ### Lacunarity
        /// The lacunarity specifies the frequency multiplier between successive octaves.
        /// Best results are achieved with values between 1.5 and 3.5.
        class Perlin : public Module {
        public:
            /// Constructor.
            Perlin() noexcept
                : Module(GetSourceModuleCount()),
                m_frequency(DEFAULT_PERLIN_FREQUENCY),
                m_lacunarity(DEFAULT_PERLIN_LACUNARITY),
                m_noiseQuality(DEFAULT_PERLIN_QUALITY),
                m_octaveCount(DEFAULT_PERLIN_OCTAVE_COUNT),
                m_persistence(DEFAULT_PERLIN_PERSISTENCE),
                m_seed(DEFAULT_PERLIN_SEED) {
            }

            /// Returns the frequency of the first octave.
            ///
            /// @returns The frequency of the first octave.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the lacunarity of the Perlin noise.
            ///
            /// @returns The lacunarity of the Perlin noise.
            ///
            /// The lacunarity is the frequency multiplier between successive octaves.
            [[nodiscard]] inline double GetLacunarity() const noexcept {
                return m_lacunarity;
            }

            /// Returns the quality of the Perlin noise.
            ///
            /// @returns The quality of the Perlin noise.
            [[nodiscard]] inline noise::NoiseQuality GetNoiseQuality() const noexcept {
                return m_noiseQuality;
            }

            /// Returns the number of octaves that generate the Perlin noise.
            ///
            /// @returns The number of octaves.
            ///
            /// The number of octaves controls the amount of detail in the Perlin noise.
            [[nodiscard]] inline int GetOctaveCount() const noexcept {
                return m_octaveCount;
            }

            /// Returns the persistence value of the Perlin noise.
            ///
            /// @returns The persistence value.
            ///
            /// The persistence value controls the roughness of the Perlin noise.
            [[nodiscard]] inline double GetPersistence() const noexcept {
                return m_persistence;
            }

            /// Returns the seed value used by the Perlin-noise function.
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

            double GetValue(double x, double y, double z) const noexcept override;

            /// Sets the frequency of the first octave.
            ///
            /// @param frequency The frequency of the first octave.
            inline void SetFrequency(double frequency) noexcept {
                m_frequency = frequency;
            }

            /// Sets the lacunarity of the Perlin noise.
            ///
            /// @param lacunarity The lacunarity of the Perlin noise.
            ///
            /// For best results, set the lacunarity to a number between 1.5 and 3.5.
            inline void SetLacunarity(double lacunarity) noexcept {
                m_lacunarity = lacunarity;
            }

            /// Sets the quality of the Perlin noise.
            ///
            /// @param noiseQuality The quality of the Perlin noise.
            inline void SetNoiseQuality(noise::NoiseQuality noiseQuality) noexcept {
                m_noiseQuality = noiseQuality;
            }

            /// Sets the number of octaves that generate the Perlin noise.
            ///
            /// @param octaveCount The number of octaves.
            ///
            /// @throw noise::ExceptionInvalidParam If octaveCount is not between 1 and PERLIN_MAX_OCTAVE.
            ///
            /// The number of octaves controls the amount of detail in the Perlin noise.
            inline void SetOctaveCount(int octaveCount) {
                if (octaveCount < 1 || octaveCount > PERLIN_MAX_OCTAVE) {
                    throw noise::ExceptionInvalidParam();
                }
                m_octaveCount = octaveCount;
            }

            /// Sets the persistence value of the Perlin noise.
            ///
            /// @param persistence The persistence value.
            ///
            /// For best results, set the persistence to a number between 0.0 and 1.0.
            inline void SetPersistence(double persistence) noexcept {
                m_persistence = persistence;
            }

            /// Sets the seed value used by the Perlin-noise function.
            ///
            /// @param seed The seed value.
            inline void SetSeed(int seed) noexcept {
                m_seed = seed;
            }

        protected:
            /// Frequency of the first octave.
            double m_frequency;

            /// Frequency multiplier between successive octaves.
            double m_lacunarity;

            /// Quality of the Perlin noise.
            noise::NoiseQuality m_noiseQuality;

            /// Total number of octaves that generate the Perlin noise.
            int m_octaveCount;

            /// Persistence of the Perlin noise.
            double m_persistence;

            /// Seed value used by the Perlin-noise function.
            int m_seed;
        };

    } // namespace module

} // namespace noise