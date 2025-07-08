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

#include <oops/rOopsTokenizerBase.h>
#include <algorithm>
#include <array>

namespace {
    const char sWhiteSpaceChars[] = " \t\n\r\v\f";
}

namespace rOops {

    rOopsTokenizerBase::rOopsTokenizerBase(std::istream& arStrm, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines)
        : _definedWords(arPreprocessorDefines)
        , _strm(arStrm)
        , _fileName(arFileName)
    {
    }

    bool rOopsTokenizerBase::isEndOfLine(char ch)
    {
        if ('\n' == ch) {
            if (strm().get(ch)) {
                if ('\r' != ch) strm().putback(ch);
            }
            return true;
        }
        else if ('\r' == ch) {
            if (strm().get(ch)) {
                if ('\n' != ch) strm().putback(ch);
            }
            return true;
        }
        return false;
    }

    bool rOopsTokenizerBase::getLine(std::string& arStr)
    {
        char ch = '\0';
        while (strm().get(ch)) {
            if (isEndOfLine(ch)) {
                return false;
            }
            arStr.push_back(ch);
        } //while
        return endOfFile();
    }

    bool rOopsTokenizerBase::getWord(std::string& arStr, const char* apTermChars)
    {
        char ch = '\0';
        while (strm().get(ch)) {
            if (nullptr != strchr(apTermChars, ch)) {
                strm().putback(ch);
                return false;
            }
            if (nullptr != strchr(sWhiteSpaceChars, ch)) {
                strm().putback(ch);
                return false;
            }
            arStr.push_back(ch);
        } //while
        return endOfFile();
    }

    bool rOopsTokenizerBase::getQuoted(const char aQuote, std::string& arStr)
    {
        const char cEscape = '\\';
        char ch = '\0';
        while (strm().get(ch)) {
            if( isEndOfLine(ch) ) {
                newLine();
                arStr.push_back('\n');  // TODO: push back the real line end LF or CR or both.
                continue;
            }
            if (cEscape == ch) {
                // Drop the escaped character.
                if (strm().get(ch)) {
                    if (aQuote == ch) {
                        arStr.push_back(ch);
                    }
                    else {
                        switch (ch) {
                            case '\'':
                            case '"':
                            case '?':
                            case '\\':
                                arStr.push_back(ch);
                                break;
                            case 'a': arStr.push_back(0x07); break;
                            case 'b': arStr.push_back(0x08); break;
                            case 'f': arStr.push_back(0x0c); break;
                            case 'n': arStr.push_back(0x0a); break;
                            case 'r': arStr.push_back(0x0d); break;
                            case 't': arStr.push_back(0x09); break;
                            case 'v': arStr.push_back(0x0b); break;
                            case 'x':
                                {
                                    unsigned val;
                                    strm() >> std::hex >> val;
                                    if (val < 256) {
                                        arStr.push_back(val);
                                    }
                                    else {
                                        throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Invalid escape sequence. Hexadecimal value is more than 255: " << val));
                                    }
                                }
                                break;
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                                {
                                    unsigned val;
                                    strm().putback(ch);
                                    strm() >> std::oct >> val;
                                    if (val < 256) {
                                        arStr.push_back(val);
                                    }
                                    else {
                                        throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Invalid escape sequence. Octal value is more than 255: " << val));
                                    }
                                }
                                break;
                            default:
                                throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Invalid escape character: " << ch));
                        }
                    }
                }
            }
            else if (aQuote == ch) {
                return false;
            }
            else {
                arStr.push_back(ch);
            }
        } //while
        return endOfFile();
    }

    bool rOopsTokenizerBase::getTabulation(std::string& arStr)
    {
        arStr = "";
        char ch = '\0';
        while (strm().get(ch)) {
            if (strchr(" \t", ch)) {
                arStr.push_back(ch);
            }
            else {
                strm().putback(ch);
                return false;
            }
        } //while
        return endOfFile();
    }

    void rOopsTokenizerBase::processInclude(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        if (arLine.length() <= aEndOfKeywordPos) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "#include statement has no argument."));
        }
        auto bgn_pos = arLine.find('<', aEndOfKeywordPos);
        if (std::string::npos == bgn_pos) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Cannot find '<' character in #include statement."));
        }
        auto end_pos = arLine.find('>', bgn_pos + 1);
        if (std::string::npos == end_pos) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Cannot find '>' character in #include statement."));
        }
        IncludeFileInfo info(arLine.substr(bgn_pos + 1, end_pos - bgn_pos - 1));
        if (info.strm.good()) {
            _strmStack.push(std::move(info));
        }
        else {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Error opening include file " << info.fileName));
        }
    }

    size_t rOopsTokenizerBase::parse1stArg(std::string& arArg, const std::string& arLine, size_t aFrom, const char* apErrorMessage)
    {
        if (arLine.length() <= aFrom) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, apErrorMessage));
        }
        auto arg_bgn = arLine.find_first_not_of(sWhiteSpaceChars, aFrom);
        if (std::string::npos==arg_bgn) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, apErrorMessage));
        }
        auto arg_end = arLine.find_first_of(sWhiteSpaceChars, arg_bgn);
        if (std::string::npos==arg_end) {
            arg_end = arLine.length();
        }
        arArg = arLine.substr(arg_bgn, arg_end-arg_bgn);
        if (arArg.empty()) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, apErrorMessage));
        }
        return arg_end;
    }

    bool rOopsTokenizerBase::clearSkipInput( const std::string& arArg)
    {
        if (_definedWords.end() == _definedWords.find(arArg)) {
            // Set the _skipInput flag, if the keyword is not found in _definedWords map.
            _skipInput = true;
            return false;
        }
        else {
            // Clear the _skipInput flag, if the keyword is found in _definedWords map.
            _skipInput = false;
            return true;
        }
    }

    bool rOopsTokenizerBase::setSkipInput( const std::string& arArg)
    {
        if (_definedWords.end() == _definedWords.find(arArg)) {
            // Clear the _skipInput flag, if the keyword is not found in _definedWords map.
            _skipInput = false;
            return false;
        }
        else {
            // Set the _skipInput flag, if the keyword is found in _definedWords map.
            _skipInput = true;
            return true;
        }
    }

    void rOopsTokenizerBase::processDefine(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // Parse the defined word and its value and store them in _definedWords map.
        std::string arg;
        auto arg_end = parse1stArg(arg, arLine, aEndOfKeywordPos,"#define statement has no argument.");
        // The rest of the line from the first non-whitespace character is the value.
        std::string val = "";
        auto val_bgn = arLine.find_first_not_of(sWhiteSpaceChars, arg_end);
        if (std::string::npos != val_bgn) {
            val = arLine.substr(val_bgn, arLine.length()-val_bgn);
        }
        if (_definedWords.end() != _definedWords.find(arg)) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "#define argument already defined: " << arg));
        }
        _definedWords[arg] = val;
    }

    void rOopsTokenizerBase::processUndefine(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // Parse the word and remove it from _definedWords map.
        std::string arg;
        parse1stArg(arg, arLine, aEndOfKeywordPos,"#undef statement has no argument.");
        _definedWords.erase(arg);
    }

    void rOopsTokenizerBase::processIfDefined(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // Parse the argument of the #ifdef keyword.
        std::string arg;
        parse1stArg(arg, arLine, aEndOfKeywordPos,"#ifdef statement has no argument.");
        PreprocStatus status{ PreprocKeywords::IfDef, _skipInput, false };
        status.branch = clearSkipInput(arg);
        _preProcessorStack.push(status);  // Push PreprocKeywords::IfDef to the _preProcessorStack.
    }

    void rOopsTokenizerBase::processIfNotDefined(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // Parse the argument of the #ifndef keyword, and set the _skipInput flag, if the keyword is found in _definedWords map.
        std::string arg;
        parse1stArg(arg, arLine, aEndOfKeywordPos,"#ifndef statement has no argument.");
        PreprocStatus status{ PreprocKeywords::IfNDef, _skipInput, false };
        status.branch = !setSkipInput(arg);
        _preProcessorStack.push(status);  // Push PreprocKeywords::IfNDef to the _preProcessorStack.
    }

    void rOopsTokenizerBase::processIf(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // TODO handle expressions like A==B. Works like #ifdef for now.
        std::string arg;
        parse1stArg(arg, arLine, aEndOfKeywordPos,"#if statement has no argument.");
        PreprocStatus status{ PreprocKeywords::If, _skipInput, false };
        status.branch = clearSkipInput(arg);
        _preProcessorStack.push(status);  // Push PreprocKeywords::If to the _preProcessorStack.
    }

    void rOopsTokenizerBase::processElIf(const std::string& arLine, std::size_t aEndOfKeywordPos)
    {
        // TODO handle expressions line A==B.
        std::string arg;
        parse1stArg(arg, arLine, aEndOfKeywordPos,"#elif statement has no argument.");
        if (_preProcessorStack.empty()) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #elif statement."));
        }
        auto prev_state = _preProcessorStack.top();
        // Check if _preProcessorStack.top() is on of #ifdef, #if, #ifndef, #elif.
        static const std::array<PreprocKeywords, 5> prevStatements = {PreprocKeywords::IfDef, PreprocKeywords::IfNDef, PreprocKeywords::If, PreprocKeywords::ElIf};
        if (prevStatements.end()==std::find(prevStatements.begin(), prevStatements.end(), prev_state.keyword)) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #elif statement. No previous #if/#ifdef/#ifndef/#elif statement found."));
        }
        PreprocStatus status{ PreprocKeywords::ElIf, prev_state.skip, prev_state.branch };
        if (prev_state.branch) {
            // One of the previous branches already executed. Skip this one.
            _skipInput = true;
        }
        else {
            // Check the argument, and set the skipping flag according to it.
            status.branch |= clearSkipInput(arg);
        }
        // Pop previous if statement and push the new elif.
        _preProcessorStack.pop();
        _preProcessorStack.push(status);  // Push PreprocKeywords::ElIf to the _preProcessorStack.
    }

    void rOopsTokenizerBase::processElse(const std::string& /*arLine*/, std::size_t /*aEndOfKeywordPos*/)
    {
        if (_preProcessorStack.empty()) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #else statement."));
        }
        auto prev_state = _preProcessorStack.top();
        // Check if _preProcessorStack.top() is on of #ifdef, #if, #ifndef, #elif.
        static const std::array<PreprocKeywords, 5> prevStatements = {PreprocKeywords::IfDef, PreprocKeywords::IfNDef, PreprocKeywords::If, PreprocKeywords::ElIf};
        if (prevStatements.end()==std::find(prevStatements.begin(), prevStatements.end(), prev_state.keyword)) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #else statement. No previous #if/#ifdef/#ifndef/#elif statement found."));
        }
        _preProcessorStack.pop();
        _preProcessorStack.emplace(PreprocKeywords::Else, prev_state.skip, true);
        _skipInput = prev_state.branch;
    }

    void rOopsTokenizerBase::processEndIf(const std::string& /*arLine*/, std::size_t /*aEndOfKeywordPos*/)
    {
        if (_preProcessorStack.empty()) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #endif statement."));
        }
        auto prev_state = _preProcessorStack.top();
        // Check if _preProcessorStack.top() is one of #ifdef, #if, #ifndef, #elif #else.
        static const std::array<PreprocKeywords, 5> prevStatements = {PreprocKeywords::IfDef, PreprocKeywords::IfNDef, PreprocKeywords::If, PreprocKeywords::ElIf, PreprocKeywords::Else};
        if (prevStatements.end()==std::find(prevStatements.begin(), prevStatements.end(), prev_state.keyword)) {
            throw std::runtime_error(rOOPS_PARSING_ERROR_EXCEPTION(*this, "Unexpected #endif statement."));
        }
        _skipInput = prev_state.skip;
        // Pop previous if / elif / else statement.
        _preProcessorStack.pop();
    }

    rOopsTokenizerBase::PreprocKeywords rOopsTokenizerBase::preprocessor(const std::string& arLine)
    {
        static const std::unordered_map<std::string, PreprocKeywords> sKeywords = {
            {"include",PreprocKeywords::Include},
            {"define", PreprocKeywords::Define},
            {"undef",  PreprocKeywords::UnDef},
            {"ifdef",  PreprocKeywords::IfDef},
            {"ifndef", PreprocKeywords::IfNDef},
            {"if",     PreprocKeywords::If},
            {"elif",   PreprocKeywords::ElIf},
            {"else",   PreprocKeywords::Else},
            {"endif",  PreprocKeywords::EndIf},
        };
        auto first_char = arLine.find_first_not_of(sWhiteSpaceChars);
        auto end_index = arLine.find_first_of(sWhiteSpaceChars, first_char+1);
        if (std::string::npos == end_index) end_index = arLine.length();
        auto keyword_found = sKeywords.find( arLine.substr(first_char, end_index - first_char) );
        if (sKeywords.end() == keyword_found) {
            // comment line
            return PreprocKeywords::Comment;
        }
        switch (keyword_found->second) {
            case PreprocKeywords::Include:
                processInclude(arLine, end_index);
                break;
            case PreprocKeywords::Define:
                processDefine(arLine, end_index);
                break;
            case PreprocKeywords::UnDef:
                processUndefine(arLine, end_index);
                break;
            case PreprocKeywords::IfDef:
                processIfDefined(arLine, end_index);
                break;
            case PreprocKeywords::IfNDef:
                processIfNotDefined(arLine, end_index);
                break;
            case PreprocKeywords::If:
                processIf(arLine, end_index);
                break;
            case PreprocKeywords::ElIf:
                processElIf(arLine, end_index);
                break;
            case PreprocKeywords::Else:
                processElse(arLine, end_index);
                break;
            case PreprocKeywords::EndIf:
                processEndIf(arLine, end_index);
                break;
            case PreprocKeywords::Comment:
                break;
        }
        return keyword_found->second;
    }

} //namespace rOops
