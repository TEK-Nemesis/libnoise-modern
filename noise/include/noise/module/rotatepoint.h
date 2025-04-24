// rotatepoint.h
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
// - Used std::sin and std::cos for modern C++ style.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Added necessary include for std::sin and std::cos (<cmath>).
// - Added include for mathconsts.h (../mathconsts.h).

#pragma once

#include <cassert>   // For assert
#include <cmath>     // For std::sin, std::cos
#include "../mathconsts.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default x rotation angle for the RotatePoint noise module.
        inline constexpr double DEFAULT_ROTATE_X = 0.0;

        /// Default y rotation angle for the RotatePoint noise module.
        inline constexpr double DEFAULT_ROTATE_Y = 0.0;

        /// Default z rotation angle for the RotatePoint noise module.
        inline constexpr double DEFAULT_ROTATE_Z = 0.0;

        /// Noise module that rotates the input value around the origin before
        /// returning the output value from a source module.
        ///
        /// @image html modulerotatepoint.png
        ///
        /// The GetValue() method rotates the coordinates of the input value around
        /// the origin before returning the output value from the source module.
        /// Rotation angles are set using SetAngles() or individually with
        /// SetXAngle(), SetYAngle(), or SetZAngle().
        ///
        /// The coordinate system is assumed to be "left-handed" (x increases to the
        /// right, y increases upward, z increases inward).
        ///
        /// This noise module requires one source module.
        class RotatePoint : public Module {
        public:
            /// Constructor.
            RotatePoint() noexcept : Module(GetSourceModuleCount()) {
                SetAngles(DEFAULT_ROTATE_X, DEFAULT_ROTATE_Y, DEFAULT_ROTATE_Z);
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Returns the result of rotating the input coordinates and retrieving
            /// the output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The output value from the source module after rotation.
            /// @pre The source module (index 0) has been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");

                double nx = (m_x1Matrix * x) + (m_y1Matrix * y) + (m_z1Matrix * z);
                double ny = (m_x2Matrix * x) + (m_y2Matrix * y) + (m_z2Matrix * z);
                double nz = (m_x3Matrix * x) + (m_y3Matrix * y) + (m_z3Matrix * z);
                return m_sourceModules[0]->GetValue(nx, ny, nz);
            }

            /// Returns the rotation angle around the x axis.
            ///
            /// @returns The rotation angle around the x axis, in degrees.
            [[nodiscard]] inline double GetXAngle() const noexcept {
                return m_xAngle;
            }

            /// Returns the rotation angle around the y axis.
            ///
            /// @returns The rotation angle around the y axis, in degrees.
            [[nodiscard]] inline double GetYAngle() const noexcept {
                return m_yAngle;
            }

            /// Returns the rotation angle around the z axis.
            ///
            /// @returns The rotation angle around the z axis, in degrees.
            [[nodiscard]] inline double GetZAngle() const noexcept {
                return m_zAngle;
            }

            /// Sets the rotation angles around all three axes.
            ///
            /// @param xAngle The rotation angle around the x axis, in degrees.
            /// @param yAngle The rotation angle around the y axis, in degrees.
            /// @param zAngle The rotation angle around the z axis, in degrees.
            void SetAngles(double xAngle, double yAngle, double zAngle) noexcept;

            /// Sets the rotation angle around the x axis.
            ///
            /// @param xAngle The rotation angle around the x axis, in degrees.
            inline void SetXAngle(double xAngle) noexcept {
                SetAngles(xAngle, m_yAngle, m_zAngle);
            }

            /// Sets the rotation angle around the y axis.
            ///
            /// @param yAngle The rotation angle around the y axis, in degrees.
            inline void SetYAngle(double yAngle) noexcept {
                SetAngles(m_xAngle, yAngle, m_zAngle);
            }

            /// Sets the rotation angle around the z axis.
            ///
            /// @param zAngle The rotation angle around the z axis, in degrees.
            inline void SetZAngle(double zAngle) noexcept {
                SetAngles(m_xAngle, m_yAngle, zAngle);
            }

        protected:
            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_x1Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_x2Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_x3Matrix;

            /// x rotation angle applied to the input value, in degrees.
            double m_xAngle;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_y1Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_y2Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_y3Matrix;

            /// y rotation angle applied to the input value, in degrees.
            double m_yAngle;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_z1Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_z2Matrix;

            /// Entry within the 3x3 rotation matrix used for rotating the input value.
            double m_z3Matrix;

            /// z rotation angle applied to the input value, in degrees.
            double m_zAngle;
        };

    } // namespace module

} // namespace noise