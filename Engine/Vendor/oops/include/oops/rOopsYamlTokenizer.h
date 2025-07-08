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
Tokenizer for YAML file format.
*/

#include <oops/rOopsTokenizerBase.h>
#include <oops/rPropLog.h>
#include <cctype>
#include <iostream>
#include <string>

namespace rOops
{

    enum class YamlTokenStatus : char {
        Base,
        Value, // : or - was read
        FlowSequenceOrMapping
    };
    inline std::string to_string(YamlTokenStatus sts)
    {
        switch (sts) {
        case YamlTokenStatus::Base:  return "Base";
        case YamlTokenStatus::Value: return "Val_";
        case YamlTokenStatus::FlowSequenceOrMapping: return "Flow";
        default: return "Invalid";
        }
    }
    class YamlToken
    {
    public:
        explicit YamlToken(const char aCh = '\n', const std::string& arStr = "")
            : _token(aCh), _str(arStr)
            {}
        YamlTokenStatus _status{ YamlTokenStatus::Base };
        char _token{ '\n' }; ///< Initialize to new line. '\0' means end-of-file, which force the parser to exit.
        std::size_t _lineNr{ 0 };
        std::string _str;
        inline std::string to_string() const
        {
            return (_str.empty()) ? escaping(_token) : _str;
        }
    }; //struct YamlToken

    class rOopsYamlTokenizer : public rOopsTokenizerBase
    {
    public:
        rOopsYamlTokenizer(rOops::istream_type& arStrm, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines)
            : rOopsTokenizerBase(arStrm, arFileName, arPreprocessorDefines)
            {
                tokenStatus.push(YamlTokenStatus::Base);
            }
        YamlToken getNextToken();
    protected:
        static bool handleNullValue(std::string& arStr)
        {
            if ("~"==arStr || "null"==arStr|| "Null"==arStr|| "NULL"==arStr) {
                arStr = "";
                return true;
            }
            return false;
        }
        /// Get word similarly to std::getline().
        bool getYamlWord(std::string& arStr, const char* apTermChars = "\r\n()[]{},#")
        {
            char ch = '\0';
            while (strm().get(ch)) {
                if (nullptr != strchr(apTermChars, ch)) {
                    strm().putback(ch);
                    return false;
                }
                // ':' followed by space.
                if (':'==ch && strchr(" \t\n\r", strm().peek())) {
                    // Put the ':' character back to the stream.
                    // It is part of the ': ' token following the word.
                    strm().putback(ch);
                    // Remove trailing spaces.
                    arStr.erase(arStr.find_last_not_of(" \t\v") + 1);
                    return false;
                }
                arStr.push_back(ch);
            } //while
            return endOfFile();
        }
        bool getComment(std::string& arStr)
        {
            char ch = '\0';
            while (strm().get(ch)) {
                if (nullptr != strchr("\r\n", ch)) {
                    strm().putback(ch);
                    return false;
                }
                arStr.push_back(ch);
            } //while
            return endOfFile();
        }
    private:
        std::stack<YamlTokenStatus> tokenStatus;
    }; //class rOopsYamlTokenizer

    inline YamlToken rOopsYamlTokenizer::getNextToken()
    {
        char ch = '\0';
        YamlToken tkn;
        while (true) {
            // Try to get the next character.
            if ( ! strm().get(tkn._token) ) {
                // End of file, return from include.
                if (endOfFile()) {
                    tkn._token = '\0';
                    tkn._lineNr = lineNr();
                    return tkn;
                }
                continue;
            }
            // skip whitespace
            if (' '== tkn._token || '\t'==tkn._token) {
                continue;
            }
            tkn._status = tokenStatus.top();
            tkn._lineNr = lineNr();
            // Status independent tokens.
            if (YamlTokenStatus::FlowSequenceOrMapping != tokenStatus.top()) {
                if ('\r'==tkn._token || '\n'==tkn._token) {
                    char ch2 = ('\r' == tkn._token) ? '\n' : '\r';
                    if (ch2 == strm().peek()) {
                        // DOS line ending. Get the 2nd end of line character.
                        strm().get(ch);
                    }
                    newLine();  // New line: increment the line counter
                    getTabulation(tkn._str);  // Return tabulation string.
                    // Skip comments and execute preprocessor statements.
                    if ('#' == strm().peek()) {
                        strm().get(tkn._token); // #
                        getComment(tkn._str); // comment or preprocessor statement
                        // Include statement require some extra work:
                        //   - return a new line token with the appropriate tabulation.
                        //   - TODO: store the current indentation and insert it into the tabulation of the included file.
                        if (PreprocKeywords::Include == preprocessor(tkn._str)) {
                            // a new line has to be processed before starting the include file.
                            getTabulation(tkn._str);
                            tkn._token = '\n';
                            tkn._status = tokenStatus.top();
                            tkn._lineNr = lineNr();
                            return tkn;
                        }
                        // Skip the comment or preprocessor statement.
                        continue;
                    }
                    // This is a real new line. tkn._str contains the indentation string.
                    tkn._lineNr = lineNr();
                    // Do not return the new line if skipInput is true.
                    if (skipInput()) continue;
                    tkn._token = '\n'; // set the new line token.
                    return tkn;
                }
            }
            // Check preprocessor status, and skip input if an #ifdef statement set the skipInput flag.
            // Lines starting with '#' may not skipped.
            if (skipInput() && '#'!=tkn._token) {
                continue;
            }
            switch (tokenStatus.top()) {
            case YamlTokenStatus::Base:
                switch (tkn._token) {
                // Single character tokens:
                case '{':
                    tokenStatus.push(YamlTokenStatus::FlowSequenceOrMapping);
                    tkn._status = YamlTokenStatus::FlowSequenceOrMapping;
                    return tkn;
                case '[':
                case ']':
                case '>':
                    return tkn;
                case ':':
                    if (strm().get(ch)) {
                        switch (ch) {
                        case ' ':
                        case '\t':
                            // ': ' key - value separator.
                            tokenStatus.push(YamlTokenStatus::Value);
                            break;
                        case '\n':
                        case '\r':
                            // ':' key - value separator followed with new line.
                            tokenStatus.push(YamlTokenStatus::Value);
                            strm().putback(ch);
                            break;
                        default:
                            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "': ' is expected. Space must follow the ':' character."));
                        }
                    }
                    return tkn;
                case '-':
                    // sequence separator or start of document.
                    if (strm().get(ch)) {
                        switch (ch) {
                        case ' ':
                        case '\t':
                            // '- ': sequence separator
                            // TODO: check previous token, if '-' is the first character of the line?
                            tokenStatus.push(YamlTokenStatus::Value);
                            break;
                        case '\r':
                        case '\n':
                            // '- ': sequence separator followed with new line.
                            tokenStatus.push(YamlTokenStatus::Value);
                            // The new line has to be processed.
                            strm().putback(ch);
                            break;
                        case '-':
                            // '---' - Start of doc
                            if (strm().get(ch)) {
                                if ('-' != ch) throw std::runtime_error(
                                    rOOPS_PARSING_ERROR_EXCEPTION(*this, "'---' is expected. Only two dash character found."));
                                tkn._token = '\x02'; // STX, start of text
                            }
                            break;
                        default:
                            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "': ' is expected. Space must follow the '-' character."));
                        }
                    }
                    return tkn;
                case '.':
                    // '...' - End of doc
                    strm().get(ch);
                    if ('.' != ch) throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "'...' is expected. Only one dot character found."));
                    strm().get(ch);
                    if ('.' != ch) throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "'...' is expected. Only two dot character found."));
                    tkn._token = '\x03'; // ETX, end of text
                    return tkn;
                case '"':
                case '\'':
                    // Quoted text.
                    tkn._str = "";
                    getQuoted(tkn._token, tkn._str);
                    return tkn;
                case '(':
                    tkn._str = "";
                    getQuoted(')', tkn._str);
                    return tkn;
                case '#':
                    // Comment: preprocessor statement or real comment. Skip both.
                    getComment(tkn._str);
                    preprocessor(tkn._str);
                    continue;
                case '*':
                    // Address instead of name.
                    tkn._str = "";
                    getYamlWord(tkn._str);
                    return tkn;
                default:
                    // key or value.
                    tkn._str = tkn._token;
                    tkn._token = 'S'; // string
                    getYamlWord(tkn._str);
                    if (handleNullValue(tkn._str)) continue;
                    return tkn;
                } //switch
                break;
            case YamlTokenStatus::Value:
                // The '-' character may be part of the value.
                // It is the sequence token, when the next character is whitespace.
                if ('-' == tkn._token) {
                    if (strm().get(ch)) {
                        switch (ch) {
                        case ' ':
                        case '\t':
                            return tkn;
                        case '\n':
                        case '\r':
                            strm().putback(ch);
                            return tkn;
                        default:
                            // Put back to the stream, and let the default of the switch to process it as part of the Yaml word.
                            strm().putback(ch);
                        } //switch
                    } //if
                } // if '- '
                switch (tkn._token) {
                case '!': // Tag used for storing type name
                    tkn._str = "";
                    getYamlWord(tkn._str, "\n\r \t");
                    return tkn;
                case '{':
                case '[':
                    tokenStatus.push(YamlTokenStatus::FlowSequenceOrMapping);
                    tkn._status = YamlTokenStatus::FlowSequenceOrMapping;
                    return tkn;
                case '#':
                    {
                        // Comment, skip the rest of the line.
                        std::string tmp;
                        getComment(tmp);
                        continue;
                    }
                case '"':  // String literal.
                case '\'': // Character literal.
                    // Quoted text.
                    tokenStatus.pop();
                    tkn._str = "";
                    getQuoted(tkn._token, tkn._str);
                    return tkn;
                case '(':  // User defined type.
                    tokenStatus.pop();
                    tkn._str = "";
                    getQuoted(')', tkn._str);
                    return tkn;
                case ',':
                    // Skip ',' for flow mode ...
                    return tkn;
                case '*':
                    // Address instead of value.
                    tokenStatus.pop();
                    tkn._str = "";
                    getYamlWord(tkn._str, "\r\n["); // value may contain any character. Only ': ' or new line can terminate this word.
                    return tkn;
                default:
                    tokenStatus.pop();
                    tkn._str = tkn._token;
                    tkn._token = 'S'; // string
                    getYamlWord(tkn._str, "\r\n["); // value may contain any character. Only ': ' or new line can terminate this word.
                    if (handleNullValue(tkn._str)) continue;
                    return tkn;
                }
            case YamlTokenStatus::FlowSequenceOrMapping:
                if ('\n' == tkn._token || '\r' == tkn._token) {
                    newLine();
                }
                if (std::isspace(tkn._token)) continue; // skip whitespace
                tkn._lineNr = lineNr();
                switch (tkn._token) {
                case ',':
                case ':':
                    return tkn;
                case '}':
                case ']':
                    tokenStatus.pop();
                    tkn._status = tokenStatus.top();
                    return tkn;
                case '{':
                case '[': // List in list.
                    tokenStatus.push(YamlTokenStatus::FlowSequenceOrMapping);
                    return tkn;
                case '!': // Type name
                case '&': // Pointer
                    tkn._str = "";
                    getYamlWord(tkn._str, "\n\r \t");
                    return tkn;
                case '"':  // String literal.
                case '\'': // Character literal.
                    // Quoted text.
                    tkn._str = "";
                    getQuoted(tkn._token, tkn._str);
                    return tkn;
                case '(':  // User defined type.
                    tkn._str = "";
                    getQuoted(')', tkn._str);
                    return tkn;
                default:
                    // TODO: What about quoted values?
                    tkn._str = tkn._token;
                    tkn._token = 'S'; // string
                    tkn._status = tokenStatus.top();
                    getYamlWord(tkn._str, ",}]\n\r");
                    if (handleNullValue(tkn._str)) continue;
                    return tkn;
                } //switch
            default:
                throw std::runtime_error(
                    rOOPS_PARSING_ERROR_EXCEPTION(*this, "Invalid Yaml Tokenizer status."));
            } // switch(tokenStatus)
        } //while
    }

} //namespace rOops
