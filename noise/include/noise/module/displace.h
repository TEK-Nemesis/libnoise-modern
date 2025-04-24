// displace.h
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
// - Moved GetSourceModuleCount to header as inline to improve efficiency.
// - Used nullptr instead of NULL for modern C++ style.
// - Used member initializer list in constructor for clarity.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.
// - Updated m_pSourceModule to m_sourceModules to match the base class.
// - Fixed null checks to use m_sourceModules[index] instead of comparing vector with nullptr.

#pragma once

#include <cassert>  // For assert
#include "modulebase.h"

namespace noise {

    namespace module {

        /// Noise module that uses three source modules to displace the input coordinates
        /// before returning the output value from a fourth source module.
        ///
        /// @image html moduledisplace.png
        ///
        /// This noise module requires four source modules:
        /// - Source module 0 (index 0): The source module to displace.
        /// - Source module 1 (index 1): The x displacement module.
        /// - Source module 2 (index 2): The y displacement module.
        /// - Source module 3 (index 3): The z displacement module.
        ///
        /// The output is computed by displacing the input coordinates (x, y, z) by the
        /// output values of the displacement modules before querying the source module.
        class Displace : public Module {
        public:
            /// Constructor.
            Displace() noexcept : Module(GetSourceModuleCount()) {}

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns Always 4, as this module requires four source modules.
            inline int GetSourceModuleCount() const noexcept override {
                return 4;
            }

            /// Returns the displaced output value from the source module.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The displaced output value.
            /// @pre All source modules (indices 0, 1, 2, and 3) have been set.
            inline double GetValue(double x, double y, double z) const noexcept override {
                assert(m_sourceModules[0] != nullptr && "Source module 0 must be set before calling GetValue");
                assert(m_sourceModules[1] != nullptr && "X displace module (source module 1) must be set before calling GetValue");
                assert(m_sourceModules[2] != nullptr && "Y displace module (source module 2) must be set before calling GetValue");
                assert(m_sourceModules[3] != nullptr && "Z displace module (source module 3) must be set before calling GetValue");

                const double xDisplace = m_sourceModules[1]->GetValue(x, y, z);
                const double yDisplace = m_sourceModules[2]->GetValue(x, y, z);
                const double zDisplace = m_sourceModules[3]->GetValue(x, y, z);
                return m_sourceModules[0]->GetValue(x + xDisplace, y + yDisplace, z + zDisplace);
            }

            /// Sets the x displacement module.
            ///
            /// @param xDisplaceModule The x displacement module.
            ///
            /// The module must remain valid for the lifetime of this object until replaced.
            inline void SetXDisplaceModule(const Module& xDisplaceModule) noexcept {
                assert(m_sourceModules.size() >= 4);
                m_sourceModules[1] = &xDisplaceModule;
            }

            /// Sets the y displacement module.
            ///
            /// @param yDisplaceModule The y displacement module.
            ///
            /// The module must remain valid for the lifetime of this object until replaced.
            inline void SetYDisplaceModule(const Module& yDisplaceModule) noexcept {
                assert(m_sourceModules.size() >= 4);
                m_sourceModules[2] = &yDisplaceModule;
            }

            /// Sets the z displacement module.
            ///
            /// @param zDisplaceModule The z displacement module.
            ///
            /// The module must remain valid for the lifetime of this object until replaced.
            inline void SetZDisplaceModule(const Module& zDisplaceModule) noexcept {
                assert(m_sourceModules.size() >= 4);
                m_sourceModules[3] = &zDisplaceModule;
            }

            /// Sets all displacement modules.
            ///
            /// @param xDisplaceModule The x displacement module.
            /// @param yDisplaceModule The y displacement module.
            /// @param zDisplaceModule The z displacement module.
            ///
            /// The modules must remain valid for the lifetime of this object until replaced.
            inline void SetDisplaceModules(const Module& xDisplaceModule,
                const Module& yDisplaceModule,
                const Module& zDisplaceModule) noexcept {
                assert(m_sourceModules.size() >= 4);
                m_sourceModules[1] = &xDisplaceModule;
                m_sourceModules[2] = &yDisplaceModule;
                m_sourceModules[3] = &zDisplaceModule;
            }
        };

    } // namespace module

} // namespace noise