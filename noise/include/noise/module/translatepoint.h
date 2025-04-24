// translatepoint.h
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

        /// Default translation factor applied to the x coordinate for the TranslatePoint noise module.
        inline constexpr double DEFAULT_TRANSLATE_POINT_X = 0.0;

        /// Default translation factor applied to the y coordinate for the TranslatePoint noise module.
        inline constexpr double DEFAULT_TRANSLATE_POINT_Y = 0.0;

        /// Default translation factor applied to the z coordinate for the TranslatePoint noise module.
        inline constexpr double DEFAULT_TRANSLATE_POINT_Z = 0.0;

        /// Noise module that translates the coordinates of the input value before
        /// returning the output value from a source module.
        ///
        /// @image html moduletranslatepoint.png
        ///
        /// The GetValue() method translates the (x, y, z) coordinates of the input
        /// value by a specified amount before returning the output value from the
        /// source module. Translation amounts can be set uniformly with
        /// SetTranslation() or individually with SetXTranslation(), SetYTranslation(),
        /// or SetZTranslation().
        ///
        /// This noise module requires one source module.
        class TranslatePoint : public Module {
        public:
            /// Constructor.
            TranslatePoint() noexcept
                : Module(GetSourceModuleCount()),
                m_xTranslation(DEFAULT_TRANSLATE_POINT_X),
                m_yTranslation(DEFAULT_TRANSLATE_POINT_Y),
                m_zTranslation(DEFAULT_TRANSLATE_POINT_Z) {
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Returns the translated output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The translated output value.
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
                return m_sourceModules[0]->GetValue(x + m_xTranslation, y + m_yTranslation, z + m_zTranslation);
            }

            /// Returns the translation amount applied to the x coordinate.
            ///
            /// @returns The translation amount for the x coordinate.
            [[nodiscard]] inline double GetXTranslation() const noexcept {
                return m_xTranslation;
            }

            /// Returns the translation amount applied to the y coordinate.
            ///
            /// @returns The translation amount for the y coordinate.
            [[nodiscard]] inline double GetYTranslation() const noexcept {
                return m_yTranslation;
            }

            /// Returns the translation amount applied to the z coordinate.
            ///
            /// @returns The translation amount for the z coordinate.
            [[nodiscard]] inline double GetZTranslation() const noexcept {
                return m_zTranslation;
            }

            /// Sets the translation amount to apply uniformly to all coordinates.
            ///
            /// @param translation The translation amount to apply.
            inline void SetTranslation(double translation) noexcept {
                m_xTranslation = translation;
                m_yTranslation = translation;
                m_zTranslation = translation;
            }

            /// Sets the translation amounts for each coordinate individually.
            ///
            /// @param xTranslation The translation amount for the x coordinate.
            /// @param yTranslation The translation amount for the y coordinate.
            /// @param zTranslation The translation amount for the z coordinate.
            inline void SetTranslation(double xTranslation, double yTranslation, double zTranslation) noexcept {
                m_xTranslation = xTranslation;
                m_yTranslation = yTranslation;
                m_zTranslation = zTranslation;
            }

            /// Sets the translation amount for the x coordinate.
            ///
            /// @param xTranslation The translation amount for the x coordinate.
            inline void SetXTranslation(double xTranslation) noexcept {
                m_xTranslation = xTranslation;
            }

            /// Sets the translation amount for the y coordinate.
            ///
            /// @param yTranslation The translation amount for the y coordinate.
            inline void SetYTranslation(double yTranslation) noexcept {
                m_yTranslation = yTranslation;
            }

            /// Sets the translation amount for the z coordinate.
            ///
            /// @param zTranslation The translation amount for the z coordinate.
            inline void SetZTranslation(double zTranslation) noexcept {
                m_zTranslation = zTranslation;
            }

        protected:
            /// Translation amount applied to the x coordinate of the input value.
            double m_xTranslation;

            /// Translation amount applied to the y coordinate of the input value.
            double m_yTranslation;

            /// Translation amount applied to the z coordinate of the input value.
            double m_zTranslation;
        };

    } // namespace module

} // namespace noise