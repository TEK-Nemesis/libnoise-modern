// clamp.h
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
// - Moved GetSourceModuleCount and GetValue to header as inline to improve efficiency.
// - Used std::clamp for modern C++ style.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Updated m_sourceModules to m_sourceModules to match the base class.
// - Fixed std::clamp call to use correct arguments.

#pragma once

#include <algorithm>  // For std::clamp
#include <cassert>    // For assert
#include "modulebase.h"

namespace noise {

namespace module {

    /// Default lower bound of the clamping range for the Clamp noise module.
    inline constexpr double DEFAULT_CLAMP_LOWER_BOUND = -1.0;

    /// Default upper bound of the clamping range for the Clamp noise module.
    inline constexpr double DEFAULT_CLAMP_UPPER_BOUND = 1.0;

    /// Noise module that clamps the output value from a source module to a specified range.
    ///
    /// @image html moduleclamp.png
    ///
    /// The GetValue() method clamps the output value from the source module to the
    /// range defined by the lower and upper bounds, set via SetBounds().
    ///
    /// This noise module requires one source module.
    class Clamp : public Module {
    public:
        /// Constructor.
        Clamp() noexcept
            : Module(GetSourceModuleCount()),
              m_lowerBound(DEFAULT_CLAMP_LOWER_BOUND),
              m_upperBound(DEFAULT_CLAMP_UPPER_BOUND) {}

        /// Returns the number of source modules required by this noise module.
        ///
        /// @returns Always 1, as this module requires one source module.
        inline int GetSourceModuleCount() const noexcept override {
            return 1;
        }

        /// Returns the clamped output value from the source module.
        ///
        /// @param x The x-coordinate of the input value.
        /// @param y The y-coordinate of the input value.
        /// @param z The z-coordinate of the input value.
        ///
        /// @returns The clamped output value.
        /// @pre The source module (index 0) has been set.
        inline double GetValue(double x, double y, double z) const noexcept override {
            assert(m_sourceModules[0] != nullptr && "Source module must be set before calling GetValue");
            return std::clamp(m_sourceModules[0]->GetValue(x, y, z), m_lowerBound, m_upperBound);
        }

        /// Returns the lower bound of the clamping range.
        ///
        /// @returns The lower bound.
        [[nodiscard]] inline double GetLowerBound() const noexcept {
            return m_lowerBound;
        }

        /// Returns the upper bound of the clamping range.
        ///
        /// @returns The upper bound.
        [[nodiscard]] inline double GetUpperBound() const noexcept {
            return m_upperBound;
        }

        /// Sets the lower and upper bounds of the clamping range.
        ///
        /// @param lowerBound The lower bound.
        /// @param upperBound The upper bound.
        ///
        /// @throw noise::ExceptionInvalidParam If lowerBound exceeds upperBound.
        inline void SetBounds(double lowerBound, double upperBound) {
            if (lowerBound > upperBound) {
                throw noise::ExceptionInvalidParam();
            }
            m_lowerBound = lowerBound;
            m_upperBound = upperBound;
        }

    protected:
        /// Lower bound of the clamping range.
        double m_lowerBound;

        /// Upper bound of the clamping range.
        double m_upperBound;
    };

} // namespace module

} // namespace noise