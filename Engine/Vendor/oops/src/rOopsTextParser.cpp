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
Text format parser.
*/

#include <oops/rOopsTextParser.h>
#include <sstream>
#include <stack>

namespace rOops
{

using ParserStateStack = std::stack<ParserState>;

namespace {
    void parseTypeName(rOopsParserContext& arCtx, const std::string& arToken)
    {
        auto addr_pos = arToken.find('&');
        if (std::string::npos == addr_pos) { 
            arCtx.typeName_ = arToken;
        }
        else {
            arCtx.typeName_ = arToken.substr(0, addr_pos);
            arCtx.address_ = arToken.substr(addr_pos+1);
        }
        auto version_pos = arCtx.typeName_.find('/');
        if (std::string::npos != version_pos) {
            arCtx.version_ = rOops::string_to<std::uint32_t>(arToken.substr(version_pos+1));
        }
    }
}

ParserReturnCode rOopsTextParser::parse(rOopsParserContext& arCtx)
{
    arCtx.clear();
    while (true) {
        tkn_ = tokenizer_.getNextToken();
        if ('\0' == tkn_._token) break; // End of input stream.
        if ('#' == tkn_._token) continue; // Comment is skipped.
        switch (arCtx.getParserState()) {

        // >>> TopLevel: <Identifier> = !<TypeName> <value>
        case ParserState::TopLevel:
            switch (tkn_._token) {
                case 'I':
                    arCtx.propName_ = tkn_._str;
                    break;
                case '*':
                    arCtx.address_ = tkn_._str;
                    break;
                case '=':
                    arCtx.pushParserState(ParserState::Value);
                    break;
                case ';':
                    // terminating ';'
                    return arCtx.returnCode_ = ParserReturnCode::eTopLevel;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< TopLevel

        // >>> Block of Statements: <Identifier> = !<TypeName> <value>
        case ParserState::Block:
            switch (tkn_._token) {
            case 'I':
                arCtx.propName_ = tkn_._str;
                break;
            case '*':
                arCtx.address_ = tkn_._str;
                break;
            case '[':
                arCtx.pushParserState(ParserState::Size);
                break;
            case '=':
                arCtx.pushParserState(ParserState::Value);
                break;
            case '}':
                arCtx.popParserState("Unexpected closing curly brackets.", tkn_._lineNr);
                return arCtx.returnCode_ = ParserReturnCode::eBlockEnd;
            case ';':
                // empty statement: ';' following '}'.
                break;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< Block

        // >>> Size: identifier[NN]
        case ParserState::Size:
            switch (tkn_._token) {
            case 'N':
                arCtx.size_ = static_cast<std::size_t>(std::stoll(tkn_._str));
                break;
            case ']':
                arCtx.popParserState("Unexpected closing square brackets.", tkn_._lineNr);
                break;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< Size

        // >>> List: coma separated list of values.
        case ParserState::List:
            switch (tkn_._token) {
            case '!':
                parseTypeName(arCtx, tkn_._str);
                break;
            case '=':
                // Key value separator found: parsing a map not a list.
                arCtx.changeParserState(ParserState::List, ParserState::Map);
                arCtx.pushParserState(ParserState::Value);
                break;
            case 'N':
            case '\'':
            case '"':
            case '(':
            case 'I': // for true, false, etc.
                arCtx.value_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::eValue;
            case '*':
                arCtx.pointer_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::ePointer;
            case ',':
                if (arCtx.isParserStateEmpty()) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected coma (,) at line " << tkn_._lineNr));
                break;
            case ']':
                arCtx.popParserState("Unexpected closing parentheses.", tkn_._lineNr);
                // At this point there are 2 options:
                // If this list was the value of a simple assignment statement, a ';' is expected.
                // If the list was part of another list, the list processing shall continue.
                if (ParserState::Value == arCtx.getParserState()) {
                    arCtx.pushParserState(ParserState::ReadValue);
                }
                return arCtx.returnCode_ = ParserReturnCode::eListEnd;
            // The list may contain a compound object.
            case '{':
                arCtx.pushParserState(ParserState::Block);
                return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
            // The list may contain another list.
            case '[':
                arCtx.pushParserState(ParserState::List);
                return arCtx.returnCode_ = ParserReturnCode::eListBgn;
            // The list may contain a map.
            case '<':
                arCtx.pushParserState(ParserState::Map);
                return arCtx.returnCode_ = ParserReturnCode::eMapBgn;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< List

        // >>> Map: coma separated list of key = value pairs.
        case ParserState::Map:
            switch (tkn_._token) {
            case '!':
                parseTypeName(arCtx, tkn_._str);
                break;
            case '=':
                // Key value separator found: parsing a map not a list.
                arCtx.changeParserState(ParserState::List, ParserState::Map);
                arCtx.pushParserState(ParserState::Value);
                break;
            case 'N':
            case '\'':
            case '"':
            case '(':
            case 'I': // for true, false, etc.
                arCtx.value_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::eValue;
            case '*':
                arCtx.pointer_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::ePointer;
            case ',':
                if (arCtx.isParserStateEmpty()) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected coma (,) at line " << tkn_._lineNr));
                break;
            case '>':
                arCtx.popParserState("Unexpected closing parentheses.", tkn_._lineNr);
                // At this point there are 2 options:
                // If this list was the value of a simple assignment statement, a ';' is expected.
                // If the list was part of another list, the list processing shall be continue.
                if (ParserState::Value == arCtx.getParserState()) {
                    arCtx.pushParserState(ParserState::ReadValue);
                }
                return arCtx.returnCode_ = ParserReturnCode::eMapEnd;
            // The list may contain a compound object.
            case '{':
                arCtx.pushParserState(ParserState::Block);
                return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
            // The list may contain another list.
            case '[':
                arCtx.pushParserState(ParserState::List);
                return arCtx.returnCode_ = ParserReturnCode::eListBgn;
            // The list may contain a map.
            case '<':
                arCtx.pushParserState(ParserState::Map);
                return arCtx.returnCode_ = ParserReturnCode::eMapBgn;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< List

        // >>> Value: anything, which may stand on the right side of the '='.
        case ParserState::Value:
            switch (tkn_._token) {
            case '!':
                //arCtx.typeName_ = tkn_._str;
                parseTypeName(arCtx, tkn_._str);
                break;
            case 'N':
            case '\'':
            case '"':
            case '(':
            case 'I': // for true, false, etc.
                arCtx.pushParserState(ParserState::ReadValue);
                arCtx.value_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::eValue;
            case '*':
                arCtx.pushParserState(ParserState::ReadValue);
                arCtx.pointer_ = tkn_._str;
                return arCtx.returnCode_ = ParserReturnCode::ePointer;
            case ';':
                // In Value state, this means empty assignment (a=;).
                arCtx.popParserState("Unexpected semicolon (;).", tkn_._lineNr);
                arCtx.value_ = "";
                return arCtx.returnCode_ = ParserReturnCode::eValue;
            // The value may contain a compound object.
            case '{':
                arCtx.changeParserState(ParserState::Value, ParserState::Block);
                return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
            // The value might be a list.
            case '[':
                arCtx.pushParserState(ParserState::List);
                return arCtx.returnCode_ = ParserReturnCode::eListBgn;
            // The value might be a map.
            case '<':
                arCtx.pushParserState(ParserState::Map);
                return arCtx.returnCode_ = ParserReturnCode::eMapBgn;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< Value

        // >>> ReadValue: set, when the value is read. This state is required for handling empty values: a = ;
        case ParserState::ReadValue:
            switch (tkn_._token) {
            case ';': // end of statement. ';' is the only acceptable character.
            case ',': // middle of a list of key-value pairs.
                // After reading the value we have to remove 2 states from the stack.
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop ReadValue
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop Value
                break;
            case ']': // end of list.
                // After reading the value we have to remove 2 states from the stack.
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop ReadValue
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop Value
                arCtx.popParserState("Unexpected closing parentheses.", tkn_._lineNr);
                // At this point there are 2 options:
                // If this list was the value of a simple assignment statement, a ';' is expected.
                // If the list was part of another list, the list processing shall be continue.
                if (ParserState::Value == arCtx.getParserState()) {
                    arCtx.pushParserState(ParserState::ReadValue);
                }
                return arCtx.returnCode_ = ParserReturnCode::eListEnd;
            case '>': // end of map.
                // After reading the value we have to remove 2 states from the stack.
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop ReadValue
                arCtx.popParserState("Unexpected token.", tkn_._lineNr); // pop Value
                arCtx.popParserState("Unexpected closing parentheses.", tkn_._lineNr);
                // At this point there are 2 options:
                // If this list was the value of a simple assignment statement, a ';' is expected.
                // If the list was part of another list, the list processing shall continue.
                if (ParserState::Value == arCtx.getParserState()) {
                    arCtx.pushParserState(ParserState::ReadValue);
                }
                return arCtx.returnCode_ = ParserReturnCode::eMapEnd;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
            }
            break;
        // <<< ReadValue
        default:
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected parser state.'" << static_cast<char>(arCtx.getParserState()) << "' at line " << tkn_._lineNr));
        } //switch parser state stack
    } //while
    return arCtx.returnCode_ = ParserReturnCode::eEndOfStream;
}

} //namespace rOops
