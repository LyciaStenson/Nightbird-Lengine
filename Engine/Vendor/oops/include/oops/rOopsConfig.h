#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2021:
    Robot Control Software Ltd.
    Peter Barczikay
    Andrew Tantos

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/

/**
Configuration
*/

namespace rOops
{
    struct OopsConfig
    {
        enum class WriteTypeName {
            eNever,       ///< Do not save type name at all, e.g. in case of writing a pointer by value.
            eWhenNeeded,  ///< Save type only when needed for polymorphism.
            //eForCompound, ///< Save type name for compound types as well for self describing streams. This is needed for loading the stream to other languages e.g. Python.
            eAlways       ///< Always save type name.
        };
        /// Throw exception if property name is not found. Unknown property names are stored, logged and otherwise skipped by default.
        bool errorIfPropNameNotFound{ false };
        /// Control if typename is written even if it is not required for deserialization.
        WriteTypeName writeTypeNameLevel{ WriteTypeName::eWhenNeeded };
        /// Print YAML files in 'flow' format.
        bool flowStyleBlock{ false };
        bool flowStyleList{ false };
        /// Print log messages to the standard output.
        bool printLogMsgs{ true };
        /// Save address instead of saving the pointed object in-place. Add the object to list of the unsaved objects.
        bool saveAddress{ false };
    };
    /// Returns a reference to the config record.
    OopsConfig& globalConfig();
}
