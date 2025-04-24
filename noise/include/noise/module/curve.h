// curve.h
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
// - Moved GetControlPointArray, GetControlPointCount, GetSourceModuleCount, and ClearAllControlPoints to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Replaced raw pointer array with std::vector for control points to improve safety.
// - Removed destructor as std::vector handles cleanup.
// - Updated AddControlPoint, FindInsertionPos, and InsertAtPos to use std::vector.
// - Improved documentation with consistent formatting and mathematical details.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert>  // For assert
#include <vector>   // For std::vector
#include "../interp.h"
#include "../misc.h"
#include "modulebase.h"

namespace noise {

    namespace module {

        /// A structure defining a control point for the cubic spline.
        struct ControlPoint {
            /// The input value.
            double inputValue;

            /// The output value that maps to the input value.
            double outputValue;
        };

        /// A noise module that maps the output value from a source module onto a cubic spline.
        ///
        /// @image html modulecurve.png
        ///
        /// This noise module maps the output value from its source module onto a cubic spline
        /// defined by a set of control points. Each control point consists of an input value
        /// and an output value, forming a mapping from the source module’s output to a new value.
        ///
        /// @image html curve.png
        ///
        /// To define the spline, add control points using AddControlPoint(). The control points
        /// must be added in order of increasing input value; attempting to add a control point
        /// with a duplicate input value will throw an exception.
        ///
        /// This module requires at least four control points to perform cubic interpolation.
        /// If fewer than four points are present, GetValue() will fail with an assertion.
        /// The spline is evaluated using cubic interpolation between the control points
        /// closest to the source module’s output value.
        ///
        /// This noise module requires one source module.
        class Curve : public Module {
        public:
            /// Constructor.
            Curve() noexcept : Module(GetSourceModuleCount()), m_controlPoints() {}

            /// Adds a control point to the cubic spline.
            ///
            /// @param inputValue The input value of the control point.
            /// @param outputValue The output value of the control point.
            ///
            /// @pre No two control points can have the same input value.
            /// @throw noise::ExceptionInvalidParam If the input value already exists.
            ///
            /// Control points are stored in order of increasing input value.
            void AddControlPoint(double inputValue, double outputValue);

            /// Removes all control points from the cubic spline.
            inline void ClearAllControlPoints() noexcept {
                m_controlPoints.clear();
            }

            /// Returns a pointer to the array of control points on the curve.
            ///
            /// @returns A pointer to the control points.
            ///
            /// Use GetControlPointCount() to determine the number of control points.
            /// Avoid storing this pointer for later use, as it may become invalid if
            /// other methods of this object are called.
            [[nodiscard]] inline const ControlPoint* GetControlPointArray() const noexcept {
                return m_controlPoints.data();
            }

            /// Returns the number of control points on the curve.
            ///
            /// @returns The number of control points.
            [[nodiscard]] inline int GetControlPointCount() const noexcept {
                return static_cast<int>(m_controlPoints.size());
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires exactly one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Maps the source module's output value onto the cubic spline.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The mapped output value.
            /// @pre The source module (index 0) has been set.
            /// @pre At least four control points have been added.
            double GetValue(double x, double y, double z) const noexcept override;

        protected:
            /// Finds the position to insert a new control point while maintaining sorted order.
            ///
            /// @param inputValue The input value of the control point to insert.
            ///
            /// @returns The index at which to insert the control point.
            /// @throw noise::ExceptionInvalidParam If the input value already exists.
            int FindInsertionPos(double inputValue) const;

            /// Inserts a control point at the specified position in the control point vector.
            ///
            /// @param insertionPos The position to insert the control point.
            /// @param inputValue The input value of the control point.
            /// @param outputValue The output value of the control point.
            void InsertAtPos(int insertionPos, double inputValue, double outputValue);

            /// Vector storing the control points, sorted by input value.
            std::vector<ControlPoint> m_controlPoints;
        };

    } // namespace module

} // namespace noise