// scalepoint.h
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

        /// Default scaling factor applied to the x coordinate for the ScalePoint noise module.
        inline constexpr double DEFAULT_SCALE_POINT_X = 1.0;

        /// Default scaling factor applied to the y coordinate for the ScalePoint noise module.
        inline constexpr double DEFAULT_SCALE_POINT_Y = 1.0;

        /// Default scaling factor applied to the z coordinate for the ScalePoint noise module.
        inline constexpr double DEFAULT_SCALE_POINT_Z = 1.0;

        /// Noise module that scales the coordinates of the input value before returning
        /// the output value from a source module.
        ///
        /// @image html modulescalepoint.png
        ///
        /// The GetValue() method multiplies the (x, y, z) coordinates of the input value
        /// by scaling factors before returning the output value from the source module.
        /// Scaling can be set uniformly with SetScale() or individually with
        /// SetXScale(), SetYScale(), or SetZScale().
        ///
        /// This noise module requires one source module.
        class ScalePoint : public Module {
        public:
            /// Constructor.
            ScalePoint() noexcept
                : Module(GetSourceModuleCount()),
                m_xScale(DEFAULT_SCALE_POINT_X),
                m_yScale(DEFAULT_SCALE_POINT_Y),
                m_zScale(DEFAULT_SCALE_POINT_Z) {
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Returns the scaled output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The scaled output value.
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
                return m_sourceModules[0]->GetValue(x * m_xScale, y * m_yScale, z * m_zScale);
            }

            /// Returns the scaling factor applied to the x coordinate.
            ///
            /// @returns The scaling factor for the x coordinate.
            [[nodiscard]] inline double GetXScale() const noexcept {
                return m_xScale;
            }

            /// Returns the scaling factor applied to the y coordinate.
            ///
            /// @returns The scaling factor for the y coordinate.
            [[nodiscard]] inline double GetYScale() const noexcept {
                return m_yScale;
            }

            /// Returns the scaling factor applied to the z coordinate.
            ///
            /// @returns The scaling factor for the z coordinate.
            [[nodiscard]] inline double GetZScale() const noexcept {
                return m_zScale;
            }

            /// Sets the scaling factor to apply uniformly to all coordinates.
            ///
            /// @param scale The scaling factor to apply.
            inline void SetScale(double scale) noexcept {
                m_xScale = scale;
                m_yScale = scale;
                m_zScale = scale;
            }

            /// Sets the scaling factors to apply to each coordinate individually.
            ///
            /// @param xScale The scaling factor for the x coordinate.
            /// @param yScale The scaling factor for the y coordinate.
            /// @param zScale The scaling factor for the z coordinate.
            inline void SetScale(double xScale, double yScale, double zScale) noexcept {
                m_xScale = xScale;
                m_yScale = yScale;
                m_zScale = zScale;
            }

            /// Sets the scaling factor for the x coordinate.
            ///
            /// @param xScale The scaling factor for the x coordinate.
            inline void SetXScale(double xScale) noexcept {
                m_xScale = xScale;
            }

            /// Sets the scaling factor for the y coordinate.
            ///
            /// @param yScale The scaling factor for the y coordinate.
            inline void SetYScale(double yScale) noexcept {
                m_yScale = yScale;
            }

            /// Sets the scaling factor for the z coordinate.
            ///
            /// @param zScale The scaling factor for the z coordinate.
            inline void SetZScale(double zScale) noexcept {
                m_zScale = zScale;
            }

        protected:
            /// Scaling factor applied to the x coordinate of the input value.
            double m_xScale;

            /// Scaling factor applied to the y coordinate of the input value.
            double m_yScale;

            /// Scaling factor applied to the z coordinate of the input value.
            double m_zScale;
        };

    } // namespace module

} // namespace noise