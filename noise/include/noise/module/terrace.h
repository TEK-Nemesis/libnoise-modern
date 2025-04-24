// terrace.h
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
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Used std::vector for control points to improve safety and eliminate manual memory management.
// - Removed destructor since std::vector handles cleanup.
// - Updated methods to work with std::vector instead of raw pointer array.

#pragma once

#include <cassert>  // For assert
#include <vector>   // For std::vector
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Noise module that maps the output value from a source module onto a
        /// terrace-forming curve.
        ///
        /// @image html moduleterrace.png
        ///
        /// This noise module maps the output value from the source module onto a
        /// terrace-forming curve. The curve starts with a slope of zero, which
        /// smoothly increases, and resets to zero at control points, creating a
        /// "terracing" effect (see illustration: @image html terrace.png).
        ///
        /// Control points are added using AddControlPoint(), and at least two must
        /// be defined. The output is clamped to the lowest or highest control point
        /// if the source value falls outside their range. The curve can be inverted
        /// between control points using InvertTerraces().
        ///
        /// This noise module is often used to generate terrain features like desert
        /// canyons and requires one source module.
        class Terrace : public Module {
        public:
            /// Constructor.
            Terrace() noexcept
                : Module(GetSourceModuleCount()),
                m_controlPoints(),
                m_invertTerraces(false) {
            }

            /// Adds a control point to the terrace-forming curve.
            ///
            /// @param value The value of the control point to add.
            ///
            /// @throw noise::ExceptionInvalidParam If the value already exists.
            ///
            /// At least two control points must be defined. The points are automatically
            /// sorted by value.
            void AddControlPoint(double value);

            /// Deletes all control points on the terrace-forming curve.
            inline void ClearAllControlPoints() noexcept {
                m_controlPoints.clear();
            }

            /// Returns a pointer to the array of control points.
            ///
            /// @returns A pointer to the control points.
            ///
            /// Use GetControlPointCount() to determine the number of points. Do not
            /// store this pointer, as it may change with other method calls.
            [[nodiscard]] inline const double* GetControlPointArray() const noexcept {
                return m_controlPoints.data();
            }

            /// Returns the number of control points on the terrace-forming curve.
            ///
            /// @returns The number of control points.
            [[nodiscard]] inline int GetControlPointCount() const noexcept {
                return static_cast<int>(m_controlPoints.size());
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 1, as this module requires one source module.
            inline int GetSourceModuleCount() const noexcept override {
                return 1;
            }

            /// Enables or disables inversion of the terrace-forming curve.
            ///
            /// @param invert Specifies whether to invert the curve (default: true).
            inline void InvertTerraces(bool invert = true) noexcept {
                m_invertTerraces = invert;
            }

            /// Determines if the terrace-forming curve is inverted.
            ///
            /// @returns True if the curve is inverted, false otherwise.
            [[nodiscard]] inline bool IsTerracesInverted() const noexcept {
                return m_invertTerraces;
            }

            double GetValue(double x, double y, double z) const noexcept override;

            /// Creates equally-spaced control points ranging from -1 to +1.
            ///
            /// @param controlPointCount The number of control points to generate.
            ///
            /// @throw noise::ExceptionInvalidParam If controlPointCount < 2.
            void MakeControlPoints(int controlPointCount);

        protected:
            /// Determines the position to insert a new control point.
            ///
            /// @param value The value of the control point.
            ///
            /// @returns The insertion position.
            ///
            /// @throw noise::ExceptionInvalidParam If the value already exists.
            int FindInsertionPos(double value) const;

            /// Inserts a control point at the specified position.
            ///
            /// @param insertionPos The position to insert at.
            /// @param value The value of the control point.
            void InsertAtPos(int insertionPos, double value);

            /// Vector that stores the control points in sorted order.
            std::vector<double> m_controlPoints;

            /// Determines if the terrace-forming curve is inverted.
            bool m_invertTerraces;
        };

    } // namespace module

} // namespace noise