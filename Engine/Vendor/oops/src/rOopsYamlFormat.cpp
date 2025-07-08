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
YAML streaming class.
*/

#include <oops/rOopsYamlFormat.h>
#include <oops/rPropDescriptor.h>
#include <oops/rOopsConfig.h>

namespace rOops
{

rOopsYamlFormat::rOopsYamlFormat(ostream_type& arStrm, std::uint8_t arProtocol, const OopsConfig& arConfig)
    : rTextFormat(arStrm, arProtocol, arConfig)
{
}

void rOopsYamlFormat::writeSizeTypeAndName(bool aBeforeEndOfLine)
{
    doIndent();
    bool write_space = false;
    if (!propName_.empty()) {
        strm_ << propName_;
        // Do not write size, it confuses other parsers: keeping the size part of the key.
        //if (nrOfElements_) { strm_ << '[' << std::dec << nrOfElements_ << ']'; }
        strm_ << ':';
        write_space = true;
    }
    // Runtime properties does not have type name.
    if (!typeInfo_->typeName().empty()) {
        if (writeTypeNameAbove_<=config_.writeTypeNameLevel || 0<typeInfo_->version() || 0u!=objAddress_) {
            if (write_space) strm_ << ' ';
            strm_ << '!' << typeInfo_->typeName();
            if (0<typeInfo_->version()) {
                strm_ << '/' << static_cast<unsigned>(typeInfo_->version());
            }
            if (0u != objAddress_) {
                strm_ << '&' << objAddress_;
            }
            write_space = true;
        }
    }
    if (write_space && !aBeforeEndOfLine) strm_ << ' ';
    propName_.clear();
    objAddress_ = 0;
    nrOfElements_ = 0;
}

void rOopsYamlFormat::writePointer(const void* apObjAddr)
{
    writeSizeTypeAndName(false);
    // TODO: Do we need that space before *?
    strm_ << " *" << std::to_string(reinterpret_cast<std::uintptr_t>(apObjAddr));
}

void
rOopsYamlFormat::writeBlockBegin()
{
    if( !typeInfo_->isCompound() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error: writeBlockBegin() is called for non compound type!"));
    }
    writeSizeTypeAndName(true);
    if (config_.flowStyleBlock) {
        strm_ << " { ";
        ++inBlock_;
    }
    else {
        writeEndOfLine();
        ++indent_;
        ++inBlock_;
    }
}

void rOopsYamlFormat::writeBlockEnd()
{
    if (0 < inBlock_) {
        if (config_.flowStyleBlock) {
            --inBlock_;
            strm_ << "}";
        }
        else {
            --inBlock_;
            --indent_;
        }
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of block."));
    }
}

void
rOopsYamlFormat::writeListBegin()
{
    if (!typeInfo_->isSTLContainer()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("writeListBegin() is called for non list type!"));
    }
    writeSizeTypeAndName(true);
    ++inBlock_;
    if (config_.flowStyleList) {
        //_writeEndOfLine();
        //_doIndent();
        strm_ << " [ ";
    }
    else {
        ++indent_;
        writeEndOfLine();
        doIndent();
        strm_ << "- ";
    }
}

void rOopsYamlFormat::writeListEnd()
{
    --inBlock_;
    if (config_.flowStyleList) {
        strm_ << ']';
    }
    else {
        --indent_;
    }
}

void
rOopsYamlFormat::writeMapBegin()
{
    if (!typeInfo_->isAssociativeContainer()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("writeMapBegin() is called for non associative container type!"));
    }
    writeSizeTypeAndName(true);
    ++inBlock_;
    if (config_.flowStyleList) {
        strm_ << " [ ";
    }
    else {
        ++indent_;
        writeEndOfLine();
        doIndent();
        strm_ << "- ";
    }
}

void rOopsYamlFormat::writeMapEnd()
{
    --inBlock_;
    if (config_.flowStyleList) {
        strm_ << ']';
    }
    else {
        --indent_;
    }
}

void
rOopsYamlFormat::writeValueSeparator( bool aEndOf )
{
    if (config_.flowStyleBlock) {
        if (!aEndOf) strm_ << ", ";
    }
    else {
        if (!endOfLineAlreadyWritten_) {
            writeEndOfLine();
        }
    }
}


void
rOopsYamlFormat::writeListSeparator( bool aEndOf )
{
    if (config_.flowStyleList) {
        if( !aEndOf ) {
            strm_ << ", ";
        }
    }
    else {
        if (!endOfLineAlreadyWritten_) {
            writeEndOfLine();
        }
        if( !aEndOf ) {
            doIndent();
            strm_ << "- ";
        }
    }
}


void
rOopsYamlFormat::writeKeyValueSeparator()
{
    strm_ << " : ";
}

void rOopsYamlFormat::writeValueBegin()
{
    char separator = typeInfo_->getOpeningCharacter();
    if (separator) strm_ << separator;
}

void rOopsYamlFormat::writeValueEnd()
{
    char separator = typeInfo_->getClosingCharacter();
    if (separator) strm_ << separator;
}

} //namespace rOops

