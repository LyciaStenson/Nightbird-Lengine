#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2019:
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
Type Info Classes for Units library types.
meter, kg, newton, etc.
*/

#include <oops/rPropTypeTraits.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropTypeInfoBase.h>
#include <units.h>

namespace rOops
{
    template <typename UnitsT>
    std::string units_to_string(const UnitsT& arVal)
    {
        std::string str = std::to_string(arVal());
        str += arVal.abbreviation();
        return str;
    }

    /// Convert string with unit extension to units library type.
    template <typename UnitsT>
    UnitsT string_to_units(const std::string& arValue)
    {
        size_t idx = 0;
        typename UnitsT::value_type val = 0;
        try {
            val = string_to<typename UnitsT::value_type>(arValue, &idx);
        }
        catch(...) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Units library type value is wrong. String to value conversion failed: " << arValue));
        }
        UnitsT units_val(val);
        if (units_val.abbreviation() == arValue.substr(idx)) {
            return units_val;
        }
        // TODO: Try to convert from another measurement unit. e.g. mm to m.
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Different units. Expecting " << units_val.abbreviation() << ", but found " << arValue));
    }


    /// Type info template for units library types.
    template<typename UnitsT>
    class rPropTypeInfoUnits : public rPropTypeInfoBase<UnitsT>
    {
    public:
        explicit rPropTypeInfoUnits(const std::string& arTypeName)
            : rPropTypeInfoBase<UnitsT>(arTypeName)
        {}
        ~rPropTypeInfoUnits() override = default;

        std::string value(const void *apVal) const override
        {
            return units_to_string(*static_cast<const UnitsT*>(apVal));
        }
        void value(const void *apVal, std::string& arValue) const override
        {
            arValue = units_to_string(*static_cast<const UnitsT*>(apVal));
        }
        void setValue(void *apVal, const std::string& arValue) const override
        {
            *(static_cast<UnitsT*>(apVal)) = string_to_units<UnitsT>(arValue);
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            UnitsT value = *static_cast<const UnitsT*>(apVal);
            auto data = value();
            const char* abbrev = value.abbreviation();
            std::size_t abbrev_len = strlen(abbrev);
            if (127<abbrev_len) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Units abbreviation is too long: " << abbrev_len << " It is expected to be less than 128."));
            }
            if (0 == data) {
                arFormat.setSize(0);
                arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eValue));
            }
            else {
                arFormat.setSize(sizeof(data) + 1 + abbrev_len);
                arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eValue));
                arFormat.write(&data, sizeof(data));
                arFormat.writeSize(abbrev_len);
                arFormat.write(abbrev, abbrev_len);
            }
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName();
            arFormat.strm() << units_to_string(*static_cast<const UnitsT*>(apVal));
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName(false);
            arFormat.strm() << units_to_string(*static_cast<const UnitsT*>(apVal));
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            UnitsT& value = *static_cast<UnitsT*>(apVal);
            if (0==arCtx.size_) {
                value = UnitsT(0);
            }
            else {
                typename UnitsT::underlying_type underlying_data;
                arParser.read(underlying_data, arParser.doNotSwapInteger());
                std::size_t str_len = arParser.readSize();
                std::string abbrev_str;
                arParser.readString(str_len, abbrev_str);
                if (sizeof(underlying_data)+1+str_len != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Size error when loading units value."));
                }
                if (abbrev_str != value.abbreviation()) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Error loading units value, abbreviation mismatch: " << abbrev_str << " != " << value.abbreviation()));
                }
                value = UnitsT(underlying_data);
            }
        }
        void load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
        void load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
    }; //class rPropTypeInfoUnits

} //namespace rOops

rOOPS_DECLARE_TYPEINFO(units::length::meter_t)
rOOPS_DECLARE_TYPEINFO(units::length::centimeter_t)
