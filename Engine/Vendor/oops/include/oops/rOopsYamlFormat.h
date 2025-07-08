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
Yaml format class.
*/


#include <oops/rTextFormat.h>
#include <oops/rPropTypeDef.h>
#include <oops/rPropTypeInfo.h>
#include <string>
#include <stack>


namespace rOops
{

class rOopsYamlFormat : public rTextFormat
{
public:
    /**
     *
     * @param arStrm
     * @param protocol 'Y'-standard yaml or 'F'-for flow style.
     */
    explicit rOopsYamlFormat(ostream_type& arStrm, std::uint8_t arProtocol='Y', const OopsConfig& arConfig=globalConfig());
    /// Protocol version is not used in yaml format.
    void writeProtocolVersion() {}
    void writeSizeTypeAndName(bool aBeforeEndOfLine=false);
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
    void writeValueBegin();
    void writeValueEnd();
protected:
    // State:
    unsigned inBlock_{ 0 }; ///< writing compound data, between {}
}; //class rOopsYamlFormat


} //namespace rOops

