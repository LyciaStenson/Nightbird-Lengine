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

#include <oops/rPropTypeInfo.h>
#include <oops/rFormatBase.h>

namespace rOops
{

class rTextFormat : public rFormatBase
{
public:
    rTextFormat(std::ostream& arStrm, std::uint8_t aProtocol, const OopsConfig& arConfig)
        : rFormatBase(arStrm, arConfig)
        , protocol_(aProtocol)
    {}
    inline void flush()
    {
        strm_ << std::flush;
    }
    void doIndent()
    {
        endOfLineAlreadyWritten_ = false;
        if (!doNotIdentNow_) {
            for (int ii = 0; ii < indent_; ++ii) strm_ << "  ";
        }
        doNotIdentNow_ = true;
    }
    void writeEndOfLine()
    {
        // TODO: Delete spaces from the end of the line. How?
        strm_ << "\n";
        doNotIdentNow_ = false;
        endOfLineAlreadyWritten_ = true;
    }
    void setName(const std::string& arPropName)
    {
        propName_ = arPropName;
    }
    void setType(const rPropTypeInfo* apTI, OopsConfig::WriteTypeName aWriteTypeNameAbove, std::size_t nrOfElements)
    {
        typeInfo_ = apTI;
        writeTypeNameAbove_ = aWriteTypeNameAbove;
        nrOfElements_ = nrOfElements;
    }
    void setTopLevel(const rPropTypeInfo* apTI, const std::string& arPropName)
    {
        propName_ = arPropName;
        setType(apTI, OopsConfig::WriteTypeName::eNever, 1);
    }
    void setPointedObject(const rPropTypeInfo* apTI, const void* apObjAddr)
    {
        propName_ = '*';
        propName_ += std::to_string(reinterpret_cast<std::uintptr_t>(apObjAddr));
        setType(apTI, OopsConfig::WriteTypeName::eNever, 1);
    }
    void writeQuoted(char aChar, char aDelimiter)
    {
        strm_.put(aDelimiter);
        putEscapedCharacter(aChar);
        strm_.put(aDelimiter);
    }
    void writeQuoted(const std::string& arStr, char aDelimiter)
    {
        strm_.put(aDelimiter);
        for (auto ch : arStr) {
            putEscapedCharacter(ch);
        }
        strm_.put(aDelimiter);
    }
protected:
    void putEscapedCharacter(char ch)
    {
        constexpr char cEscapeChar = '\\';
        switch (ch) {
            case '\'':
            case '"':
            case '?':
            case cEscapeChar:
                strm_.put(cEscapeChar);
                strm_.put(ch);
                break;
            case 0x07: strm_.put(cEscapeChar); strm_.put('a'); break;
            case 0x08: strm_.put(cEscapeChar); strm_.put('b'); break;
            case 0x0c: strm_.put(cEscapeChar); strm_.put('f'); break;
            case 0x0a: strm_.put(cEscapeChar); strm_.put('n'); break;
            case 0x0d: strm_.put(cEscapeChar); strm_.put('r'); break;
            case 0x09: strm_.put(cEscapeChar); strm_.put('t'); break;
            case 0x0b: strm_.put(cEscapeChar); strm_.put('v'); break;
            default:
                strm_.put(ch);
                break;
        }
    }
protected:
    int indent_{ 0 }; ///<  indentation level.
    bool endOfLineAlreadyWritten_{ false }; ///< Avoid writing new lines several times.
    bool doNotIdentNow_{ false }; ///< Avoid ident in the middle of the line. E.g. after list separator.
    // Context:
    std::uint8_t protocol_{ ' ' }; ///< T -text, Y - yaml, F - Flow style, P - Python.
    OopsConfig::WriteTypeName writeTypeNameAbove_{ OopsConfig::WriteTypeName::eWhenNeeded };
    std::uint64_t nrOfElements_{ 0u };
}; //class rTextFormat

template <typename FormatT>
inline void writeEnd(FormatT& arFormat, const rPropTypeInfo* pTI )
{
    if( pTI->isCompound() ) {
        arFormat.writeBlockEnd();
    }
    else if( pTI->isSTLContainer() ) {
        arFormat.writeListEnd();
    }
    else if( pTI->isAssociativeContainer() ) {
        arFormat.writeMapEnd();
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error! The given type should be compound type or container."));
    }
}

} //namespace rOops
