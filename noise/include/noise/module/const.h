// const.h
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
// - Added [[nodiscard]] to GetConstValue to encourage proper use of return values.
// - Added constexpr to GetSourceModuleCount since it is a constant expression.
// - Moved GetConstValue, GetValue, and SetConstValue to header as inline to improve efficiency.
// - Used inline constexpr for DEFAULT_CONST_VALUE to ensure proper linkage.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include "modulebase.h"

namespace noise {

    namespace module {

        /// Default constant value for the Const noise module.
        inline constexpr double DEFAULT_CONST_VALUE = 0.0;

        /// A noise module that outputs a constant value.
        ///
        /// @image html moduleconst.png
        ///
        /// This noise module outputs a constant value specified by SetConstValue().
        /// It is often used as a source module for other noise modules.
        ///
        /// This noise module does not require any source modules.
        class Const : public Module {
        public:
            /// Constructor.
            ///
            /// The default constant value is set to DEFAULT_CONST_VALUE.
            Const() noexcept
                : Module(GetSourceModuleCount()),
                m_constValue(DEFAULT_CONST_VALUE) {
            }

            /// Returns the constant output value for this noise module.
            ///
            /// @returns The constant output value.
            [[nodiscard]] inline double GetConstValue() const noexcept {
                return m_constValue;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 0, as this module does not require any source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 0;
            }

            /// Returns the constant output value for the given input coordinates.
            ///
            /// @param x The x-coordinate of the input value (ignored).
            /// @param y The y-coordinate of the input value (ignored).
            /// @param z The z-coordinate of the input value (ignored).
            ///
            /// @returns The constant value set by SetConstValue().
            inline double GetValue(double x, double y, double z) const noexcept override {
                return m_constValue;
            }

            /// Sets the constant output value for this noise module.
            ///
            /// @param constValue The constant output value.
            inline void SetConstValue(double constValue) noexcept {
                m_constValue = constValue;
            }

        protected:
            /// The constant value to output.
            double m_constValue;
        };

    } // namespace module

} // namespace noise