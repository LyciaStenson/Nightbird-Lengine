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

#include <oops/rOopsTextFormat.h>
#include <oops/rPropDescriptor.h>
#include <oops/rOopsConfig.h>


namespace rOops
{


rOopsTextFormat::rOopsTextFormat( ostream_type& arStrm, std::uint8_t, const OopsConfig& arConfig)
    : rTextFormat(arStrm, 'T', arConfig)
{
}

void rOopsTextFormat::writeSizeTypeAndName()
{
    // If property name is empty, and type name is not required only the value is written!
    doIndent();
    if( !propName_.empty() ) {
        strm_ << propName_;
        if (1 < nrOfElements_) {
            strm_ << '[' << std::dec << nrOfElements_ << ']';
        }
        strm_ << " = ";
    }
    // Runtime properties does not have type name.
    if (!typeInfo_->typeName().empty()) {
        if (writeTypeNameAbove_<=config_.writeTypeNameLevel || 0<typeInfo_->version() || 0u!=objAddress_) {
            strm_ << '!' << typeInfo_->typeName();
            if (0<typeInfo_->version()) {
                strm_ << '/' << static_cast<unsigned>(typeInfo_->version()); // uint8_t is character, cast to unsigned int before writing to the stream.
            }
            if (0u != objAddress_) {
                strm_ << '&' << objAddress_;
            } 
            strm_ << ' ';
        }
    }
    propName_.clear();
    objAddress_ = 0;
    nrOfElements_ = 0;
}

void rOopsTextFormat::writeProtocolVersion()
{
    strm_ << "# Oops Text Stream v" << cOopsTextFormatVersionMajor << '.' << cOopsTextFormatVersionMinor << '\n';
}

void rOopsTextFormat::writePointer(const void* apObjAddr)
{
    writeSizeTypeAndName();
    strm_ << '*' << std::to_string(reinterpret_cast<std::uintptr_t>(apObjAddr));
}

void rOopsTextFormat::writeBlockBegin()
{
    if( !typeInfo_->isCompound() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error: writeBlockBegin() is called for non compound type!"));
    }
    writeSizeTypeAndName();
    strm_ << '{';
    ++inBlock_;
    ++indent_;
    writeEndOfLine();
}

void rOopsTextFormat::writeBlockEnd()
{
    --indent_;
    doIndent();
    if (0 < inBlock_) {
        --inBlock_;
        strm_ << '}';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of block."));
    }
}

void rOopsTextFormat::writeListBegin()
{
    if( !typeInfo_->isSTLContainer() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error! Type should be an STL container."));
    }
    writeSizeTypeAndName();
    strm_ << '[';
    ++inCntnr_;
    ++indent_;
    writeEndOfLine();
}

void rOopsTextFormat::writeListEnd()
{
    --indent_;
    doIndent();
    if (0 < inCntnr_) {
        --inCntnr_;
        strm_ << ']';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of list."));
    }
}

void
rOopsTextFormat::writeMapBegin()
{
    if( !typeInfo_->isAssociativeContainer() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error! Type should be an associative container."));
    }
    writeSizeTypeAndName();
    strm_ << '<';
    ++inCntnr_;
    ++indent_;
    writeEndOfLine();
}

void rOopsTextFormat::writeMapEnd()
{
    --indent_;
    doIndent();
    if (0 < inCntnr_) {
        --inCntnr_;
        strm_ << '>';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of map."));
    }
}

void
rOopsTextFormat::writeValueSeparator( bool )
{
    strm_ << ';';
    writeEndOfLine();
}


void
rOopsTextFormat::writeListSeparator( bool aEndOf )
{
    // Coma separated list of values.
    if( !aEndOf ) {
        strm_ << ',';
    }
    writeEndOfLine();
}


void
rOopsTextFormat::writeKeyValueSeparator()
{
    // Separator between the key and value.
    strm_ << " = ";
    doNotIdentNow_ = true;
}

void rOopsTextFormat::writeValueBegin()
{
    char separator = typeInfo_->getOpeningCharacter();
    if (separator) strm_ << separator;
}

void rOopsTextFormat::writeValueEnd()
{
    char separator = typeInfo_->getClosingCharacter();
    if (separator) strm_ << separator;
}

} //namespace rOops
