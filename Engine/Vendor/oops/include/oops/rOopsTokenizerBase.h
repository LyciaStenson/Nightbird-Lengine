#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2019:
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
Base class of tokenizer for implementing the common part.
*/

#include <oops/rPropTypeDef.h>
#include <oops/rPropLog.h>
#include <cstring>
#include <fstream>
#include <stack>
#include <unordered_map>

// TODO: Remove this macro. Line number is added to all exception messages in the parser.
#define rOOPS_PARSING_ERROR_EXCEPTION(tokenizer, err_msg)                 \
    ([&]() {                                                              \
        std::stringstream strm;                                           \
        strm << "Parsing error: " << (tokenizer).fileName()               \
                                  << "(" << (tokenizer).lineNr() <<"): "; \
        strm << err_msg;                                                  \
        rOops::printLogMsg(1, __FILE__, __LINE__, strm.str());            \
        return strm.str(); })()


namespace rOops {

class rOopsTokenizerBase
{
public:
    enum class PreprocKeywords : short { Comment, Include, Define, UnDef, IfDef, IfNDef, If, ElIf, Else, EndIf };
    rOopsTokenizerBase(rOops::istream_type& arStrm, const std::string& arFileName, const PreprocessorDefines_t& arPreprocessorDefines);
    bool isEndOfLine(char ch);
    /// Reimplement std::getline() for handling any kind of line ending.
    bool getLine(std::string& arStr);
    /// Get word similarly to std::getline().
    bool getWord(std::string& arStr, const char* apTermChars = "[]{}=,;#");
    /// Get a stream of characters terminated with any character of arTerm.
    bool getQuoted(char aQuote, std::string& arStr);
    /// Get the tabulation string.
    bool getTabulation(std::string& arStr);
    /// Return the <file_name>(<line_nr>) information of the current parsing position.
    std::string where(size_t aLineNr) const
    {
        std::string str;
        str += fileName();
        str += '(';
        str += std::to_string(aLineNr);
        str += "): ";
        return str;
    }

    inline void skipWhiteSpace()
    {
        char tkn{ '\0' };
        while (true) {
            // Try to get the next character.
            strm().get(tkn);
            if (!strm()) {
                // End of file or end of string, return from include.
                if (endOfFile()) {
                    return;
                }
                continue;
            }
            // Skip end of line and increment line counter.
            if (isEndOfLine(tkn)) {
                newLine();
                continue;
            }
            // Skip whitespace
            if (std::isspace(tkn) || '\0' == tkn) {
                continue;
            }
            if (';' == tkn) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected ';' at the end."));
            }
            strm().putback(tkn);
            return;
        }
    }

protected:
    inline rOops::istream_type& strm()
    {
        return (_strmStack.empty()) ? _strm : _strmStack.top().strm;
    }
    inline const std::string& fileName() const
    {
        return (_strmStack.empty()) ? _fileName : _strmStack.top().fileName;
    }
    inline std::size_t lineNr() const
    {
        return (_strmStack.empty()) ? _lineNr : _strmStack.top().lineNr;
    }
    inline void newLine()
    {
        if (_strmStack.empty()) { ++_lineNr; } else { ++(_strmStack.top().lineNr); }
    }
    bool endOfFile()
    {
        if (_strmStack.empty()) {
            return true;
        }
        else {
            _strmStack.pop();
            return false;
        }
    }
    void processInclude(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processDefine(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processUndefine(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processIfDefined(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processIfNotDefined(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processIf(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processElIf(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processElse(const std::string& arLine, std::size_t aEndOfKeywordPos);
    void processEndIf(const std::string& arLine, std::size_t aEndOfKeywordPos);
    PreprocKeywords preprocessor(const std::string& arLine);
    bool skipInput() const
        { return _skipInput; }
private:
    size_t parse1stArg(std::string& arArg, const std::string& arLine, size_t aFrom, const char* apErrorMessage);
    /// Set the _skipInput flag for #ifdef. If arg is defined -> no skip, _skipInput = false. Returns true if arg is found in _definedWords.
    bool clearSkipInput(const std::string& arArg);
    /// Set the _skipInput flag for #ifndef.  If arg is defined -> skip, _skipInput = true. Returns true if arg is found in _definedWords.
    bool setSkipInput(const std::string& arArg);
    struct IncludeFileInfo
    {
        explicit IncludeFileInfo(const std::string& arFileName)
            : strm(arFileName, std::ios_base::in | std::ios_base::binary)
            , fileName(arFileName)
        {}
        //std::unique_ptr<rOops::istream_type> strm;
        std::ifstream strm;
        std::string fileName;
        std::size_t lineNr{ 1 };
    };
    std::stack<IncludeFileInfo> _strmStack; /// Stack of stream objects created for #include statements.
    PreprocessorDefines_t _definedWords; /// Key - value map of preprocessor variables. Filled from #define statements, command line, and environmnet variables.
    /// Preprocessor status.
    /**
    Preprocessor statements may be nested.
    These nested levels are stored in the preprocessor stack.
    3 different information is stored.
    Keyword: It must be checked, if the keywords are in the proper order: e.g. #ifdef ... #else ... #endif
    Skip:    When return from a block the skip input status has to be restored.
    Branch:  While an #if ... #elif ... # else ... #endif statement is evaluated, the parser has to know,
             if on of the previous braches was executed, and the brach have to be skipped independently of is argumnet.
    */
    struct PreprocStatus
    {
        PreprocKeywords keyword{ PreprocKeywords::Define };  /// Store the preprocessor statent.
        bool skip{ false }; /// Store the skip input status of the outer preprocessor statement.
        bool branch{ false }; /// Set when a branch condition was true.
        PreprocStatus( PreprocKeywords aKeyword, bool aSkip, bool aBranch)
            : keyword(aKeyword), skip(aSkip), branch(aBranch)
            {}
    };
    std::stack<PreprocStatus> _preProcessorStack;
    rOops::istream_type& _strm; /// The original input stream.
    std::size_t _lineNr{ 1 };
    std::string _fileName;
    bool _skipInput{ false };
};

} //namspace rOops
