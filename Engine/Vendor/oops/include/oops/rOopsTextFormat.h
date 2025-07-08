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

#include <oops/rTextFormat.h>
#include <oops/rPropTypeDef.h>
#include <oops/rPropTypeInfo.h>
#include <string>


namespace rOops
{

/**
 * Oops Text Format Protocol Versions:
 *    1.x - [key = value, ... ] was used for saving associative containers.
 *    2.x = <key = value, ...> is used for saving associative containers.
 */
constexpr uint32_t cOopsTextFormatVersionMajor = 2;
constexpr uint32_t cOopsTextFormatVersionMinor = 0;

class rOopsTextFormat : public rTextFormat
{
public:
    /// Constructor binds the format object to an output stream.
    explicit rOopsTextFormat(ostream_type& arStrm, std::uint8_t arProtocol='T', const OopsConfig& arConfig=globalConfig());
    /// Protocol version is written to the stream as a comment.
    void writeProtocolVersion();
    void writePointer(const void* apObjAddr);
    void writeBlockBegin();
    void writeBlockEnd();
    void writeListBegin();
    void writeListEnd();
    void writeMapBegin();
    void writeMapEnd();
    void writeValueSeparator( bool aEndOf );
    void writeListSeparator( bool aEndOf );
    void writeKeyValueSeparator();
    void writeSizeTypeAndName();
    void writeValueBegin();
    void writeValueEnd();
protected:
    // State:
    unsigned inBlock_{ 0 }; ///< writing compound data, between {}
    unsigned inCntnr_{ 0 }; ///< writing container, between []
}; //class rOopsTextFormat


} //namespace rOops
