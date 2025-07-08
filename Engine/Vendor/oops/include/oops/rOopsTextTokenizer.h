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
Tokinezer for Oops text file format.
*/

#include <oops/rOopsTokenizerBase.h>
#include <oops/rPropLog.h>
#include <cctype>
#include <iostream>
#include <string>

namespace rOops
{

    struct TextToken
    {
        explicit TextToken(const char aCh = '\0', const std::string& arStr = "")
            : _token(aCh), _str(arStr)
        {}
        char _token{ '\0' };
        std::string _str;
        std::size_t _lineNr{ 0 };
        std::string to_string() const
        {
            return (_str.empty()) ? escaping(_token) : _str;
        }
    }; //struct TextToken

    class rOopsTextTokenizer : public rOopsTokenizerBase
    {
    public:
        rOopsTextTokenizer(rOops::istream_type& arStrm, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines)
            : rOopsTokenizerBase(arStrm, arFileName, arPreprocessorDefines)
        {}
        TextToken getNextToken();
    }; //class rOopsTextTokenizer

    inline TextToken rOopsTextTokenizer::getNextToken()
    {
        TextToken tkn;
        while (true) {
            // Try to get the next character.
            strm().get(tkn._token);
            if (!strm() || '\0'==tkn._token) {
                // End of file or end of string, return from include.
                if (endOfFile()) {
                    return TextToken();
                }
                continue;
            }
            // Skip end of line and increment line counter.
            if (isEndOfLine(tkn._token)) {
                newLine();
                continue;
            }
            // Skip whitespace
            if (std::isspace(tkn._token)) {
                continue;
            }
            // Check preprocessor status, and skip input if an ifdef statement set the skipInput flag.
            // Lines starting with '#' has to be processed as preprocessor statement.
            if (skipInput() && '#'!=tkn._token) {
                continue;
            }
            tkn._lineNr = lineNr();
            switch (tkn._token) {
                // Single character tokens:
                case '=':
                case ',':
                case ';':
                case '{':
                case '}':
                case '[':
                case ']':
                case '<':
                case '>':
                    return tkn;
                case '!':
                    // Type name
                    getWord(tkn._str);
                    return tkn;
                case '"':  // String literal
                case '\'': // Character literal
                    tkn._str = "";
                    getQuoted(tkn._token, tkn._str);
                    return tkn;
                case '(':  // User defined type.
                    tkn._str = "";
                    getQuoted(')', tkn._str);
                    return tkn;
                case '&':
                case '*':
                    // Pointer
                    //--- tkn._str = tkn._token;
                    getWord(tkn._str);
                    return tkn;
                case '#':
                    // Comment, skip the line.
                    getLine(tkn._str);
                    newLine(); // increment line counter in case of comment or preprocessor statement.
                    if (PreprocKeywords::Comment != preprocessor(tkn._str)) {
                        // Do not return the preprocessor statement.
                        tkn._str = "";
                        continue;
                    }
                    return tkn;
                default:
                    if (std::isalpha(tkn._token) || '_'==tkn._token) {
                        // Idetifier
                        tkn._str = tkn._token;
                        tkn._token = 'I';
                        getWord(tkn._str);
                        return tkn;
                    }
                    else {
                        // Number
                        tkn._str = tkn._token;
                        tkn._token = 'N';
                        getWord(tkn._str);
                        return tkn;
                    }
            } //switch
        } //while
        return TextToken();
    }

} //namespace rOops
