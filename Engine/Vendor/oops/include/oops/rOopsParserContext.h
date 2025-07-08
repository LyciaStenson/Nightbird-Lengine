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
Parser Context class.
*/

#include <oops/rPropLog.h>
#include <algorithm>
#include <array>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace rOops
{

enum class ParserReturnCode : uint8_t
{
    // 0x01 .. 0x0F - type id
    eNone      = 0x00,
    eSignedInt = 0x01, // int8, 16, 32, 64, ... --> size 1, 2, 4, 8, ... (The format can handle longer integers up tp 127 bytes.)
    eUnsigned  = 0x02, // uint8, 16, 32, 64, ... --> size 1, 2, 4, 8, ...
    eReal      = 0x03, // float, double, long double --> size 4, 8, 10
    eString    = 0x04, // size stores the length of the string.
    eValue     = 0x05, // size stores the size of the value.
    ePointer   = 0x06, // Pointer, the address of an object. When the pointer is NULL, the size control byte is 0, anyway it is the size of the address.
    eBool      = 0x07, // Size control byte stores the value. 0 - false, 1 - true
    eEnum      = 0x08, // Size control byte stores the size of the underlying integer type. Enum value is saved as the enum's underlying type.
    eChar      = 0x09,
    // Markers:
    eBlockBgn = 0x0A,
    eBlockEnd = 0x0B,
    eListBgn  = 0x0C,
    eListEnd  = 0x0D,
    eMapBgn   = 0x0E,
    eMapEnd   = 0x0F,
    eTopLevel = 0x10,
    eEndOfStream = 0x11,
};

std::string to_string(ParserReturnCode aControl);

enum class ParserState : char
{
    TopLevel = 'T',
    Block = 'B',
    List  = 'L',
    Map   = 'M',
    Value = 'V',
    ReadValue = 'R', // TODO: Rename or remove. Do we need it?
    Size  = 'S',
    FlowBlock = 'b',
    FlowList = 'l',
    FlowValue = 'v',
};

/**
 * Fixed string class for storing address.
 * Both text and binary addresses are stored in this class.
 * Address is just a label, byte order, length, and the actual value are irrelevant.
 * Empty or NULL if size is 0.
 * Capacity is 2 * sizeof(std::uintptr_t) for being able to store the address in decimal or octal version.
 * Text formats can use any arbitrary string label in place of the address,
 * but only the characters up the capacity is stored and used in comparison.
 */
class rOopsAddressString
{
public:
    static constexpr std::size_t cCapacity = std::max(std::size_t(16u), 2*sizeof(std::uintptr_t));
    void clear() { size_ = 0; }
    std::size_t empty() const { return 0 == size_; }
    std::size_t size() const { return size_; }
    /// Read address from the binary stream.
    /// Endianness is irrelevant. It is only used to compare them as labels, the actual value of the address in not used.
    bool read(std::istream& arStream, std::size_t aSize);
    rOopsAddressString& operator=(const std::string& arStr);
    bool operator==(const rOopsAddressString& arR) const;
    char operator[](std::size_t aIdx) const
    {
        return data_[aIdx];
    }
    /**
     * Get the underlying data as a string.
     * This function assumes, that data_ stores a string used as an address label.
     * @return the string copied from \c data_.
     */
    std::string asString() const;
    /**
     * Write the address stored in data to the given ostream.
     * This function assumes, that data_ stores a binary address.
     * @param arStream
     * @param arAddress
     * @return
     */
    friend inline std::ostream& operator<<(std::ostream& arStream, const rOopsAddressString& arAddress);
private:
    std::size_t size_ { 0 };
    std::array<char, cCapacity> data_{};
};

struct rOopsAddressStringHash
{
    std::size_t operator() (const rOopsAddressString& key) const {
        std::hash<char> hasher;
        std::size_t result = 144451;
        for(size_t idx = 0; idx < rOopsAddressString::cCapacity; ++idx) {
            result = (result << 1) ^ hasher(key[idx]);
        }
        return result;
    }
};

inline std::ostream& operator<<(std::ostream& arStream, const rOopsAddressString& arAddress)
{
    arStream << *reinterpret_cast<const std::uintptr_t*>(&arAddress.data_[0]);
    return arStream;
}

class rOopsParserContext
{
public:
    rOopsParserContext()
    {
        typeName_.reserve(256);
        propName_.reserve(256);
        stateStack_.push(ParserState::TopLevel);
    }
    void changeParserState(ParserState aFrom, ParserState aTo)
    {
        if (stateStack_.empty()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("changeParserState() is called on empty stack."));
        }
        const ParserState current_state = stateStack_.top();
        if (current_state != aFrom && current_state != aTo) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Parser state (" << static_cast<char>(current_state) << ") is not " << static_cast<char>(aFrom) << " nor " << static_cast<char>(aTo) << "."));
        }
        stateStack_.pop();
        stateStack_.push(aTo);
    }
    void pushParserState(ParserState aState)
    {
        stateStack_.push(aState);
    }
    ParserState popParserState()
    {
        ParserState ret_val = stateStack_.top();
        if (stateStack_.empty() || ParserState::TopLevel == ret_val) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Program error. stateStack should not be empty."));
        }
        stateStack_.pop();
        return ret_val;
    }
    ParserState popParserState(const char* aErrorMsg)
    {
        ParserState ret_val = stateStack_.top();
        if (stateStack_.empty() || ParserState::TopLevel == ret_val) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(aErrorMsg));
        }
        stateStack_.pop();
        return ret_val;
    }
    ParserState popParserState(const std::string& aErrorMsg, std::size_t aLineNr)
    {
        ParserState ret_val = stateStack_.top();
        if (stateStack_.empty() || ParserState::TopLevel == ret_val) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(aErrorMsg << " Line nr: " << aLineNr));
        }
        stateStack_.pop();
        return ret_val;
    }
    bool isParserStateEmpty() const
    {
        return (stateStack_.size() <= 1);
    }
    ParserState getParserState() const
    {
        if (stateStack_.empty()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Parser state stack is empty."));
        }
        return stateStack_.top();
    }
    bool isBlock() const
    {
        return ParserState::Block == stateStack_.top() || ParserState::FlowBlock == stateStack_.top() || ParserState::TopLevel == stateStack_.top();
    }
    bool isList() const
    {
        return ParserState::List == stateStack_.top() || ParserState::FlowList == stateStack_.top();
    }
    bool isMap() const
    {
        return ParserState::Map == stateStack_.top();
    }
    void clear()
    {
        returnCode_ = ParserReturnCode::eNone;
        size_ = 1u;
        typeName_.clear();
        version_ = 0u;
        address_.clear();
        propName_.clear();
        value_.clear();
        pointer_.clear();
        comment_.clear();
    }

public:
    ParserReturnCode returnCode_{ ParserReturnCode::eNone };
    std::uint64_t size_{ 1u };
    std::string typeName_;
    std::uint32_t version_{ 0u };
    rOopsAddressString address_; ///< Both text and binary parsers are store the address here.
    std::string propName_;
    std::string value_;
    rOopsAddressString pointer_;
    // Meta-data:
    std::vector<std::string> comment_;
    std::uint8_t versionMajor{ 0u };
    std::uint8_t versionMinor{ 0u };
private:
    std::stack<ParserState> stateStack_;
};

} //namespace rOops
