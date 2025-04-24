// select.h
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

        /// Default edge-falloff value for the Select noise module.
        inline constexpr double DEFAULT_SELECT_EDGE_FALLOFF = 0.0;

        /// Default lower bound of the selection range for the Select noise module.
        inline constexpr double DEFAULT_SELECT_LOWER_BOUND = -1.0;

        /// Default upper bound of the selection range for the Select noise module.
        inline constexpr double DEFAULT_SELECT_UPPER_BOUND = 1.0;

        /// Noise module that outputs the value selected from one of two source modules
        /// chosen by the output value from a control module.
        ///
        /// @image html moduleselect.png
        ///
        /// This noise module requires three source modules:
        /// - Source module 0 (index 0): First output value.
        /// - Source module 1 (index 1): Second output value.
        /// - Source module 2 (index 2): Control module, which determines the selection.
        ///
        /// If the control module's output is within the selection range (set via
        /// SetBounds()), this module outputs the value from source module 1; otherwise,
        /// it outputs the value from source module 0. The edge transition can be smoothed
        /// by setting an edge falloff value with SetEdgeFalloff().
        class Select : public Module {
        public:
            /// Constructor.
            Select() noexcept
                : Module(GetSourceModuleCount()),
                m_edgeFalloff(DEFAULT_SELECT_EDGE_FALLOFF),
                m_lowerBound(DEFAULT_SELECT_LOWER_BOUND),
                m_upperBound(DEFAULT_SELECT_UPPER_BOUND) {
            }

            /// Returns the control module.
            ///
            /// @returns A reference to the control module.
            /// @throw noise::ExceptionNoModule If the control module is not set.
            [[nodiscard]] const Module& GetControlModule() const;

            /// Returns the falloff value at the edge transition.
            ///
            /// @returns The falloff value at the edge transition.
            ///
            /// The falloff value determines the width of the edge transition at either
            /// edge of the selection range.
            [[nodiscard]] inline double GetEdgeFalloff() const noexcept {
                return m_edgeFalloff;
            }

            /// Returns the lower bound of the selection range.
            ///
            /// @returns The lower bound of the selection range.
            [[nodiscard]] inline double GetLowerBound() const noexcept {
                return m_lowerBound;
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 3, as this module requires three source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 3;
            }

            /// Returns the upper bound of the selection range.
            ///
            /// @returns The upper bound of the selection range.
            [[nodiscard]] inline double GetUpperBound() const noexcept {
                return m_upperBound;
            }

            double GetValue(double x, double y, double z) const noexcept override;

            /// Sets the lower and upper bounds of the selection range.
            ///
            /// @param lowerBound The lower bound.
            /// @param upperBound The upper bound.
            ///
            /// @throw noise::ExceptionInvalidParam If lowerBound exceeds upperBound.
            void SetBounds(double lowerBound, double upperBound);

            /// Sets the control module.
            ///
            /// @param controlModule The control module.
            ///
            /// This method assigns the control module to index 2, equivalent to calling
            /// SetSourceModule(2, controlModule).
            ///
            /// The control module must remain valid for the lifetime of this object
            /// until replaced.
            inline void SetControlModule(const Module& controlModule) noexcept {
                assert(m_sourceModules.size() >= 3);
                m_sourceModules[2] = &controlModule;
            }

            /// Sets the falloff value at the edge transition.
            ///
            /// @param edgeFalloff The falloff value at the edge transition.
            ///
            /// The falloff value determines the width of the edge transition at either
            /// edge of the selection range. If the falloff exceeds half the range size,
            /// it is clamped to prevent overlap.
            void SetEdgeFalloff(double edgeFalloff) noexcept;

        protected:
            /// Edge-falloff value.
            double m_edgeFalloff;

            /// Lower bound of the selection range.
            double m_lowerBound;

            /// Upper bound of the selection range.
            double m_upperBound;
        };

    } // namespace module

} // namespace noise