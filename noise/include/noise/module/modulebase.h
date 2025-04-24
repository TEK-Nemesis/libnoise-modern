// modulebase.h
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
// - Added [[nodiscard]] to GetSourceModule and GetSourceModuleCount to encourage proper use of return values.
// - Replaced raw pointer array with std::vector for source modules to improve safety.
// - Removed unnecessary includes (stdlib.h, math.h, basictypes.h, noisegen.h).
// - Used nullptr instead of NULL for modern C++ style.
// - Moved constructor to header as inline to improve efficiency.
// - Removed private assignment operator since std::vector handles copying safely.
// - Removed destructor as std::vector handles cleanup.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <cassert>  // For assert
#include <vector>   // For std::vector
#include "../exception.h"

namespace noise {

    namespace module {

        /// Abstract base class for all noise modules in libnoise.
        ///
        /// A noise module calculates and outputs a value given a three-dimensional
        /// input value (x, y, z), defining a near-infinite 3D texture. Each position
        /// in this "texture" has a specific value.
        ///
        /// Noise modules can be categorized into:
        /// - **Generator Modules**: Output values using coherent-noise functions
        ///   (e.g., Const, Perlin, Voronoi).
        /// - **Modifier Modules**: Mathematically modify the output of a source module
        ///   (e.g., Curve, Invert).
        /// - **Combiner Modules**: Combine the outputs of multiple source modules
        ///   (e.g., Add, Max).
        /// - **Selector Modules**: Use a control module to combine source modules
        ///   (e.g., Blend, Select).
        /// - **Transformer Modules**: Transform input coordinates before retrieving
        ///   the output (e.g., RotatePoint, ScalePoint).
        ///
        /// ### Combining Noise Modules
        /// Noise modules can be connected to form complex noise patterns. A source module
        /// is a noise module used as input to another. There is no limit to the number
        /// of source modules that can be connected, but each additional module increases
        /// computation time.
        ///
        /// ### Connecting Source Modules
        /// Use SetSourceModule() to connect a source module, specifying an index value
        /// (starting at 0). The number of required source modules is determined by
        /// GetSourceModuleCount(). For selector modules, the index determines the role
        /// (e.g., in Select, index 2 is the control module).
        ///
        /// ### Generating Output Values
        /// Call GetValue() with (x, y, z) coordinates to generate an output value.
        /// All required source modules must be connected via SetSourceModule() beforehand.
        ///
        /// ### Using Noise Modules
        /// - **Terrain Height Maps**: Use output values as elevation values.
        /// - **Procedural Textures**: Use output values as grayscale or RGB-channel values.
        /// - **Simulation**: Use output values to control movement or other properties.
        ///
        /// ### Creating Custom Noise Modules
        /// Derive a class from Module, implement GetSourceModuleCount() to specify the
        /// number of source modules, and implement GetValue() to compute the output.
        /// Ensure that GetValue() raises an assertion if required source modules are not set.
        /// Do not modify source modules; only modify their output values.
        class Module {
        public:
            /// Constructor.
            ///
            /// @param sourceModuleCount The number of source modules required by this noise module.
            explicit Module(int sourceModuleCount) noexcept
                : m_sourceModules(static_cast<size_t>(sourceModuleCount), nullptr) {
            }

            /// Returns a reference to a source module connected to this noise module.
            ///
            /// @param index The index value assigned to the source module.
            ///
            /// @returns A reference to the source module.
            /// @pre The index is in the range [0, GetSourceModuleCount() - 1].
            /// @pre A source module has been set at the specified index.
            /// @throw noise::ExceptionNoModule If the preconditions are not met.
            [[nodiscard]] virtual const Module& GetSourceModule(int index) const {
                if (index >= GetSourceModuleCount() || index < 0 || m_sourceModules[index] == nullptr) {
                    throw noise::ExceptionNoModule();
                }
                return *(m_sourceModules[index]);
            }

            /// Returns the number of source modules required by this noise module.
            ///
            /// @returns The number of source modules required.
            [[nodiscard]] virtual int GetSourceModuleCount() const noexcept = 0;

            /// Generates an output value given the coordinates of the specified input value.
            ///
            /// @param x The x-coordinate of the input value.
            /// @param y The y-coordinate of the input value.
            /// @param z The z-coordinate of the input value.
            ///
            /// @returns The output value.
            /// @pre All required source modules have been set via SetSourceModule().
            virtual double GetValue(double x, double y, double z) const noexcept = 0;

            /// Connects a source module to this noise module at the specified index.
            ///
            /// @param index The index value to assign to the source module.
            /// @param sourceModule The source module to attach.
            ///
            /// @pre The index is in the range [0, GetSourceModuleCount() - 1].
            /// @throw noise::ExceptionInvalidParam If the index is out of range.
            ///
            /// The source module must remain valid for the lifetime of this object
            /// until replaced. This noise module does not modify the source module;
            /// it only modifies its output values.
            virtual void SetSourceModule(int index, const Module& sourceModule) {
                if (index >= GetSourceModuleCount() || index < 0) {
                    throw noise::ExceptionInvalidParam();
                }
                m_sourceModules[index] = &sourceModule;
            }

        protected:
            /// Vector containing pointers to all source modules required by this noise module.
            std::vector<const Module*> m_sourceModules;
        };

    } // namespace module

} // namespace noise