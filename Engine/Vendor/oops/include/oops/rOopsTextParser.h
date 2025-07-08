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
#include <oops/rOopsTextTokenizer.h>
#include <sstream>
#include <string>

namespace rOops
{

class rOopsTextParser : public rOopsTextParserBase
{
public:
    rOopsTextParser(std::istream& arStream, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines={})
        : rOopsTextParserBase(arStream)
        , tokenizer_(istrm_, arFileName, arPreprocessorDefines)
    {}
    ~rOopsTextParser() = default;
    ParserReturnCode parse(rOopsParserContext& arCtx);
    void parseTopLevelSeparator(rOopsParserContext& arCtx)
    {
        // Top level ';' is left on the stream.
        (void)parse(arCtx);
    }
    /// Return information of the current parsing position. E.g. <file_name>(<line_nr>).
    std::string where() const
    {
        return tokenizer_.where(tkn_._lineNr);
    }
    void skipWhiteSpace()
    {
        tokenizer_.skipWhiteSpace();
    }
protected:
    rOops::rOopsTextTokenizer tokenizer_;
    rOops::TextToken tkn_;
}; //class rOopsTextParser

} //namespace rOops
