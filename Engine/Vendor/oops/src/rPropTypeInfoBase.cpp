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

#include <oops/rPropTypeInfoBase.h>
#include <cctype>

namespace
{
    void skipWhiteSpace( rOops::istream_type& arStrm )
    {
        rOops::istream_type::char_type ch;
        while( !arStrm.eof() && (' '==arStrm.peek()) ) {
            arStrm.get(ch);
        } //while
    } //skipWhiteSpace()
}

namespace rOops
{
    // class rPropTypeInfoBool
    //========================
    rPropTypeInfoBool::rPropTypeInfoBool(const std::string& arTypeName)
    : rPropTypeInfoBase<bool>(arTypeName)
    {
    }

    rPropTypeInfoBool::~rPropTypeInfoBool() = default;

    std::string rPropTypeInfoBool::value(const void *apVal) const
    {
        return *(static_cast<const bool*>(apVal)) ? "True" : "False";
    }

    void rPropTypeInfoBool::value(const void *apVal, std::string& arValue) const
    {
        arValue = *(static_cast<const bool*>(apVal)) ? "True" : "False";
    }

    bool iequal(const std::string& a, const std::string& b)
    {
        return std::equal(a.begin(), a.end(),
            b.begin(), b.end(),
            [](char a, char b) {
            return std::tolower(a) == std::tolower(b);
        });
    }
    void rPropTypeInfoBool::setValue(void *apVal, const std::string& arValue) const
    {
        if ((arValue == "0") || iequal(arValue,"false")) {
            *(static_cast< bool*>(apVal)) = false;
        }
        else if ((arValue == "1") || iequal(arValue,"true")) {
            *(static_cast<bool*>(apVal)) = true;
        }
        else {
            // Error
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Invalid bool value: " << arValue));
        }
    }

    void rPropTypeInfoBool::save(rOopsBinaryFormat& arFormat, const void *apVal) const
    {
        arFormat.setSize((*static_cast<const bool*>(apVal)) ? 1 : 0);
        arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eBool));
    }

    void rPropTypeInfoBool::save(rOopsTextFormat& arFormat, const void *apVal) const
    {
        arFormat.writeSizeTypeAndName();
        arFormat.strm() << ((*static_cast<const bool*>(apVal)) ? "True" : "False");
    }

    void rPropTypeInfoBool::save(rOopsYamlFormat& arFormat, const void *apVal) const
    {
        arFormat.writeSizeTypeAndName(false);
        arFormat.strm() << ((*static_cast<const bool*>(apVal)) ? "True" : "False");
    }

    void rPropTypeInfoBool::load(rOopsBinaryParser&, void *apVal, rOopsParserContext& arCtx) const
    {
        // aCtxSize contains the value: 0 - false, 1 - true
        *(static_cast<bool*>(apVal)) = (0 != arCtx.size_);
    }

    void rPropTypeInfoBool::load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const
    {
        setValue(apVal, arCtx.value_);
    }

    void rPropTypeInfoBool::load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const
    {
        setValue(apVal, arCtx.value_);
    }

    // class rPropTypeInfoStdString
    //=============================

    rPropTypeInfoStdString::rPropTypeInfoStdString( const std::string& arTypeName )
    : rPropTypeInfoBase<std::string>(arTypeName)
    {
    }

    //virtual
    rPropTypeInfoStdString::~rPropTypeInfoStdString() = default;

    std::string rPropTypeInfoStdString::value( const void *apVal ) const
    {
        // Character escaping and quotation characters must be handled in the save/load implementation,
        // because they depend on the format.
        return *((const std::string*)apVal);
    }

    void rPropTypeInfoStdString::value( const void *apVal, std::string& arValue ) const
    {
        arValue = *static_cast<const std::string*>(apVal);
    }

    void rPropTypeInfoStdString::setValue( void *apVal, const std::string& arValue ) const
    {
        *static_cast<std::string*>(apVal) = arValue;
    }

    void rPropTypeInfoStdString::save(rOopsBinaryFormat& arFormat, const void *apVal) const
    {
        const std::string& str = *static_cast<const std::string*>(apVal);
        arFormat.setSize(str.size());
        arFormat.writeCtrlSizeTypeName(enum2integral(ControlByte::eString));
        arFormat.write(str.data(), str.size());
    }

    void rPropTypeInfoStdString::save(rOopsTextFormat& arFormat, const void *apVal) const
    {
        arFormat.writeSizeTypeAndName();
        arFormat.writeQuoted(*static_cast<const std::string*>(apVal), cDelimiterChar);
    }

    void rPropTypeInfoStdString::save(rOopsYamlFormat& arFormat, const void *apVal) const
    {
        arFormat.writeSizeTypeAndName(false);
        arFormat.writeQuoted(*static_cast<const std::string*>(apVal), cDelimiterChar);
    }

    void rPropTypeInfoStdString::load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const
    {
        std::string& str = *static_cast<std::string*>(apVal);
        arParser.readString(arCtx.size_, str);
    }

    void rPropTypeInfoStdString::load(rOopsTextParser&, void* apVal, rOopsParserContext& arCtx) const
    {
        setValue(apVal, arCtx.value_);
    }

    void rPropTypeInfoStdString::load(rOopsYamlParser&, void* apVal, rOopsParserContext& arCtx) const
    {
        setValue(apVal, arCtx.value_);
    }

    // Time and date
    //===============

    void readTimeAndUnits(istream_type& arStream, std::string& arString)
    {
        if (arStream.eof()) return;
        // Read time string.
        arStream >> std::noskipws;
        skipWhiteSpace(arStream);
        char ch = '\0';
        arStream >> ch;
        while (!arStream.eof()) {
            if (!isdigit(ch) && ':'!=ch && '.'!=ch) {
                break;
            }
            arString += ch;
            arStream >> ch;
        } //while

        // Read measurement unit characters.
        char ch1, ch2, ch3;
        ch1 = ch;
        switch (ch1) {
        case 'n':
        case 'u':
            // ns, us
            arStream >> ch2;
            if (arStream.eof()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file. Should be 'ns' or 'us'? Incomplete measurment unit: " << ch1));
            }
            switch(ch2) {
            case 's':
                arString += ch1;
                arString += ch2;
                break; // ns or ms
            default:
                arStream.putback(ch2);
                arStream.putback(ch1);
                break;
            }
            break;
        case 'm':
            // ms or min
            arStream >> ch2;
            if (arStream.eof()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file. Should be 'ms' or 'min'? Incomplete measurment unit: " << ch1));
            }
            switch(ch2) {
            case 's':
                arString += "ms";
                break; // ms
            case 'i':
                arStream >> ch3;
                if (arStream.eof()) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file. Should be 'min'? Incomplete measurment unit: mi"));
                }
                if ('n' == ch3) {
                    arString += "min";
                    break; // min
                }
                arStream.putback(ch3);
                arStream.putback(ch2);
                arStream.putback(ch1);
                break;
            default:
                arStream.putback(ch2);
                arStream.putback(ch1);
                break;
            }
            break;
        case 's':
            // s or sec
            arStream >> ch2;
            if (arStream.eof()) {
                // end of steram, 's' found.
                arString += 's';
            }
            else if ('e'==ch2) {
                // Try to read the 3rd character.
                arStream >> ch3;
                if (arStream.eof()) {
                    // eof after reading 'se'. This is still an 's'. The 2nd charater shall be put back to the stream.
                    arStream.putback(ch2);
                    arString += 's';
                }
                else if ('c'==ch3) {
                    // Found 'sec'.
                    arString += "sec";
                }
                else {
                    // The 3rd chacter is something else, but this is still an 's'. The 2nd and 3rd charater shall be put back to the stream.
                    arStream.putback(ch3);
                    arStream.putback(ch2);
                    arString += 's';
                }
            }
            else {
                arStream.putback(ch2);
                arString += 's';
            }
            break;
        case 'h':
            // h
            arString += 'h';
            break;
        case 'd':
            // day
            arStream >> ch2;
            if (arStream.eof()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file. Should be 'day'? Incomplete measurment unit: d"));
            }
            arStream >> ch3;
            if (arStream.eof()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file. Should be 'day'? Incomplete measurment unit: d" << ch2));
            }
            if ('a'!=ch2 || 'y'!=ch3) {
                arStream.putback(ch3);
                arStream.putback(ch2);
                arStream.putback(ch1);
            }
            else {
                arString += "day";
            }
            break;
        default:
            arStream.putback(ch1);
            break;
        }
    }

} //namespace rOops

