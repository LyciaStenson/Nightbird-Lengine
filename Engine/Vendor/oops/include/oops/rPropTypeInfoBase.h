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
Base types Type Info Classes.
string, char, bool, chrono
*/


#include <oops/rPropDescriptor.h>
#include <oops/rPropIter.h>
#include <oops/rPropIter_impl.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropTypeTraits.h>
#include <oops/rOopsBinaryFormat.h>
#include <oops/rOopsTextFormat.h>
#include <oops/rOopsYamlFormat.h>
#include <oops/rOopsBinaryParser.h>
#include <oops/rOopsTextParser.h>
#include <oops/rOopsYamlParser.h>
#include <string>
#include <chrono>
#include <cmath> // for fabs
#include <cctype>
#include <cstring>

namespace rOops
{

    /// Common base class collecting member functions having a common implementation.
    template< typename BaseT >
    class rPropTypeInfoBase : public rPropTypeInfo
    {
    public:
        explicit rPropTypeInfoBase( const std::string& arTypeName )
            : rPropTypeInfo(arTypeName)
            {}
        ~rPropTypeInfoBase() override = default;
        bool isValue() const override
            { return true; }
        size_type Size() const override { return sizeof(BaseT); }
        /// Create single object with 'new'.
        void* createObj() const override
        {
            return new BaseT();
        }
        /// Create an object with placement new.
        void *createObj( void *apData ) const override
        {
            return new(apData) BaseT();
        }
        /// Create array of objects with 'new[aN]'.
        void* createObj( size_type aN ) const override
        {
            return new BaseT[aN];
        }
        /// Destroy object allocated with new.
        void destroyObj( void* apObj ) const override
        {
            delete static_cast<BaseT*>(apObj);
        }
        /// Destroy object allocated with placement new.
        void destroyObj( void* apObj, void* /*apData*/ ) const override
        {
            static_cast<BaseT*>(apObj)->~BaseT();
        }
        /// Destroy array of objects with 'delete[]'.
        void destroyObj( void* apObjArr, size_type /*aN*/ ) const override
            { delete[] (static_cast<BaseT*>(apObjArr)); }
        rPropIterator begin( void* apVal, const std::string& arName ) const override
        {
            return rPropIterator( std::make_unique<rPropIteratorValue>(this,apVal,arName) );
        }
        rPropConstIterator begin( const void* apVal, const std::string& arName) const override
        {
            return rPropConstIterator( std::make_unique<rPropConstIterValue>(this,apVal,arName) );
        }
    }; //rPropTypeInfoBase


    /// Type info class for base types (e.g. int, unsigned int).
    template< typename BaseT >
    class rPropTypeInfoBaseType : public rPropTypeInfoBase<BaseT>
    {
    public:
        explicit rPropTypeInfoBaseType( const std::string& arTypeName )
            : rPropTypeInfoBase<BaseT>(arTypeName)
        {}
        ~rPropTypeInfoBaseType() override = default;

        std::string value( const void *apVariable ) const override
        {
            return std::to_string( *(static_cast<const BaseT*>(apVariable)) );
        }
        void value( const void *apVariable, std::string& arValue ) const override
        {
            arValue = std::to_string( *(static_cast<const BaseT*>(apVariable)) );
        }
        void setValue( void *apVariable, const std::string& arValue ) const override
        {
            *(static_cast<BaseT*>(apVariable)) = string_to<BaseT>(arValue);
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            if (BaseT(0) == *static_cast<const BaseT*>(apVal)) {
                arFormat.setSize(0);
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<BaseT>()));
            }
            else {
                arFormat.setSize(sizeof(BaseT));
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<BaseT>()));
                arFormat.write(apVal, sizeof(BaseT));
            }
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName();
            arFormat.strm() << std::to_string(*(static_cast<const BaseT*>(apVal)));
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName(false);
            arFormat.strm() << std::to_string(*(static_cast<const BaseT*>(apVal)));
        }

        void load(rOopsBinaryParser& arParser, void *apVal, rOopsParserContext& arCtx) const override
        {
            if (0==arCtx.size_) {
                // null value
                memset(apVal, 0, this->Size());
            }
            else {
                if (sizeof(BaseT) != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
                }
                arParser.read(apVal, arCtx.size_, arParser.doNotSwapInteger());
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
    }; //rPropTypeInfoBaseType

    class rPropTypeInfoFloat : public rPropTypeInfoBaseType<float>
    {
    public:
        explicit rPropTypeInfoFloat( const std::string& arTypeName )
            : rPropTypeInfoBaseType<float>(arTypeName)
            {}
        ~rPropTypeInfoFloat() override = default;
        std::string value( const void *apVariable ) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                snprintf(buf, buf_size, "%.10g", *(static_cast<const float*>(apVariable)));
                return {buf};
            }
        void value( const void *apVariable, std::string& arValue ) const override
        {
            arValue = value(apVariable);
        }

        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName();
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.10g", *(static_cast<const float*>(apVal)));
                arFormat.strm().write(buf, n);
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName(false);
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.10g", *(static_cast<const float*>(apVal)));
                arFormat.strm().write(buf, n);
            }
        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            if (0 == arCtx.size_) {
                // null value
                memset(apVal, 0, this->Size());
            }
            else {
                if (sizeof(float) != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
                }
                arParser.read(apVal, arCtx.size_, arParser.doNotSwapFloat());
            }
        }
    };

    class rPropTypeInfoDouble : public rPropTypeInfoBaseType<double>
    {
    public:
        explicit rPropTypeInfoDouble( const std::string& arTypeName )
            : rPropTypeInfoBaseType<double>(arTypeName)
            {}
        ~rPropTypeInfoDouble() override = default;
        std::string value( const void *apVariable ) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                snprintf(buf, buf_size, "%.18g", *(static_cast<const double*>(apVariable)));
                return {buf};
            }
        void value( const void *apVariable, std::string& arValue ) const override
        {
            arValue = value(apVariable);
        }

        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName();
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.18g", *(static_cast<const double*>(apVal)));
                arFormat.strm().write(buf, n);
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName(false);
                const int buf_size = 63;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.18g", *(static_cast<const double*>(apVal)));
                arFormat.strm().write(buf, n);
            }
        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            if (0 == arCtx.size_) {
                // null value
                memset(apVal, 0, this->Size());
            }
            else {
                if (sizeof(double) != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
                }
                arParser.read(apVal, arCtx.size_, arParser.doNotSwapFloat());
            }
        }
    };

    class rPropTypeInfoLongDouble : public rPropTypeInfoBaseType<long double>
    {
    public:
        explicit rPropTypeInfoLongDouble( const std::string& arTypeName )
            : rPropTypeInfoBaseType<long double>(arTypeName)
            {}
        ~rPropTypeInfoLongDouble() override = default;

        std::string value( const void *apVariable ) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                const int buf_size = 127;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                snprintf(buf, buf_size, "%.22Lg", *(static_cast<const long double*>(apVariable)));
                return {buf};
            }
        void value( const void *apVariable, std::string& arValue ) const override
        {
            arValue = value(apVariable);
        }

        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName();
                const int buf_size = 127;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.22Lg", *(static_cast<const long double*>(apVal)));
                arFormat.strm().write(buf, n);
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                // rPropTypeInfoBaseType cannot handle precision.
                arFormat.writeSizeTypeAndName(false);
                const int buf_size = 127;
                char buf[buf_size + 1];
                buf[buf_size] = '\0';
                auto n = snprintf(buf, buf_size, "%.22Lg", *(static_cast<const long double*>(apVal)));
                arFormat.strm().write(buf, n);
            }
    };

    template< typename CharT >
    class rPropTypeInfoChar : public rPropTypeInfoBase<CharT>
    {
        const char cDelimiterChar = '\'';
    public:
        explicit rPropTypeInfoChar( const std::string& arTypeName )
            : rPropTypeInfoBase<CharT>(arTypeName)
            {}
        ~rPropTypeInfoChar() override = default;
        char getOpeningCharacter() const override
            { return cDelimiterChar; }
        char getClosingCharacter() const override
            { return cDelimiterChar; }
        std::string value( const void *apVal ) const override
            { return std::string(1, *((const CharT*)apVal)); }
        void value( const void *apVal, std::string& arValue ) const override
            { arValue = *((const CharT*)apVal); }
        void setValue( void *apVal, const std::string& arValue ) const override
            { *((CharT*)apVal) = static_cast<CharT>((0 < arValue.length()) ? arValue[0] : '\0'); }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            if (CharT(0) == *static_cast<const CharT*>(apVal)) {
                arFormat.setSize(0);
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<CharT>()));
            }
            else {
                arFormat.setSize(sizeof(CharT));
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<CharT>()));
                arFormat.write(apVal, sizeof(CharT));
            }
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName();
                arFormat.writeQuoted(*static_cast<const CharT*>(apVal), cDelimiterChar);
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName(false);
                arFormat.writeQuoted(*static_cast<const CharT*>(apVal), cDelimiterChar);
            }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            CharT& chr = *static_cast<CharT*>(apVal);
            if (0==arCtx.size_) {
                chr = CharT(0);
            }
            else {
                if (this->Size() != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
                }
                arParser.read(apVal, arCtx.size_, arParser.doNotSwapInteger());
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
    }; //class rPropTypeInfoChar


    template< typename CharT >
    class rPropTypeInfoCharAsInt : public rPropTypeInfoBase<CharT>
    {
    public:
        explicit rPropTypeInfoCharAsInt( const std::string& arTypeName )
            : rPropTypeInfoBase<CharT>(arTypeName)
            {}
        ~rPropTypeInfoCharAsInt() override = default;

        std::string value( const void *apVal ) const override
            { unsigned short temp = *((CharT*)apVal); return std::to_string( temp ); }
        void value( const void *apVal, std::string& arValue ) const override
            { unsigned short temp = *((CharT*)apVal); arValue = std::to_string( temp ); }
        void setValue( void *apVal, const std::string& arValue ) const override
            { *((CharT*)apVal) = static_cast<CharT>(std::stoul(arValue)); }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            if (CharT(0) == *static_cast<const CharT*>(apVal)) {
                arFormat.setSize(0);
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<CharT>()));
            }
            else {
                arFormat.setSize(sizeof(CharT));
                arFormat.writeCtrlSizeTypeName(enum2integral(type2ControlByte<CharT>()));
                arFormat.write(apVal, sizeof(CharT));
            }
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName();
                unsigned short temp = *static_cast<const CharT*>(apVal);
                arFormat.strm() << std::to_string(temp);
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName(false);
                unsigned short temp = *static_cast<const CharT*>(apVal);
                arFormat.strm() << std::to_string(temp);
            }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            CharT& chr = *static_cast<CharT*>(apVal);
            if (0==arCtx.size_) {
                chr = CharT(0);
            }
            else {
                if (this->Size() != arCtx.size_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
                }
                arParser.read(apVal, arCtx.size_, arParser.doNotSwapInteger());
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
    }; //class rPropTypeInfoCharAsInt


    /// Type info class of bool for handling 0/1 and false/true as well.
    class rPropTypeInfoBool : public rPropTypeInfoBase<bool>
    {
    public:
        explicit rPropTypeInfoBool(const std::string& arTypeName);
        ~rPropTypeInfoBool() override;

        std::string value(const void *apVal) const override;
        void value(const void *apVal, std::string& arValue) const override;
        void setValue(void *apVal, const std::string& arValue) const override;

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override;
        void save(rOopsTextFormat& arFormat, const void *apVal) const override;
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override;

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
    }; //class rPropTypeInfoBool


    /// Type info class of std::string.
    class rPropTypeInfoStdString : public rPropTypeInfoBase<std::string>
    {
        const char cDelimiterChar = '\"';
    public:
        explicit rPropTypeInfoStdString( const std::string& arTypeName );
        ~rPropTypeInfoStdString() override;
        char getOpeningCharacter() const override
            { return cDelimiterChar; }
        char getClosingCharacter() const override
            { return cDelimiterChar; }

        std::string value( const void *apVal ) const override;
        void value( const void *apVal, std::string& arValue ) const override;
        void setValue( void *apVal, const std::string& arValue ) const override;

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override;
        void save(rOopsTextFormat& arFormat, const void *apVal) const override;
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override;

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
    }; //class rPropTypeInfoStdString


    class rPropTypeInfoRunTimeType : public rPropTypeInfoStdString
    {
    public:
        explicit rPropTypeInfoRunTimeType( const std::string& arTypeName )
            : rPropTypeInfoStdString("RunTimeType")
        {
            // Calling the base class constructor with empty type for not register this type info.
            typeName_ = arTypeName;
        }
    };


    /// Type info template for enumeration types having enum to string conversion object.
    /**
    EnumStrConvT must have 2 function call operators for converting enum to string and string to enum.
    */
    template< typename EnumT, typename EnumStrConvT >
    class rPropTypeInfoEnum : public rPropTypeInfoBase<EnumT>
    {
    public:
        explicit rPropTypeInfoEnum( const std::string& arTypeName )
            : rPropTypeInfoBase<EnumT>(arTypeName)
            {}
        ~rPropTypeInfoEnum() override = default;

        std::string value( const void *apVal ) const override { return enumStrConv_( *((EnumT*)apVal) ); }
        void value( const void *apVal, std::string& arValue ) const override { arValue = enumStrConv_( *((EnumT*)apVal) ); }
        void setValue( void *apVal, const std::string& arValue ) const override
        {
            *((EnumT*)apVal) = enumStrConv_( arValue );
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
            {
                arFormat.setSize(sizeof(EnumT));
                arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eEnum));
                arFormat.write(apVal, sizeof(EnumT));
            }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName();
                arFormat.strm() << enumStrConv_( *((EnumT*)apVal) );
            }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
            {
                arFormat.writeSizeTypeAndName(false);
                arFormat.strm() << enumStrConv_( *((EnumT*)apVal) );
            }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            if (sizeof(EnumT) != arCtx.size_) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object size and the size of the value on the stream are different: " << this->Size() << " != " << arCtx.size_));
            }
            arParser.read(apVal, arCtx.size_, arParser.doNotSwapInteger());
        }
        void load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
        void load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
    protected:
        EnumStrConvT enumStrConv_; /// Conversion tool for enum to string and string to enum conversions.
    }; //class rPropTypeInfoEnum

    /// Convert string with unit extension to chrono::duration.
    template <typename DurationT>
    DurationT string_to_duration(const std::string& arValue)
    {
        static const int powOf10[] = {1, 10, 100, 1000};
        if (arValue.empty()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Time value is empty."));
        }
        size_t idx = 0;
        typename DurationT::rep tv = 0;
        try {
            tv = string_to<typename DurationT::rep>(arValue, &idx);
        }
        catch(...) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Time value is wrong. String to int conversion failed: " << arValue));
        }
        auto len = arValue.length() - idx;
        if (':'==arValue[idx]) {
            // HH:MM:SS.mmm
            auto col_pos = arValue.find(':', idx+1); // find the 2nd colon.
            auto dot_pos = arValue.find('.', idx+1);
            int tvh=0, tvm=0, tvs=0;
            if (std::string::npos != dot_pos) {
                // There are a fractional part at the end: xx:yy.mmm
                int tvms = 0;
                size_t fraction_idx = 0;
                try {
                    tvms = std::stoi(arValue.substr(dot_pos+1), &fraction_idx);
                }
                catch(...) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Time value is wrong. String to int conversion for the fractional part failed: " << arValue));
                }
                // Check the length of the fractional part.
                if ( 3 < fraction_idx ) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Fraction part is longer than 3 digits: " << arValue));
                }
                if (0 < tvms) {
                    tvms *= powOf10[3-static_cast<int>(fraction_idx)];
                }
                // Check if there are 1 or 2 ':' character?
                if (std::string::npos != col_pos) {
                    // HH:MM:SS.mmm
                    if (3 != sscanf(&arValue[0], "%d:%d:%d", &tvh, &tvm, &tvs)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value: " << arValue));
                    }
                    tv = 60*tvh + tvm;
                    tv = 60*tv + tvs;
                    tv = 1000*tv + tvms;
                    return std::chrono::duration_cast<DurationT>(std::chrono::milliseconds(tv));
                }
                else {
                    // MM:SS.mmm
                    if (2 != sscanf(&arValue[0], "%d:%d", &tvm, &tvs)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value: " << arValue));
                    }
                    tv = 60*tvm + tvs;
                    tv = 1000*tv + tvms;
                    return std::chrono::duration_cast<DurationT>(std::chrono::milliseconds(tv));
                }
            }
            else {
                // No fractional part. Check if there are 1 or 2 ':' character?
                if (std::string::npos != col_pos) {
                    // HH:MM:SS
                    if (3 != sscanf(&arValue[0], "%d:%d:%d", &tvh, &tvm, &tvs)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value: " << arValue));
                    }
                    return std::chrono::duration_cast<DurationT>(std::chrono::seconds(60*(60*tvh+tvm)+tvs));
                }
                else {
                    // HH:MM
                    if (2 != sscanf(&arValue[0], "%d:%d", &tvh, &tvm)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value: " << arValue));
                    }
                    return std::chrono::duration_cast<DurationT>(std::chrono::minutes(60*tvh+tvm));
                }
            }
        }
        else {
            int tf = 0;
            if ( (0 < len) && ('.' == arValue[idx]) ) {
                // 1.2s
                size_t fraction_idx = 0;
                try {
                    tf = std::stoi(arValue.substr(idx+1), &fraction_idx);
                }
                catch(...) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Time value is wrong. String to int conversion for the fractional part failed: " << arValue));
                }
                // Check the length of the fractional part.
                if ( 3 < fraction_idx ) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Fraction part is longer than 3 digits: " << arValue));
                }
                if (0<tf) {
                    tf *= powOf10[3-static_cast<int>(fraction_idx)];
                }
                idx += fraction_idx+1;
                len = arValue.length() - idx;
            }
            switch (len) {
                case 0:
                    // No measurement unit.
                    return DurationT(tv);
                case 1:
                    switch (arValue[idx]) {
                        case 's':
                            // seconds: 1s, 1.1s, 1.321s
                            if (0 != tf) {
                                return std::chrono::duration_cast<DurationT>(std::chrono::milliseconds(1000*tv+tf));
                            }
                            else {
                                return std::chrono::duration_cast<DurationT>(std::chrono::duration<typename DurationT::rep>(tv)); // seconds
                            }
                        case 'h':
                            if (0 != tf) {
                                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Hours does not support fractional part: " << arValue));
                            }
                            return std::chrono::duration_cast<DurationT>(std::chrono::hours(tv));   // 3h - hours
                        default:
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time unit: " << arValue));
                    } //switch
                case 2:
                    if ('s'!=arValue[idx+1]) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time unit: " << arValue));
                    }
                    switch (arValue[idx]) {
                        case 'm':
                            // ms - milliseconds
                            if (0 != tf) {
                                // 98.76ms
                                return std::chrono::duration_cast<DurationT>(std::chrono::microseconds(1000*tv+tf));
                            }
                            else {
                                // 67ms
                                return std::chrono::duration_cast<DurationT>(std::chrono::milliseconds(tv));
                            }
                        case 'u':
                            // us - microseconds
                            if (0 != tf) {
                                // 1.23us
                                return std::chrono::duration_cast<DurationT>(std::chrono::nanoseconds(1000*tv+tf));
                            }
                            else {
                                // 45us
                                return std::chrono::duration_cast<DurationT>(std::chrono::microseconds(tv));
                            }
                        case 'n':
                            // ns - nanoseconds
                            if (0!=tf) {
                                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Nanosecond cannot have fractional part: " << arValue));
                            }
                            return std::chrono::duration_cast<DurationT>(std::chrono::nanoseconds(tv));
                        default:
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time unit: " << arValue));
                    } //switch
                case 3:
                    // minutes: 2min invalid: 2.1min 2.123min
                    if ('m'==arValue[idx] && 'i'==arValue[idx+1] && 'n'==arValue[idx+2]) {
                        if (0 != tf) {
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Minutes does not support fractional part: " << arValue));
                        }
                        return std::chrono::duration_cast<DurationT>(std::chrono::minutes(tv)); // minutes
                    }
                    else if ('s'==arValue[idx] && 'e'==arValue[idx+1] && 'c'==arValue[idx+2]) {
                        // seconds: 1sec, 1.1sec, 1.321sec
                        if (0 != tf) {
                            return std::chrono::duration_cast<DurationT>(std::chrono::milliseconds(1000*tv+tf));
                        }
                        else {
                            return std::chrono::duration_cast<DurationT>(std::chrono::duration<typename DurationT::rep>(tv)); // seconds
                        }
                    }
                    else if ('d'==arValue[idx] && 'a'==arValue[idx+1] && 'y'==arValue[idx+2]) {
                        // seconds: 1day, 26day
                        if (0 != tf) {
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time value. Days does not support fractional part: " << arValue));
                        }
                        return std::chrono::duration_cast<DurationT>(std::chrono::hours(24*tv)); // hours
                    }
                    else {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time unit: " << arValue));
                    }
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid time unit. It is longer than 3 characters: " << arValue));
            } //switch (len)
        }
    } //string_to_duration()

    template <typename ValueT>
    const char* findTimeUnit(ValueT& arVal)
    {
        const ValueT c1000 = 1000;
        const ValueT c60 = 60;
        const ValueT c24 = 24;
        // This is probably more readable than using cycles.
        if (0 != arVal % c1000) return "ns";
        arVal /= c1000;
        if (0 != arVal % c1000) return "us";
        arVal /= c1000;
        if (0 != arVal % c1000) return "ms";
        arVal /= c1000;
        if (0 != arVal % c60) return "s";
        arVal /= c60;
        if (0 != arVal % c60) return "min";
        arVal /= c60;
        if (0 != arVal % c24) return "h";
        arVal /= c24;
        return "day";
    }

    void readTimeAndUnits(istream_type& arStream, std::string& arString);

    template <typename DurationT>
    std::string duration_to_string(const DurationT& arVal)
    {
        if (0 == arVal.count()) {
            DurationT d1{ 1 };
            auto val = std::chrono::duration_cast<std::chrono::nanoseconds>(d1).count();
            return std::string("0") + findTimeUnit(val);
        }
        else {
            // Convert duration to nanoseconds.
            auto val = std::chrono::duration_cast<std::chrono::nanoseconds>(arVal).count();
            auto time_unit = findTimeUnit(val);
            return std::to_string(val) + time_unit;
        }
    }

    template<typename DurationT>
    inline void saveDurationValue(rOopsBinaryFormat& arFormat, const DurationT aDurationValue)
    {
        if (0 == aDurationValue.count()) {
            arFormat.setSize(0);
            arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eValue));
        }
        else {
            arFormat.setSize(sizeof(typename DurationT::rep) + sizeof(DurationT::period::num) + sizeof(DurationT::period::den));
            arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eValue));
            typename DurationT::rep duration_count = aDurationValue.count();
            arFormat.write(&duration_count, sizeof(duration_count));
            arFormat.write(&DurationT::period::num, sizeof(DurationT::period::num));
            arFormat.write(&DurationT::period::den, sizeof(DurationT::period::den));
        }
    }

    template<typename DurationT>
    inline void loadDurationValue(rOopsBinaryParser& arParser, DurationT& aDurationValue, rOopsParserContext& arCtx)
    {
        if (0==arCtx.size_) {
            aDurationValue = DurationT(0);
        }
        else {
            typename DurationT::rep duration_count;
            intmax_t numerator;
            intmax_t denominator;
            if (sizeof(duration_count)+sizeof(numerator)+sizeof(denominator) != arCtx.size_) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Size error when loading std::duration."));
            }
            arParser.read(duration_count, arParser.doNotSwapInteger());
            arParser.read(numerator, arParser.doNotSwapInteger());
            arParser.read(denominator, arParser.doNotSwapInteger());
            if (DurationT::period::num != numerator) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Error loading chrono::duration: numerator mismatch."));
            if (DurationT::period::den != denominator) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Error loading chrono::duration: denominator mismatch."));
            aDurationValue = DurationT(duration_count);
        }
    }

    /// Type info template for chrono duration types.
    template<typename DurationT>
    class rPropTypeInfoStdChronoDuration : public rPropTypeInfoBase<DurationT>
    {
    public:
        explicit rPropTypeInfoStdChronoDuration(const std::string& arTypeName)
            : rPropTypeInfoBase<DurationT>(arTypeName)
        {}
        ~rPropTypeInfoStdChronoDuration() override = default;

        std::string value(const void *apVal) const override
        {
            return duration_to_string(*static_cast<const DurationT*>(apVal));
        }
        void value(const void *apVal, std::string& arValue) const override
        {
            arValue = duration_to_string(*static_cast<const DurationT*>(apVal));
        }
        void setValue(void *apVal, const std::string& arValue) const override
        {
            *(static_cast<DurationT*>(apVal)) = string_to_duration<DurationT>(arValue);
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            DurationT duration_value = *static_cast<const DurationT*>(apVal);
            saveDurationValue(arFormat, duration_value);
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName();
            arFormat.strm() << duration_to_string(*static_cast<const DurationT*>(apVal));
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName(false);
            arFormat.strm() << duration_to_string(*static_cast<const DurationT*>(apVal));
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            DurationT& duration_value = *static_cast<DurationT*>(apVal);
            loadDurationValue(arParser, duration_value, arCtx);
        }
        void load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
        void load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
    }; //class rPropTypeInfoStdChronoDuration


    /// Type info template for chrono timepoint types.
    template<typename TimePointT>
    class rPropTypeInfoStdChronoTimePoint : public rPropTypeInfoBase<TimePointT>
    {
    public:
        explicit rPropTypeInfoStdChronoTimePoint(const std::string& arTypeName)
            : rPropTypeInfoBase<TimePointT>(arTypeName)
        {}
        ~rPropTypeInfoStdChronoTimePoint() override = default;

        std::string value(const void *apVal) const override
        {
            return duration_to_string(static_cast<const TimePointT*>(apVal)->time_since_epoch());
        }
        void value(const void *apVal, std::string& arValue) const override
        {
            arValue = duration_to_string(static_cast<const TimePointT*>(apVal)->time_since_epoch());
        }
        void setValue(void *apVal, const std::string& arValue) const override
        {
            *(static_cast<TimePointT*>(apVal)) = TimePointT(string_to_duration<typename TimePointT::duration>(arValue));
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            const TimePointT& time_point = *static_cast<const TimePointT*>(apVal);
            typename TimePointT::duration duration_value = time_point.time_since_epoch();
            saveDurationValue(arFormat, duration_value);
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName();
            arFormat.strm() << duration_to_string(static_cast<const TimePointT*>(apVal)->time_since_epoch());
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName(false);
            arFormat.strm() << duration_to_string(static_cast<const TimePointT*>(apVal)->time_since_epoch());
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            TimePointT& time_point = *static_cast<TimePointT*>(apVal);
            typename TimePointT::duration duration_value;
            loadDurationValue(arParser, duration_value, arCtx);
            time_point = TimePointT(duration_value);
        }
        void load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
        void load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
    }; //class rPropTypeInfoStdChronoTimePoint

    #if defined(_MSC_VER)
    inline void localtime(struct tm* const apTm, time_t const* const apTime) { auto error_code = localtime_s(apTm, apTime); if (0 != error_code) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("localtime_s() failed.")); }
    #elif defined(__GNUC__)
    inline void localtime(struct tm* const apTm, time_t const* const apTime) { localtime_r(apTime, apTm); }
    #else
    #error "Declare localtime for this platform!"
    #endif

    /**
    Convert wall time to string in the following format.
        yyyy-mm-dd hh:mm:ss
    */
    inline std::string datetime_to_string(const std::time_t arTime)
    {
        struct tm temp_tm{};
        rOops::localtime(&temp_tm, &arTime);
        char date_time_str[64];
        /*
        if (0==temp_tm.tm_hour && 0==temp_tm.tm_min && 0==temp_tm.tm_sec) {
            // print only date, when time is 0.
            strftime(date_time_str, sizeof(date_time_str)-1, "%Y-%m-%d", &temp_tm);
        }
        else {
        */
        strftime(date_time_str, sizeof(date_time_str)-1, "%Y-%m-%d %H:%M:%S", &temp_tm);
        return {date_time_str}; // convert char array to std::string
    }

    /**
    The preferred date-time format is the following:
        yyyy-mm-dd hh:mm:ss
    But there are other supported date formats:
        yyyy-mm-dd
        yyyy/mm/dd
        mm/dd/yyyy
        dd.mm.yyyy
    e.g.:
        2019-02-08 10:53:09
        2019/02/08 10:53:09
        02/08/2019 10:53:09
        08.02.2019 10:53:09
    */
    inline std::time_t string_to_datetime(const std::string& arDateTimeStr)
    {
        if (arDateTimeStr.empty()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Date-time string is empty."));
        }

        struct tm stm{};
        memset(&stm, 0, sizeof(stm)); // Fills all members with 0.

        // Process the date part of arDateTimeStr.
        // Try to find one of the date separator characters (-/.),
        // and process the date string according to the format.
        std::size_t date_separator = std::string::npos;
        if (std::string::npos != arDateTimeStr.find('-')) {
            // yyyy-mm-dd
            // TODO: check the 2nd '-'. Use RegEx?
            if (3 != sscanf(&arDateTimeStr[0], "%d-%d-%d", &stm.tm_year, &stm.tm_mon, &stm.tm_mday)) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
            }
        }
        else if(std::string::npos != (date_separator=arDateTimeStr.find('/'))) {
            // TODO: check the 2nd
            if (date_separator < 3) {
                // mm/dd/yyyy
                if (3 != sscanf(&arDateTimeStr[0], "%d/%d/%d", &stm.tm_mon, &stm.tm_mday, &stm.tm_year)) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
                }
            }
            else if ('/' == arDateTimeStr[4]) {
                // yyyy/mm/dd
                if (3 != sscanf(&arDateTimeStr[0], "%d/%d/%d", &stm.tm_year, &stm.tm_mon, &stm.tm_mday)) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
                }
            }
            else {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
            }
        }
        else if(std::string::npos != arDateTimeStr.find('.')) {
            // dd.mm.yyyy
            // TODO: check the 2nd
            if (3 != sscanf(&arDateTimeStr[0], "%d.%d.%d", &stm.tm_mday, &stm.tm_mon, &stm.tm_year)) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
            }
        }
        else {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. None of the expected date separator characters (-/.) found: " << arDateTimeStr));
        }
        // end of processing date part

        // Process the optional time part of the date-time string.
        std::size_t time_separator = arDateTimeStr.find(' ');
        if (std::string::npos==time_separator) time_separator = arDateTimeStr.find('T');
        if (std::string::npos!=time_separator) {
            // Separator of date and time part (space or T) is found.
            std::size_t colon1 = arDateTimeStr.find(':', time_separator+1);
            if ( std::string::npos == colon1 ) {
                // No colon, try to read hours.
                if ( time_separator+1 < arDateTimeStr.length() ) {
                    if ( isdigit(arDateTimeStr[time_separator+1]) ) {
                        // HH
                        if (1 != sscanf(&arDateTimeStr[time_separator+1], "%d", &stm.tm_hour)) {
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
                        }
                    }
                    else {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Time part is missing: " << arDateTimeStr));
                    }
                }
                // else there is a space or T character at the end of the arDateTimeStr.
            }
            else {
                // First colon is found. Try to find a 2nd one.
                std::size_t colon2 = arDateTimeStr.find(':', colon1+1);
                if (std::string::npos==colon2) {
                    // HH:MM
                    if (2 != sscanf(&arDateTimeStr[time_separator+1], "%d:%d", &stm.tm_hour, &stm.tm_min)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
                    }
                }
                else {
                    // HH::MM::SS
                    if (3 != sscanf(&arDateTimeStr[time_separator+1], "%d:%d:%d", &stm.tm_hour, &stm.tm_min, &stm.tm_sec)) {
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value: " << arDateTimeStr));
                    }
                }
            }
        }
        // end of processing time part

        // For compatibility reasons date/time range is limited to 1970-2038.
        // This is the range covered with 32-bits long signed or unsigned time_t. (e.g. QNX6 uses unsigned time_t.)
        // https://en.wikipedia.org/wiki/Unix_time
        if (1970>stm.tm_year) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Year is less then 1970: " << arDateTimeStr));
        if (stm.tm_year>2037) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Year is less more than 2037: " << arDateTimeStr));
        stm.tm_year -= 1900;
        --stm.tm_mon; if (0>stm.tm_mon || stm.tm_mon>11) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Month is out of range [1..12]: " << arDateTimeStr));
        if (1>stm.tm_mday || stm.tm_mday>31) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Day is out of range [1..31]: " << arDateTimeStr));
        if (0>stm.tm_hour || stm.tm_hour>23) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Hour is out of range [0..23]: " << arDateTimeStr));
        if (0>stm.tm_min || stm.tm_min>59) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Minutes is out of range [0..59]: " << arDateTimeStr));
        if (0>stm.tm_sec || stm.tm_sec>61) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Seconds is out of range [0..61]: " << arDateTimeStr));  // 61 for leap seconds ....
        stm.tm_isdst = -1; // DST info is not available.
        auto ret_val = mktime(&stm);
        if (-1 == ret_val) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid date-time value. Calendar time cannot be represented (mktime returned -1): " << arDateTimeStr));
        }
        return ret_val;
    }

    /// Type info template for chrono system clock's timepoint types.
    template<typename TimePointT>
    class rPropTypeInfoStdChronoSystemClock : public rPropTypeInfoBase<TimePointT>
    {
        const char cOpeningChar = '\"';
        const char cClosingChar = '\"';
    public:
        explicit rPropTypeInfoStdChronoSystemClock(const std::string& arTypeName)
            : rPropTypeInfoBase<TimePointT>(arTypeName)
        {}
        ~rPropTypeInfoStdChronoSystemClock() override = default;
        char getOpeningCharacter() const override
            { return cOpeningChar; }
        char getClosingCharacter() const override
            { return cClosingChar; }

        std::string value(const void *apVal) const override
        {
            TimePointT time_point = *static_cast<const TimePointT*>(apVal);
            std::chrono::system_clock::time_point system_time_point( std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(time_point.time_since_epoch()) );
            return datetime_to_string( TimePointT::clock::to_time_t(system_time_point) );
        }
        void value(const void *apVal, std::string& arValue) const override
        {
            TimePointT time_point = *static_cast<const TimePointT*>(apVal);
            std::chrono::system_clock::time_point system_time_point( std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(time_point.time_since_epoch()) );
            arValue = datetime_to_string( TimePointT::clock::to_time_t(system_time_point) );
        }
        void setValue(void *apVal, const std::string& arValue) const override
        {
            std::chrono::system_clock::time_point system_time_point = TimePointT::clock::from_time_t(string_to_datetime(arValue));
            *(static_cast<TimePointT*>(apVal)) = TimePointT( std::chrono::duration_cast<typename TimePointT::duration>(system_time_point.time_since_epoch()) );
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            TimePointT time_point = *static_cast<const TimePointT*>(apVal);
            typename TimePointT::duration duration_value = time_point.time_since_epoch();
            saveDurationValue(arFormat, duration_value);
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName();
            TimePointT time_point = *static_cast<const TimePointT*>(apVal);
            std::chrono::system_clock::time_point system_time_point( std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(time_point.time_since_epoch()) );
            arFormat.strm() << cOpeningChar << datetime_to_string(TimePointT::clock::to_time_t(system_time_point)) << cClosingChar;
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            arFormat.writeSizeTypeAndName(false);
            TimePointT time_point = *static_cast<const TimePointT*>(apVal);
            std::chrono::system_clock::time_point system_time_point( std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(time_point.time_since_epoch()) );
            arFormat.strm() << cOpeningChar << datetime_to_string(TimePointT::clock::to_time_t(system_time_point)) << cClosingChar;
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            TimePointT& time_point = *static_cast<TimePointT*>(apVal);
            typename TimePointT::duration duration_value;
            loadDurationValue(arParser, duration_value, arCtx);
            time_point = TimePointT(duration_value);
        }
        void load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
        void load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const override
        {
            setValue(apVal, arCtx.value_);
        }
    }; //class rPropTypeInfoStdChronoSystemClock

} //namespace rOops
