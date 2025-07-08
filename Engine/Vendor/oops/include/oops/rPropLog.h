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
Log callback for printing log messages
to the application log system.
*/

#include <functional>
#include <iomanip>
#include <string>
#include <sstream>

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

//#pragma message(VAR_NAME_VALUE( PrintingTheValueOfThisMacro ))


#define rOOPS_TEST_MODE
//#define rOOPS_LOG

#define rOOPS_BUILD_EXCEPTION_MESSAGE(err_msg)                  \
    ([&]() {                                                    \
        std::stringstream strm;                                 \
        strm << err_msg;                                        \
        rOops::printLogMsg(1, __FILE__, __LINE__, strm.str());  \
        return strm.str(); })()

namespace rOops
{

    inline std::string escaping(const char ch)
    {
        switch (ch) {
        case '\0': return "\\0";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        default:
            if (ch < 0x20) {
                std::stringstream strm;
                strm << "\\x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(ch);
                return strm.str();
            }
            else {
                return {1, ch};
            }
        } //switch
    }

    using LogCallback = std::function<void(int aLogLevel, const char* aFileName, int aLine, const std::string& aLogMsg)>;
    void registerLogCallback(LogCallback aCallback);
    void printLogMsg(int aLogLevel, const char* aFileName, int aLine, const std::string& aLogMsg);

} //namespace rOops


#ifdef rOOPS_LOG

#define rDbgError(aLogMsg)   { std::stringstream strm; strm << aLogMsg; rOops::printLogMsg(1, __FILE__, __LINE__, strm.str()); }
#define rDbgWarning(aLogMsg) { std::stringstream strm; strm << aLogMsg; rOops::printLogMsg(2, __FILE__, __LINE__, strm.str()); }
#define rDbgTrace(aLogMsg)   { std::stringstream strm; strm << aLogMsg; rOops::printLogMsg(3, __FILE__, __LINE__, strm.str()); }

#else

#define rDbgError(aLogMsg)
#define rDbgWarning(aLogMsg)
#define rDbgTrace(aLogMsg)

#endif //rOOPS_LOG
