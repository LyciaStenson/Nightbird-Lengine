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

#include <oops/rOopsParserBase.h>
#include <oops/rOopsParserContext.h>
#include <oops/rOopsBinaryFormat.h>
#include <string>


namespace rOops
{

class rOopsBinaryPrinter
{
public:
    rOopsBinaryPrinter();
    std::string printOopsBinaryFile(const std::stringstream& arStream);
    bool doNotSwapInteger() const { return doNotSwapInteger_; }
    bool doNotSwapFloat() const { return doNotSwapFloat_; }
protected:
    bool processMetaData(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arCntrl);
    bool processControlByte(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arCntrl) const;
    bool readControlByte(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arMarker, std::uint8_t& arCntrl);
protected:
    bool bigEndianInteger_{ false }; ///< true, if the system executing the parser is using big endian integers. This variable stores the result of the endianness test.
    bool bigEndianFloat_{ false }; ///< true, if the system executing the parser is using big endian floating point numbers. This variable stores the result of the endianness test.
    bool doNotSwapInteger_{ false }; ///< true, if the stream and system integer endianness are the same.
    bool doNotSwapFloat_{ false }; ///< true, if the stream and system floating point endianness are the same.
};

class rOopsBinaryParser : public rOopsParserBase, public rOopsBinaryPrinter
{
public:
    ~rOopsBinaryParser() = default;
    /**
     * Construct a parser for Oops Binary format.
     * Check and store the endianness of the CPU.
     * @param arStream - the parser will read data from this stream.
     * @param arFileName - the name of the stream or file. Only used in error messages.
     */
    rOopsBinaryParser(std::istream& arStream, const std::string& arFileName);
    /**
     * Read one entry from the input stream.
     * @param arCtx - output parameter: stores the name, type, size, value, etc. parsed from the entry.
     * @return the control byte of the entry
     */
    ParserReturnCode parse(rOopsParserContext& arCtx);
    void parseTopLevelSeparator(rOopsParserContext&)
    {
        // Nothing to do here.
    }
    /**
     * Return information of the current parsing position.
     * E.g. <file_name>(<byte position>).
     */
    std::string where() const;
    /**
     * Read function for size.
     */
    std::uint64_t readSize();
    /**
     * Read function for string, type and property name.
     * @param arStringSize - size of the string, the number of characters have to be read from the stream.
     * @param arString     - result.
     */
    void readString(std::uint64_t aStringSize, std::string& arString);
    void skipWhiteSpace()
    {
    }
private:
    std::string fileName_; ///< File name for the input stream, used in error messages only.
}; //class rOopsBinaryParser

} //namespace rOops
