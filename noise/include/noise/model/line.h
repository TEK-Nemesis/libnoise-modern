// line.h
//
// Copyright (C) 2004 Keith Davies
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
// The developer's email is not specified.
//
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 23, 2025:
// - Replaced include guard with #pragma once.
// - Added noexcept to member functions that do not throw exceptions.
// - Added [[nodiscard]] to GetAttenuate and GetModule to encourage proper use of return values.
// - Moved simple member functions to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer lists in constructors for clarity and efficiency.
// - Added debug assertion in GetValue to enforce p range when attenuation is enabled.
// - Improved documentation with mathematical formulas and consistent formatting.
// - Removed redundant Doxygen group tags and unclear documentation comments.

#pragma once

#include <cassert> // For assert
#include "../module/modulebase.h"

namespace noise {

    namespace model {

        /// Models a line segment in 3D space for noise generation.
        ///
        /// This class returns an output value from a noise module given a one-dimensional
        /// coordinate along a line segment, which can be used as displacements for creating
        /// roads, rivers, or similar linear features.
        ///
        /// To generate an output value, pass a parameter @a p between 0.0 and 1.0 to the
        /// GetValue() method. The value 0.0 represents the start position of the line segment,
        /// and 1.0 represents the end position.
        class Line {
        public:
            /// Constructor.
            Line() noexcept
                : m_attenuate(true), m_pModule(nullptr),
                m_x0(0.0), m_x1(1.0),
                m_y0(0.0), m_y1(1.0),
                m_z0(0.0), m_z1(1.0) {
            }

            /// Constructor.
            ///
            /// @param module The noise module used to generate the output values.
            explicit Line(const module::Module& module) noexcept
                : m_attenuate(true), m_pModule(&module),
                m_x0(0.0), m_x1(1.0),
                m_y0(0.0), m_y1(1.0),
                m_z0(0.0), m_z1(1.0) {
            }

            /// Returns whether the output value is attenuated (moved toward 0.0) as the ends
            /// of the line segment are approached.
            ///
            /// @returns True if attenuation is enabled, false otherwise.
            [[nodiscard]] inline bool GetAttenuate() const noexcept {
                return m_attenuate;
            }

            /// Returns the noise module used to generate the output values.
            ///
            /// @returns A reference to the noise module.
            /// @pre A noise module was set using the SetModule() method.
            [[nodiscard]] inline const module::Module& GetModule() const noexcept {
                assert(m_pModule != nullptr && "Noise module must be set before calling GetModule");
                return *m_pModule;
            }

            /// Returns the output value from the noise module given the one-dimensional
            /// coordinate of the specified input value along the line segment.
            ///
            /// @param p The distance along the line segment, typically in [0, 1].
            ///
            /// @returns The output value from the noise module.
            /// @pre A noise module was set using the SetModule() method.
            /// @pre The start and end points of the line segment were specified.
            ///
            /// The coordinates (x, y, z) along the line are computed as:
            /// - \( x = (x_1 - x_0) \cdot p + x_0 \)
            /// - \( y = (y_1 - y_0) \cdot p + y_0 \)
            /// - \( z = (z_1 - z_0) \cdot p + z_0 \)
            ///
            /// If attenuation is enabled (default), the output value is scaled by
            /// \( p \cdot (1 - p) \cdot 4 \) to move it toward 0.0 at the ends. When
            /// attenuation is enabled, @a p must be in [0, 1]. If attenuation is disabled,
            /// @a p can range outside [0, 1], and the output value will be extrapolated
            /// along the line.
            double GetValue(double p) const noexcept;

            /// Sets whether the output value is attenuated (moved toward 0.0) as the ends
            /// of the line segment are approached.
            ///
            /// @param att True to enable attenuation, false to disable it.
            inline void SetAttenuate(bool att) noexcept {
                m_attenuate = att;
            }

            /// Sets the end point of the line segment.
            ///
            /// @param x The x-coordinate of the end position.
            /// @param y The y-coordinate of the end position.
            /// @param z The z-coordinate of the end position.
            inline void SetEndPoint(double x, double y, double z) noexcept {
                m_x1 = x;
                m_y1 = y;
                m_z1 = z;
            }

            /// Sets the noise module used to generate the output values.
            ///
            /// @param module The noise module to use.
            ///
            /// This noise module must remain valid for the lifetime of this object
            /// until a new noise module is set.
            inline void SetModule(const module::Module& module) noexcept {
                m_pModule = &module;
            }

            /// Sets the start point of the line segment.
            ///
            /// @param x The x-coordinate of the start position.
            /// @param y The y-coordinate of the start position.
            /// @param z The z-coordinate of the start position.
            inline void SetStartPoint(double x, double y, double z) noexcept {
                m_x0 = x;
                m_y0 = y;
                m_z0 = z;
            }

        private:
            /// Whether to attenuate the value (move toward 0.0) at the ends of the line segment.
            bool m_attenuate;

            /// Pointer to the noise module used to generate the output values.
            const module::Module* m_pModule;

            /// x-coordinate of the start of the line segment.
            double m_x0;

            /// x-coordinate of the end of the line segment.
            double m_x1;

            /// y-coordinate of the start of the line segment.
            double m_y0;

            /// y-coordinate of the end of the line segment.
            double m_y1;

            /// z-coordinate of the start of the line segment.
            double m_z0;

            /// z-coordinate of the end of the line segment.
            double m_z1;
        };

    } // namespace model

} // namespace noise