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
Yaml format parser.
*/

#include <oops/rOopsYamlParser.h>

namespace
{
    /**
    * Remove type bounding characters of the yaml format, if they are around the type name,
    * and parse the type name, version, and address into the context.
    */
    void parseTypeName(rOops::rOopsParserContext& arCtx, const std::string& arToken)
    {
        auto len = arToken.length();
        if (2 > len) {
            arCtx.typeName_ = arToken;
        }
        auto addr_pos = arToken.find('&');
        if (std::string::npos == addr_pos) {
            if('<'==arToken[0] && '>'==arToken[len-1]) {
                arCtx.typeName_ = arToken.substr(1, len-2);
            }
            else {
                arCtx.typeName_ = arToken;
            }
        }
        else {
            if('<'==arToken[0] && '>'==arToken[len-1]) {
                arCtx.typeName_ = arToken.substr(1, len-2);
                arCtx.typeName_ = arToken.substr(1, addr_pos);
                arCtx.address_ = arToken.substr(addr_pos+1,len-2);
            }
            else {
                arCtx.typeName_ = arToken.substr(0, addr_pos);
                arCtx.address_ = arToken.substr(addr_pos+1);
            }
        }
        auto version_pos = arCtx.typeName_.find('/');
        if (std::string::npos != version_pos) {
            arCtx.version_ = rOops::string_to<std::uint32_t>(arToken.substr(version_pos+1));
        }
    }
}

namespace rOops
{

/**
handleNewLine() recognize end of block or list.
List and structure has to be handled on a different way.
List should depend on the indent saved with the Parser State.
Struct should depend on the decreasing indentation.
*/
bool rOopsYamlParser::handleNewLine(rOopsParserContext& arCtx)
{
    std::size_t ident_str_len = tkn_._str.length();
    if (ident_str_len > indentation_.back().indent) {
        indentation_.push_back( {arCtx.getParserState(), ident_str_len} );
        return true;
    }
    tkn_._token = '\r';
    return false;
} //handleNewLine()

ParserReturnCode rOopsYamlParser::parse(rOopsParserContext& arCtx)
{
    arCtx.clear();
    try
    {
        while (true) {
            // Handle new line: multiple states may be closed with a single new line character.
            switch (tkn_._token) {
            //case '#':
            //    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected comment."));
            //    //continue; // Comment is skipped.
            case '\0':
                // End of input stream.
                // Pop all states from the stack, and close all pending blocks.
                // Leave the initial Structure state in the stack.
                if (ParserState::TopLevel != arCtx.getParserState()) {
                    // Check the state, and call block or list end.
                    switch (arCtx.getParserState()) {
                    case ParserState::Block:
                        arCtx.popParserState();
                        return arCtx.returnCode_ = ParserReturnCode::eBlockEnd;
                    case ParserState::List:
                        arCtx.popParserState();
                        return arCtx.returnCode_ = ParserReturnCode::eListEnd;
                    case ParserState::Map:
                        arCtx.popParserState();
                        return arCtx.returnCode_ = ParserReturnCode::eMapEnd;
                    default:
                        arCtx.popParserState();
                        break;
                    } //switch
                } //if
                return arCtx.returnCode_ = ParserReturnCode::eEndOfStream;
            case '\r': {
                // Handle new line. Multiple parser states are close with the same new-line character.
                std::size_t ident_str_len = tkn_._str.length();
                if (!arCtx.isParserStateEmpty() && 1 < indentation_.size()) {
                    switch (arCtx.getParserState()) {
                    case ParserState::Block:
                        // The indentation value saved with the status is not always correct,
                        // because it is not know in struct in list situation,
                        // how much the indentation of the next line is.
                        if (ident_str_len < indentation_.back().indent) {
                            indentation_.pop_back();
                            arCtx.popParserState();
                            return arCtx.returnCode_ = ParserReturnCode::eBlockEnd;
                        }
                        break;
                    case ParserState::List:
                        if (ident_str_len < indentation_.back().indent) {
                            // Collection is not always pushed to tabulation. Check if it is not a block instead of checking if it is list.
                            // if (ParserState::Collection == indentation_.back().state) {
                            if (ParserState::Block != indentation_.back().state) {
                                // List might not be indented.
                                indentation_.pop_back();
                            }
                            arCtx.popParserState();
                            return arCtx.returnCode_ = ParserReturnCode::eListEnd;
                        }
                        if (ident_str_len == indentation_.back().indent) {
                            // Indentation is equal: may be the end of a list, if next token is not list element ('-').
                            if (ParserState::List == arCtx.getParserState() && '-' != next_tkn_._token) {
                                arCtx.popParserState();
                                return arCtx.returnCode_ = ParserReturnCode::eListEnd;
                            }
                        }
                        break;
                    default:
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected parser state."));
                    } //switch
                } //if
                // End of new line, all parser states are popped up.
                // Continue parsing.
                tkn_._token = '\n';
            } break;
            }

            if ('\r'==tkn_._token) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("End-of-line state. It should not come to this point.."));
            }
            if ('\0'==tkn_._token) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("End-of-file state. It should not come to this point."));
            }
            // Get next token with one token look ahead.
            if (!skipGetToken_) {
                tkn_ = std::move(next_tkn_);
                next_tkn_ = tokenizer_.getNextToken();
            }
            skipGetToken_ = false;
            // Handle empty lines. If both tkn and next_tkn are new lines, drop tkn.
            if ('\n'==tkn_._token && '\n'==next_tkn_._token) {
                continue;
            }
            // for all parser states:
            if ('#' == tkn_._token) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected comment."));
            }

            switch (arCtx.getParserState()) {
            case ParserState::ReadValue:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Program error."));
            case ParserState::TopLevel:
                switch (tkn_._token) {
                    case 'S':
                    case '\'':
                    case '"':
                        arCtx.propName_ = tkn_._str;
                        break;
                    case '*':
                        arCtx.address_ = tkn_._str;
                        break;
                    case ':':
                        arCtx.pushParserState(ParserState::Value);
                        break;
                    case '\n':
                        handleNewLine(arCtx);
                        break;
                    case '\0':
                        return arCtx.returnCode_ = ParserReturnCode::eEndOfStream;
                    default:
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr << "."));
                }
                break;
            case ParserState::Block:
                switch (tkn_._token) {
                case 'S':
                case '\'':
                case '"':
                    arCtx.propName_ = tkn_._str;
                    break;
                case '*':
                    arCtx.address_ = tkn_._str;
                    break;
                case '[':
                    arCtx.pushParserState(ParserState::Size);
                    break;
                case ':':
                    arCtx.pushParserState(ParserState::Value);
                    break;
                case '-':
                    arCtx.pushParserState(ParserState::List);
                    break;
                case '\n':
                    handleNewLine(arCtx);
                    break;
                case '\0': // One more BlockEnd may be in the stack.
                case ',': // Skip flow mode separator here.
                    break;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr << "."));
                }
                break;
            case ParserState::FlowBlock:
                switch (tkn_._token) {
                case '!':
                    parseTypeName(arCtx, tkn_._str);
                    break;
                case 'S':
                case '\'':
                case '"':
                    arCtx.propName_ = tkn_._str;
                    break;
                case '*':
                    arCtx.address_ = tkn_._str;
                    break;
                case '{':
                    arCtx.pushParserState(ParserState::FlowBlock);
                    break;
                case ':':
                    arCtx.pushParserState(ParserState::FlowValue);
                    break;
                case '[':
                    arCtx.pushParserState(ParserState::FlowList);
                    break;
                case '}':
                    arCtx.popParserState();
                    return arCtx.returnCode_ = ParserReturnCode::eBlockEnd;
                case ',': // Next value ...
                case '\n': // skip
                    break;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr << "."));
                }
                break;
            case ParserState::Size:
                switch (tkn_._token) {
                case 'S':
                    arCtx.size_ = static_cast<std::size_t>(std::stoll(tkn_._str));
                    break;
                case ']':
                    arCtx.popParserState();
                    break;
                }
                break;
            case ParserState::Value:
                // Going to value state when ': ' is found, but depending on the next token, it may be a list or a structure.
                switch (tkn_._token) {
                case '!':
                    parseTypeName(arCtx, tkn_._str);
                    break;
                case '\'':
                case '"':
                case 'S':
                case '(':
                    arCtx.popParserState();
                    arCtx.value_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::eValue;
                case '*':
                    arCtx.popParserState();
                    arCtx.pointer_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::ePointer;
                case '{':
                    arCtx.changeParserState(ParserState::Value, ParserState::FlowBlock);
                    return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                case '[':
                    arCtx.changeParserState(ParserState::Value, ParserState::FlowList);
                    return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                case '\n':
                    if (tkn_._str.length()<=indentation_.back().indent) {
                        // Handle empty value, when key:\n is not the beginning of a dictionary nor the beginning of a list.
                        arCtx.popParserState();
                        if ('-' == next_tkn_._token && tkn_._str.length()>=indentation_.back().indent) {
                            arCtx.pushParserState(ParserState::List);
                            return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                        }
                        else {
                            tkn_._token = '\r';
                            return arCtx.returnCode_ = ParserReturnCode::eValue;
                        }
                    }
                    if (handleNewLine(arCtx)) {
                        switch (next_tkn_._token) {
                        case 'S':
                        case '\'':
                        case '"':
                        case '(':
                            arCtx.changeParserState(ParserState::Value, ParserState::Block);
                            return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                        case '-':
                            arCtx.changeParserState(ParserState::Value, ParserState::List);
                            return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                        }
                    }
                    break;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
                }
                break;
            case ParserState::FlowValue:
                // Going to value state when ': ' is found, but depending on the next token, it may be a list or a structure.
                switch (tkn_._token) {
                case '!':
                    parseTypeName(arCtx, tkn_._str);
                    break;
                case '\'':
                case '"':
                case 'S':
                case '(':
                    arCtx.popParserState();
                    arCtx.value_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::eValue;
                case '*':
                    arCtx.popParserState();
                    arCtx.pointer_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::ePointer;
                case '{':
                    arCtx.changeParserState(ParserState::FlowValue, ParserState::FlowBlock);
                    return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                case '[':
                    arCtx.changeParserState(ParserState::FlowValue, ParserState::FlowList);
                    return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                case ',':
                    break;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' at line " << tkn_._lineNr));
                }
                break;
            case ParserState::List:
            case ParserState::Map:
                switch (tkn_._token) {
                case '!':
                    parseTypeName(arCtx, tkn_._str);
                    break;
                case 'S':
                case '\'':
                case '"':
                case '(':
                    // Right after the '-' token, we do not know yet if the token is a simple value or the first key of a map.
                    //--- arCtx.typeName_ = arCtx.propName_ = "";
                    //--- arCtx.size_ = 1;
                    switch (next_tkn_._token) {
                    case ':':
                        // The previous token was the key of a map.
                        if (arCtx.isMap()) {
                            // Store value as the key of the key-value pair.
                            // Note: only base types are accepted here.
                            // TODO: Implement compound and list keys.
                            arCtx.pushParserState(ParserState::Value);
                            return arCtx.returnCode_ = ParserReturnCode::eValue;
                        }
                        else {
                            // tkn_ already contains the value, but Block Begin has to be returned first.
                            arCtx.pushParserState(ParserState::Block); // The indentation value saved with the status is wrong here, but we do not know yet, how much the indentation of the next line is.
                            skipGetToken_ = true;
                            return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                        }
                        break;
                    case '\n':
                        // The previous token was a simple value or address.
                        arCtx.value_ = tkn_._str;
                        return rOops::ParserReturnCode::eValue;

                    default:
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected '" << tkn_.to_string() << "' after list value at line " << tkn_._lineNr << "."));
                    }
                    break;
                case '*':
                    arCtx.pointer_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::ePointer;
                case '-':
                    break;
                case '\n':
                    if (handleNewLine(arCtx)) {
                        switch (next_tkn_._token) {
                            case '-':
                                arCtx.pushParserState(ParserState::List);
                                return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                            default:
                                arCtx.pushParserState(ParserState::Block);
                                return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                        }
                    }
                    break;
                } //switch token
                break;
            case ParserState::FlowList:
                switch (tkn_._token) {
                case '!':
                    arCtx.typeName_ = tkn_._str;
                    break;
                case '\'':
                case '"':
                case 'S':
                case '(':
                    arCtx.value_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::eValue;
                case '*':
                    arCtx.pointer_ = tkn_._str;
                    return arCtx.returnCode_ = ParserReturnCode::ePointer;
                case ',':
                    break;
                case ']':
                    arCtx.popParserState(); // TODO: ??? was after cbListEnd()
                    return arCtx.returnCode_ = ParserReturnCode::eListEnd;
                case '{':
                    arCtx.pushParserState(ParserState::FlowBlock);
                    return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
                case '[':
                    arCtx.pushParserState(ParserState::FlowList);
                    return arCtx.returnCode_ = ParserReturnCode::eListBgn;
                } //switch token
                break;
            } //switch parser state
        } //while
    } //try
    catch (const std::runtime_error& arEx)
    {
        // TODO: Because of the next token already processed by the tokenizer_,
        // TODO: where() may return the wrong file name, if it just returned from an include file.
        // TODO: This is unlikely to happen: try to make a test-case for it.
        // TODO: Solution: token should carry the file name, but it is too 'heavy' for the token.
        throw std::runtime_error(tokenizer_.where(tkn_._lineNr) + arEx.what());
    } //catch
    catch (...)
    {
        throw std::runtime_error(tokenizer_.where(tkn_._lineNr) + "Unknown exception.");
    } //catch
    return ParserReturnCode::eEndOfStream;
}

} //namespace rOops

