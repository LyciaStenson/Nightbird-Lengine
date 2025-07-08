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
Stream Format Interface Class.
*/

#include <oops/rOopsParserBase.h>
#include <oops/rOopsYamlTokenizer.h>
#include <deque>
#include <string>

namespace rOops
{

//class YamlToken;

class rOopsYamlParser : public rOopsTextParserBase
{
public:
    explicit rOopsYamlParser(std::istream& arStream, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines={})
        : rOopsTextParserBase(arStream)
        , tokenizer_(arStream, arFileName, arPreprocessorDefines)
    {
        // One token look ahead.
        next_tkn_ = tokenizer_.getNextToken();
        // clear / reset the stacks.
        indentation_.clear();
        // Initialize the indentation to the white space at the beginning of the first line. 0 if no new line at the beginning.
        indentation_.push_back( {ParserState::TopLevel, ('\n'==next_tkn_._token) ? next_tkn_._str.length() : 0} );
    }
    ~rOopsYamlParser() = default;
    ParserReturnCode parse(rOopsParserContext& arCtx);
    void parseTopLevelSeparator(rOopsParserContext&)
    {
        // Nothing to do here.
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
    struct IndentationInfo {
        ParserState state;
        size_t indent;
    };
    bool handleNewLine(rOopsParserContext& arCtx);
private:
    rOopsYamlTokenizer tokenizer_;
    YamlToken tkn_;
    YamlToken next_tkn_;
    std::deque<IndentationInfo> indentation_;
    bool skipGetToken_{ false }; ///< This flag makes possible to run the main cycle of the parser again without getting the next token.
}; //class rOopsYamlParser


} //namespace rOops
