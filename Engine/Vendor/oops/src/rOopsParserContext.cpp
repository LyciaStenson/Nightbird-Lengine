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
Parser Context class.
*/

#include <oops/rOopsParserContext.h>

namespace rOops
{

    std::string to_string(ParserReturnCode aControl)
    {
        switch (aControl) {
            case ParserReturnCode::eNone     : return "eNone";
            case ParserReturnCode::eSignedInt: return "eSignedInt";
            case ParserReturnCode::eUnsigned : return "eUnsigned";
            case ParserReturnCode::eReal     : return "eReal";
            case ParserReturnCode::eString   : return "eString";
            case ParserReturnCode::eValue    : return "eValue";
            case ParserReturnCode::ePointer  : return "ePointer";
            case ParserReturnCode::eBool     : return "eBool";
            case ParserReturnCode::eEnum     : return "eEnum";
            case ParserReturnCode::eChar     : return "eChar";
            case ParserReturnCode::eBlockBgn : return "eBlockBegin";
            case ParserReturnCode::eBlockEnd : return "eBlockEnd";
            case ParserReturnCode::eListBgn  : return "eListBegin";
            case ParserReturnCode::eListEnd  : return "eListEnd";
            case ParserReturnCode::eMapBgn   : return "eMapBegin";
            case ParserReturnCode::eMapEnd   : return "eMapEnd";
            case ParserReturnCode::eTopLevel : return "eTopLevel";
            case ParserReturnCode::eEndOfStream : return "eEndOfStream";
        }
        return "???";
    }

    constexpr std::size_t rOopsAddressString::cCapacity;

    bool rOopsAddressString::read(std::istream& arStream, std::size_t aSize)
    {
        arStream.read(data_.data(), static_cast<std::streamsize>(aSize));
        if (arStream.good()) {
            size_ = aSize;
            std::fill(data_.begin()+size_, data_.end(), '\0');
            return true;
        }
        else {
            size_ = 0;
            return false;
        }
    }

    rOopsAddressString& rOopsAddressString::operator=(const std::string& arStr)
    {
        if ("0" == arStr) {
            clear();
            return *this;
        }
        size_ = std::min(cCapacity, arStr.size());
        std::copy(arStr.begin(), arStr.begin()+static_cast<std::string::difference_type>(size_), data_.begin());
        std::fill(data_.begin()+size_, data_.end(), '\0');
        return *this;
    }

    bool rOopsAddressString::operator==(const rOopsAddressString& arR) const
    {
        if (size_ != arR.size_) return false;
        return std::equal(data_.begin(), data_.begin()+size_, arR.data_.begin());
    }

    std::string rOopsAddressString::asString() const
    {
        std::string str;
        str.reserve(size_);
        str.append(data_.begin(), data_.begin()+size_);
        return str;
    }
} //namespace rOops
