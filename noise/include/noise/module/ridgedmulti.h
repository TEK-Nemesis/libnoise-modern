// ridgedmulti.h
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
// - Used std::pow and std::fabs for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Added necessary include for NoiseQuality (../noisegen.h).

#pragma once

#include <cassert>   // For assert
#include <cmath>     // For std::pow, std::fabs
#include "../noisegen.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default frequency for the RidgedMulti noise module.
        inline constexpr double DEFAULT_RIDGED_FREQUENCY = 1.0;

        /// Default lacunarity for the RidgedMulti noise module.
        inline constexpr double DEFAULT_RIDGED_LACUNARITY = 2.0;

        /// Default number of octaves for the RidgedMulti noise module.
        inline constexpr int DEFAULT_RIDGED_OCTAVE_COUNT = 6;

        /// Default noise quality for the RidgedMulti noise module.
        inline constexpr noise::NoiseQuality DEFAULT_RIDGED_QUALITY = noise::NoiseQuality::QUALITY_STD;

        /// Default noise seed for the RidgedMulti noise module.
        inline constexpr int DEFAULT_RIDGED_SEED = 0;

        /// Maximum number of octaves for the RidgedMulti noise module.
        inline constexpr int RIDGED_MAX_OCTAVE = 30;

        /// Noise module that outputs 3-dimensional ridged-multifractal noise.
        ///
        /// @image html moduleridgedmulti.png
        ///
        /// Ridged-multifractal noise is generated similarly to Perlin noise, but each
        /// octave's output is modified by an absolute-value function to create ridge-like
        /// formations. Unlike Perlin noise, it does not use a persistence value, as
        /// octave weights are based on previous octaves, creating a feedback loop.
        ///
        /// Output values typically range from -1.0 to +1.0, but this is not guaranteed.
        /// With only one octave, the output ranges from -1.0 to 0.0.
        ///
        /// This noise module does not require any source modules and is often used to
        /// generate craggy mountainous terrain or marble-like textures.
        ///
        /// ### Octaves
        /// The number of octaves controls the amount of detail in the noise. More octaves
        /// increase detail but also computation time.
        ///
        /// ### Frequency
        /// The frequency of the first octave can be set using SetFrequency().
        ///
        /// ### Lacunarity
        /// The lacunarity specifies the frequency multiplier between successive octaves.
        /// Best results are achieved with values between 1.5 and 3.5.
        class RidgedMulti : public Module {
        public:
            /// Constructor.
            RidgedMulti() noexcept
                : Module(GetSourceModuleCount()),
                m_frequency(DEFAULT_RIDGED_FREQUENCY),
                m_lacunarity(DEFAULT_RIDGED_LACUNARITY),
                m_noiseQuality(DEFAULT_RIDGED_QUALITY),
                m_octaveCount(DEFAULT_RIDGED_OCTAVE_COUNT),
                m_seed(DEFAULT_RIDGED_SEED) {
                CalcSpectralWeights();
            }

            /// Returns the frequency of the first octave.
            ///
            /// @returns The frequency of the first octave.
            [[nodiscard]] inline double GetFrequency() const noexcept {
                return m_frequency;
            }

            /// Returns the lacunarity of the ridged-multifractal noise.
            ///
            /// @returns The lacunarity of the ridged-multifractal noise.
            ///
            /// The lacunarity is the frequency multiplier between successive octaves.
            [[nodiscard]] inline double GetLacunarity() const noexcept {
                return m_lacunarity;
            }

            /// Returns the quality of the ridged-multifractal noise.
            ///
            /// @returns The quality of the ridged-multifractal noise.
            [[nodiscard]] inline noise::NoiseQuality GetNoiseQuality() const noexcept {
                return m_noiseQuality;
            }

            /// Returns the number of octaves that generate the ridged-multifractal noise.
            ///
            /// @returns The number of octaves.
            ///
            /// The number of octaves controls the amount of detail in the noise.
            [[nodiscard]] inline int GetOctaveCount() const noexcept {
                return m_octaveCount;
            }

            /// Returns the seed value used by the ridged-multifractal-noise function.
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

            /// Sets the lacunarity of the ridged-multifractal noise.
            ///
            /// @param lacunarity The lacunarity of the ridged-multifractal noise.
            ///
            /// For best results, set the lacunarity to a number between 1.5 and 3.5.
            inline void SetLacunarity(double lacunarity) noexcept {
                m_lacunarity = lacunarity;
                CalcSpectralWeights();
            }

            /// Sets the quality of the ridged-multifractal noise.
            ///
            /// @param noiseQuality The quality of the ridged-multifractal noise.
            inline void SetNoiseQuality(noise::NoiseQuality noiseQuality) noexcept {
                m_noiseQuality = noiseQuality;
            }

            /// Sets the number of octaves that generate the ridged-multifractal noise.
            ///
            /// @param octaveCount The number of octaves.
            ///
            /// @throw noise::ExceptionInvalidParam If octaveCount exceeds RIDGED_MAX_OCTAVE.
            ///
            /// The number of octaves controls the amount of detail in the noise.
            inline void SetOctaveCount(int octaveCount) {
                if (octaveCount > RIDGED_MAX_OCTAVE) {
                    throw noise::ExceptionInvalidParam();
                }
                m_octaveCount = octaveCount;
            }

            /// Sets the seed value used by the ridged-multifractal-noise function.
            ///
            /// @param seed The seed value.
            inline void SetSeed(int seed) noexcept {
                m_seed = seed;
            }

        protected:
            /// Calculates the spectral weights for each octave.
            void CalcSpectralWeights() noexcept;

            /// Frequency of the first octave.
            double m_frequency;

            /// Frequency multiplier between successive octaves.
            double m_lacunarity;

            /// Quality of the ridged-multifractal noise.
            noise::NoiseQuality m_noiseQuality;

            /// Total number of octaves that generate the ridged-multifractal noise.
            int m_octaveCount;

            /// Contains the spectral weights for each octave.
            double m_pSpectralWeights[RIDGED_MAX_OCTAVE];

            /// Seed value used by the ridged-multifractal-noise function.
            int m_seed;
        };

    } // namespace module

} // namespace noise