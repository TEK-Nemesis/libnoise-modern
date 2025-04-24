// exception.h
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
// Updated for C++17 compatibility by TEK Nemesis and Grok on April 30, 2025:
// - Replaced include guard with #pragma once.
// - Derived exception classes from std::exception for standard compatibility.
// - Added virtual destructor to base Exception class.
// - Implemented what() to provide descriptive error messages.
// - Improved documentation with consistent formatting.
// - Removed redundant Doxygen group tags.

#pragma once

#include <exception> // For std::exception

namespace noise {

    /// Base class for all libnoise exceptions.
    class Exception : public std::exception {
    public:
        Exception() = default;
        virtual ~Exception() = default;

        /// Returns a description of the exception.
        /// @returns A C-string describing the exception.
        const char* what() const noexcept override {
            return "An unspecified error occurred in libnoise";
        }
    };

    /// Exception thrown when an invalid parameter is passed to a libnoise function or method.
    class ExceptionInvalidParam : public Exception {
    public:
        const char* what() const noexcept override {
            return "Invalid parameter passed to a libnoise function or method";
        }
    };

    /// Exception thrown when a required source module cannot be retrieved from a noise module.
    ///
    /// @note If a required source module is missing and GetValue() is called, a debug assertion
    /// may be raised instead of this exception for performance reasons.
    class ExceptionNoModule : public Exception {
    public:
        const char* what() const noexcept override {
            return "Could not retrieve a required source module from a noise module";
        }
    };

    /// Exception thrown when there is not enough memory to perform an action.
    class ExceptionOutOfMemory : public Exception {
    public:
        const char* what() const noexcept override {
            return "Not enough memory to perform the requested action in libnoise";
        }
    };

    /// Exception thrown for an unknown error in libnoise.
    class ExceptionUnknown : public Exception {
    public:
        const char* what() const noexcept override {
            return "An unknown error occurred in libnoise";
        }
    };

} // namespace noise