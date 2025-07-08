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

#include <oops/rOopsBinaryFormat.h>
#include <oops/rPropDescriptor.h>

namespace rOops
{

std::string to_string(ControlByte aControl)
{
    switch (aControl) {
        case ControlByte::eNone     : return "eNone";
        case ControlByte::eSignedInt: return "eSignedInt";
        case ControlByte::eUnsigned : return "eUnsigned";
        case ControlByte::eReal     : return "eReal";
        case ControlByte::eString   : return "eString";
        case ControlByte::eValue    : return "eValue";
        case ControlByte::ePointer  : return "ePointer";
        case ControlByte::eBool     : return "eBool";
        case ControlByte::eEnum     : return "eEnum";
        case ControlByte::eChar     : return "eChar";
        case ControlByte::eBlockBgn : return "eBlockBegin";
        case ControlByte::eBlockEnd : return "eBlockEnd";
        case ControlByte::eListBgn  : return "eListBegin";
        case ControlByte::eListEnd  : return "eListEnd";
        case ControlByte::eMapBgn   : return "eMapBegin";
        case ControlByte::eMapEnd   : return "eMapEnd";
    }
    return "???";
}

rOopsBinaryFormat::rOopsBinaryFormat( ostream_type& arStrm, std::uint8_t protocol, const OopsConfig& arConfig)
: rBinaryFormat(arStrm, arConfig)
, protocolMarker_(protocol)
{
    // metaDataUpperBits_ is initialized to cFlagMetaData.
    // Check integer endianness.
    if (!isIntegerLittleEndian()) {
        metaDataUpperBits_ |= cFlagIntegerEndian;
    }
    if (!isFloatLittleEndian()) {
        metaDataUpperBits_ |= cFlagFloatEndian;
        if (isFloatLittleEndian() != isDoubleLittleEndian()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Endianness of float and double types are different."));
        }
    }
}

void rOopsBinaryFormat::setName(const std::string& arPropName)
{
    propName_ = arPropName;
}

void rOopsBinaryFormat::setType(const rPropTypeInfo* apTI, OopsConfig::WriteTypeName aWriteTypeNameAbove, std::size_t nrOfElements)
{
    typeInfo_ = apTI;
    writeTypeNameAbove_ = aWriteTypeNameAbove;
    size_ = nrOfElements;
}

void rOopsBinaryFormat::setTopLevel(const rPropTypeInfo* apTI, const std::string& arPropName)
{
    setName(arPropName);
    setType(apTI, OopsConfig::WriteTypeName::eNever, 1);
}

void rOopsBinaryFormat::setPointedObject(const rPropTypeInfo* apTI, const void* apObjAddr)
{
    propName_ = "*"; // gcc v12 cannot compile this in release build due to a compiler bug.
    setAddress(apObjAddr);
    setType(apTI, OopsConfig::WriteTypeName::eNever, 1);
}

void rOopsBinaryFormat::writeProtocolVersion()
{
    write(protocolMarker_);
    // Local variable is needed because the result of the expression is 4 bytes long.
    uint8_t ctrl_byte = cFlagMetaData | enum2integral(ControlByteMetaData::eProtocolVersion);
    write(ctrl_byte);
    write(cOopsBinaryFormatVersionMajor);
    write(cOopsBinaryFormatVersionMinor);
}

void rOopsBinaryFormat::writeComment(const std::string& arComment)
{
    write(protocolMarker_);
    // Local variable is needed because the result of the expression is 4 bytes long.
    uint8_t ctrl_byte = cFlagMetaData | enum2integral(ControlByteMetaData::eComment);
    write(ctrl_byte);
    writeSize(arComment.size());
    write(arComment);
}

void rOopsBinaryFormat::writePointer(const void* apObjAddr)
{
    if (nullptr == apObjAddr) {
        // Null pointer.
        setSize(0);
        writeCtrlSizeTypeName(enum2integral(ControlByte::ePointer));
    }
    else {
        // Value
        auto addr = reinterpret_cast<std::uintptr_t>(apObjAddr);
        setSize(sizeof(addr));
        writeCtrlSizeTypeName(enum2integral(ControlByte::ePointer));
        write(&addr, sizeof(addr));
    }
}

void
rOopsBinaryFormat::writeBlockBegin()
{
    writeCtrlSizeTypeName(enum2integral(ControlByte::eBlockBgn));
    ++inBlock_;
}

void rOopsBinaryFormat::writeBlockEnd()
{
    if (0 < inBlock_) {
        --inBlock_;
        write(protocolMarker_);
        write(enum2integral(ControlByte::eBlockEnd));
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of block."));
    }
}

void
rOopsBinaryFormat::writeListBegin()
{
    writeCtrlSizeTypeName(enum2integral(ControlByte::eListBgn));
    ++inList_;
}

void rOopsBinaryFormat::writeListEnd()
{
    if (0 < inList_) {
        --inList_;
        write(protocolMarker_);
        write(enum2integral(ControlByte::eListEnd));
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of list."));
    }
}

void
rOopsBinaryFormat::writeMapBegin()
{
    writeCtrlSizeTypeName(enum2integral(ControlByte::eMapBgn));
    ++inMap_;
}

void rOopsBinaryFormat::writeMapEnd()
{
    if (0 < inMap_) {
        --inMap_;
        write(protocolMarker_);
        write(enum2integral(ControlByte::eMapEnd));
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of map."));
    }
}

void rOopsBinaryFormat::doIndent()
{
    // Nothing to do in binary format.
}

void rOopsBinaryFormat::writeValueSeparator(bool /*aEndOf*/)
{
}


void rOopsBinaryFormat::writeListSeparator(bool /*aEndOf*/)
{
}


void rOopsBinaryFormat::writeKeyValueSeparator()
{
}

void rOopsBinaryFormat::writeSize(std::uint64_t valueSize)
{
    if (valueSize < 0x80) {
        // 7 bits is enough for storing the size.
        write(static_cast<std::uint8_t>(valueSize));
    }
    else if (valueSize < 0x10000) {
        write(cSizeControl+sizeof(std::uint16_t));
        write(static_cast<std::uint16_t>(valueSize));
    }
    else if (valueSize < 0x100000000) {
        write(cSizeControl+sizeof(std::uint32_t));
        write(static_cast<std::uint32_t>(valueSize));
    }
    else {
        write(cSizeControl+sizeof(std::uint64_t));
        write(valueSize);
    }
}

void rOopsBinaryFormat::writeCtrlSizeTypeName(uint8_t ctrl)
{
    std::uint8_t version = 0; // Binary format stores version number on 1 byte only.
    if (writeTypeNameAbove_<=config_.writeTypeNameLevel) {
        if (nullptr != typeInfo_) {
            // Type is given by rPropTypeInfo.
            if (std::numeric_limits<std::uint8_t>::max() < typeInfo_->version()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Version number '" << typeInfo_->version() << "' is out od the supported range [0..255]."));
            }
            version = static_cast<std::uint8_t>(typeInfo_->version());
            ctrl |= cFlagType;
        } else if (!typeName_.empty()) {
            // Type is given by type name.
            ctrl |= cFlagType;
        }
    }
    if (0u != version) ctrl |= cFlagVersion;
    if (0u != objAddress_) ctrl |= cFlagAddress;
    write(protocolMarker_);
    write(ctrl);
    // Size or number of elements.
    writeSize(size_);
    // Write type name if needed.
    if (0 != (ctrl & cFlagType)) {
        if (nullptr!=typeInfo_) {
            writeSize(typeInfo_->typeName().size());
            write(typeInfo_->typeName());
        }
        else if (!typeName_.empty()) {
            // Type is given by name.
            writeSize(typeName_.size());
            write(typeName_);
        }
        else {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Both typeInfo_ and typeName_ are empty."));
        }
    }
    // Write version.
    if (0u != version) write(version);
    // Write object address.
    if (0u != objAddress_) {
        writeSize(sizeof(objAddress_)); // Different platforms may have different pointer size.
        write(objAddress_);
    }
    // Name id
    if (!propName_.empty()) {
        writeSize(propName_.size());
        write(propName_);
    }
    objAddress_ = 0u;
    propName_.clear();
}

} //namespace rOops
