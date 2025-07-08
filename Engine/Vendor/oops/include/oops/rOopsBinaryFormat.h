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

#include <oops/rBinaryFormat.h>
#include <oops/rPropTypeDef.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rOopsParserContext.h>
#include <string>

/**
 * Oops Binary File / Stream format
 * 
 * The Oops Binary format is designed to be stateless.
 * It does not have any file header, and does not contain any status information.
 * Top level items are independent of each other, and can be loaded independently.
 * (There is only one limitation: complex data structures must be ended.)
 * 
 * The binary stream contains a list of items.
 * Every item starts with 3 bytes: the Marker, the Control Byte (CtrlByte), and the Size Control Byte (SizeCtrl).
 *
 * Items are atomic data values with some meta data (size, type, version, address)
 * or complex data structure: block (C++ struct or class), list, map.
 * 
 * The Marker byte starts every item. It stores the protocol version and describes the endianness as a single letter.
 * Currently only N (storing member and type names as string) are supported.
 *
 * The Size Control Byte describes how to store the size.
 * When its highest bit is set the SizeCtrl byte is followed with the size on several bytes.
 * The number of bytes stores the size is described on the lower bits of the SizeCtrl byte.
 * This means, that the size is written as 1 byte up to 127: the highest bit of the SizeCtrl byte is 0.
 * Above of that the highest bit of the SizeCtrl byte is 1 and the other 7 bits stores
 * the number of bytes used to store the size on the following bytes: 0x82, 0x84, 0x88.
 * Other values are allowed by the format, but currently not supported and does not make sense.
 * The size of the type and member names are written the same way.
 * 
 * There are 3 exceptional usage of the SizeCtrl byte:
 *   - when the value of any atomic type is 0 or nullptr or empty, the SizeCtrl byte is set to 0,
 *     and no further size and value is written.
 *     This is optional, the 0 value can be written to the place of the value as well.
 *   - In case of boolean type the value is written into the SizeCtrl byte: 0 - false, 1 - true.
 *   - The SizeCtrl byte is not written after the end markers (eBlockEnd, eListEnd, eMapEnd).
 *     It has no meaning in that case. In fact, nothing is written after the end markers,
 *     because type, name, version, address, size is never assigned to end markers.
 * 
 * Type name is written when the Type Flag (0x40) is set in the CtrlByte.
 * First its size, followed with the type name characters.
 * The size of the type name uses the same size format: SizeCtrl byte, followed with the required number of bytes for storing the size.
 * 
 * Version is stored as an unsigned byte, when the Version Flag (0x20) is set in the CtrlByte.
 * 
 * The address of the object is stored when the Address Flag (0x10) is set in the CtrlByte.
 * The address is stored on 8 bytes and used to resolve pointer references.
 *
 * Complex data structures are described with begin and end markers.
 * The begin markers store the same meta data (size, type, version, address) as the atomic data.
 *
 * Block stores name and value pairs, and size is not used, but the Size Control byte is saved.
 * The meta information of Block items are extended with name stored as a size and character array : SizeCtrl, [Size], <characters of the name>.
 *
 * List stores the list of values, size stores the number of items in the list.
 * 
 * Map stores the list of key value pairs, where both can be any atomic or complex data.
 * Size also stores the number of items in the map.
 *
 * The Control byte describes the value type, and stores some control flags.
 * The size describes the size of the type, but its meaning depends on the type.
 *
 * Base types (e.g. int, float) only stored using 3 mandatory bytes and the value:
 *     Marker, CtrlByte, SizeCtrlByte, Value
 * Size in this case always fits into 7 bits, and describes the size of the int or real value (short, int, long, float, double).
 * The boolean type and the 0, 0.0, nullptr values are special: the value is stored in the SizeCtrl byte: 1 - true, 0 - false, 0, 0.0, nullptr.
 * 
 * Other values are written as:
 *     Marker, CtrlByte(eValue), SizeControlByte, Size, [TypeId/TypeName], [Version], [Address], Value
 *
 * Blocks, lists and maps are started and terminated with markers.
 *     Marker, CtrlByte(eBlockBegin), SizeCtrl, [Type], [Version], [Address] <... list of members (name, value) ...> CtrlByte(eBlockEnd)
 *     Marker, CtrlByte(eListBegin), SizeCtrl, Size, TypeId, <... list of elements ...> CtrlByte(eListEnd)
 *     Marker, CtrlByte(eMapBegin), SizeCtrl, Size, TypeId, <... list of key - value pairs ...> CtrlByte(eMapEnd)
 * Size stores the number of elements in case of list and map.
 */

namespace rOops
{
    /**
     * Protocol Version number.
     */
    constexpr uint8_t cOopsBinaryFormatVersionMajor = 1;
    constexpr uint8_t cOopsBinaryFormatVersionMinor = 0;

    enum class ControlByte : uint8_t
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
    };

    std::string to_string(ControlByte aControl);


    /**
     * The higher order bit describes if this control byte is an extension and this items stores some meta data.
     * The other 3 bits of the upper 4 bits of the Control Byte are flags for describing
     * if the version, type id, and object address are saved with the value.
     * Any of these fields are optional, and only exists if the appropriate control flag is set.
     */
    constexpr uint8_t cFlagMetaData = 0x80; ///< This flag is 0 for data control bytes. It is set to 1, when a special control byte is sent and the meaning of the other bits are different.
    // Control Byte flags and masks for data control bytes:
    constexpr uint8_t cFlagType     = 0x40; ///< Set this flag, when type id is written after the size control byte.
    constexpr uint8_t cFlagVersion  = 0x20; ///< Set this flag when version number is written.
    constexpr uint8_t cFlagAddress  = 0x10; ///< Set this flag, when the address of the object is saved as an id, which is used to resolve references.
    constexpr uint8_t cControlByteMask = 0x0F;
    // Control Byte flags and masks for data control bytes:
    constexpr uint8_t cFlagIntegerEndian   = 0x40; ///< This flag is set, when integer data is sent using big endian byte order, and it is 0 for little endian byte order.
    constexpr uint8_t cFlagFloatEndian     = 0x20; ///< This flag is set, when floating point data is sent using big endian byte order, and it is 0 for little endian byte order.
    enum class ControlByteMetaData : uint8_t {
        eNone = 0x00,
        eProtocolVersion = 0x01, ///< This flag is set, if Oops Binary Protocol Version is sent in the following 2 bytes: Major Version Number 1st byte, Minor Version Number 2nd byte.
        eComment = 0x02, ///< Comment, written as string: SizeControlByte and text.
    };
    /**
     * The Size Control Byte follows the Control Byte. Its highest bit controls if it contains the size (0), or the number of bytes storing the size (1).
     * The size cannot be stored in 1 byte, when it is more than 127. In this case the size is stored in 2, 4, or 8 bytes following the SizeControlByte.
     * The SizeControl stores the number of bytes used to store the size. (2,4,8 are the values currently used, but any size up to 127 fits into the format.)
     */
    constexpr uint8_t cSizeControl = 0x80; ///< Set this flag when size does not fits into the size control byte.

    /**
    * Helper function for returning the appropriate Control Byte for the given C++ types.
    */
    template <typename DataT> inline ControlByte type2ControlByte() { return ControlByte::eValue; }
    template <> inline ControlByte type2ControlByte<char>() { return ControlByte::eChar; }
    template <> inline ControlByte type2ControlByte<std::int8_t>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<std::int16_t>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<std::int32_t>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<std::int64_t>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<std::uint8_t>() { return ControlByte::eUnsigned; }
    template <> inline ControlByte type2ControlByte<std::uint16_t>() { return ControlByte::eUnsigned; }
    template <> inline ControlByte type2ControlByte<std::uint32_t>() { return ControlByte::eUnsigned; }
    template <> inline ControlByte type2ControlByte<std::uint64_t>() { return ControlByte::eUnsigned; }
    // The int*_t typedefs are compiler dependent.
    // Some additional definitions are required to cover all integer types,
    // but depending on the compiler and platform they are different.
    #if defined(__clang__)
    template <> inline ControlByte type2ControlByte<long long>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<unsigned long long>() { return ControlByte::eUnsigned; }
    #elif defined(__GNUC__)
    template <> inline ControlByte type2ControlByte<long long>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<unsigned long long>() { return ControlByte::eUnsigned; }
    #elif defined(_MSC_VER)
    template <> inline ControlByte type2ControlByte<long>() { return ControlByte::eSignedInt; }
    template <> inline ControlByte type2ControlByte<unsigned long>() { return ControlByte::eUnsigned; }
    #endif
    template <> inline ControlByte type2ControlByte<float>() { return ControlByte::eReal; }
    template <> inline ControlByte type2ControlByte<double>() { return ControlByte::eReal; }
    template <> inline ControlByte type2ControlByte<long double>() { return ControlByte::eReal; }
    template <> inline ControlByte type2ControlByte<std::string>() { return ControlByte::eString; }

class rOopsBinaryFormat : public rBinaryFormat
{
public:
    /**
     * Construct s format object for Oops Binary format.
     * Check and store the endianness of the CPU.
     * @param arStrm - The serialized binary data is written to this output stream.
     * @param protocol - Select which binary protocol is used: currently only 'N' is allowed.
     * @param arConfig - The config record of some parameters controlling the serialization.
     */
    explicit rOopsBinaryFormat(ostream_type& arStrm, std::uint8_t protocol='N', const OopsConfig& arConfig=globalConfig());
    void setName(const std::string& arPropName);
    //void setAddress(const void* apObjAddr);
    void setSize(std::uint64_t aSize) { size_ = aSize; }
    void setType(const rPropTypeInfo* apTI, OopsConfig::WriteTypeName aWriteTypeNameAbove, std::size_t nrOfElements);
    void setTopLevel(const rPropTypeInfo* apTI, const std::string& arPropName);
    void setPointedObject(const rPropTypeInfo* apTI, const void* apObjAddr);
    /// Protocol version is written to the stream as a meta data record.
    void writeProtocolVersion();
    void writeComment(const std::string& arComment);
    void writePointer(const void* apObjAddr);
    void writeBlockBegin();
    void writeBlockEnd();
    void writeListBegin();
    void writeListEnd();
    void writeMapBegin();
    void writeMapEnd();
    void doIndent();
    void writeValueSeparator( bool aEndOf );
    void writeListSeparator( bool aEndOf );
    void writeKeyValueSeparator();
    void writeCtrlSizeTypeName(uint8_t ctrl);
    void writeSize(std::uint64_t valueSize);
protected:
    std::uint8_t protocolMarker_; // Initialized in the constructor.
    std::uint8_t metaDataUpperBits_{ cFlagMetaData }; // Store meta data flag and endianness.
    // State:
    unsigned inBlock_{ 0 }; ///< writing compound data
    unsigned inList_{ 0 }; ///< writing list
    unsigned inMap_{ 0 }; ///< writing map
    // Context:
    //std::uint64_t objAddress_{ 0u };
    std::string typeName_;
    OopsConfig::WriteTypeName writeTypeNameAbove_{ OopsConfig::WriteTypeName::eWhenNeeded };
    std::uint64_t size_{ 0u };
}; //class rOopsBinaryFormat

} //namespace rOops
