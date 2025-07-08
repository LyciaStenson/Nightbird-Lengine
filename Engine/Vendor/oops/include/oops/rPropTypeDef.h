#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2022:
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
Common type definitions.
*/

#include <oops/rPropLog.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>


namespace rOops
{
    using UnknownPropertyTable_t = std::unordered_map<size_t, std::string>;
    using PreprocessorDefines_t = std::unordered_map<std::string, std::string>;

    using TypeIdHashType = uint64_t;
    using NameIdHashType = uint64_t;
    constexpr uint64_t cInvalidHash = 0u;

    using istream_type = std::istream;  ///< Input stream for loading the properties from. Could be template param.
    using ostream_type = std::ostream;  ///< Output stream for saving the properties to. Could be template param.
    using size_type = std::size_t;
    using offset_type = ::ptrdiff_t;    ///< Integer for storing the result of pointer subtraction.
    using address_type = ::uintptr_t;   ///< Unsigned integer for storing an address.
    using ptr_type = char*;             ///< Pointer type used for pointer arithmetic.
    using const_ptr_type = const char*; ///< Pointer type used for pointer arithmetic.
    using flags_type = uint32_t;
    static_assert(sizeof(void*)<=sizeof(address_type), "'address_type' must be able to store an address.");
    static_assert(sizeof(void*)<=sizeof(offset_type), "'offset_type' must be able to store an address.");
    static_assert(sizeof(address_type)==sizeof(offset_type), "Size of 'address_type' and 'offset_type' must be the same.");

    /// Get the offset of the sub-class.
    template< class ClassT, class BaseT >
    rOops::offset_type getBaseClassOffset()
    {
        // nullptr cannot be cast to base class pointer. It remains nullptr.
        const auto* pClass = reinterpret_cast<const ClassT* const>(alignof(ClassT));
        const auto* pBase = static_cast<const BaseT* const>(pClass);
        auto base_addr = reinterpret_cast<::rOops::address_type>(pBase);
        auto derived_addr = reinterpret_cast<::rOops::address_type>(pClass);
        if (base_addr >= derived_addr) {
            return static_cast<::rOops::offset_type>(base_addr - derived_addr);
        }
        else {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Base class address is less than the derived class address. This should never happen."));
        }
    }

    template<typename EnumT>
    constexpr auto enum2integral(EnumT enum_value) -> typename std::underlying_type<EnumT>::type
    {
        return static_cast<typename std::underlying_type<EnumT>::type>(enum_value);
    }

    inline TypeIdHashType generateTypeId(const std::string& arTypeName)
    {
        if (arTypeName.empty()) {
            throw std::runtime_error("Empty string is not supported");
        }
        return static_cast<TypeIdHashType>(std::hash<std::string>{}(arTypeName));
    }

    inline NameIdHashType generateNameId(const std::string& arName)
    {
        if (arName.empty()) {
            return cInvalidHash;
        }
        return static_cast<NameIdHashType>(std::hash<std::string>{}(arName));
    }

    inline bool isIntegerLittleEndian()
    {
        volatile std::uint32_t ii = 0x01234567;
        return (*reinterpret_cast<volatile std::uint8_t*>(&ii) == 0x67);
    }

    inline bool isFloatLittleEndian()
    {
        volatile float ff = -0.0f;
        return (reinterpret_cast<volatile std::uint8_t*>(&ff)[3] == 0x80);
    }

    inline bool isDoubleLittleEndian()
    {
        volatile double dd = -0.0;
        return (reinterpret_cast<volatile std::uint8_t*>(&dd)[7] == 0x80);
    }
} //namespace rOops
