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
Parser for Oops Binary file format.
*/

#include <oops/rOopsBinaryParser.h>
#include <oops/rOopsBinaryFormat.h>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace
{
    template <typename DataT>
    bool read1(std::istream& arStream, DataT& arData)
    {
        static_assert(1 == sizeof(arData), "read1() argument's size must be 1.");
        arStream.read(reinterpret_cast<char*>(&arData), 1);
        return arStream.good();
    }

    template <typename DataT>
    DataT read1(std::istream& arStream)
    {
        DataT data;
        arStream.read(reinterpret_cast<char*>(&data), sizeof(data));
        return data;
    }

    template <typename DataT>
    bool read2(std::istream& arStream, DataT& arData, bool aDoNotSwap)
    {
        static_assert(2 == sizeof(arData), "read2() argument's size must be 2.");
        if (aDoNotSwap) {
            arStream.read(static_cast<char*>(static_cast<void*>(&arData)), 2);
            return arStream.good(); // it is faster to return from here.
        }
        else {
            char* buf = static_cast<char*>(static_cast<void*>(&arData));
            arStream.read(&buf[1], 1);
            arStream.read(&buf[0], 1);
            return arStream.good();
        }
    }

    template <typename DataT>
    DataT read2(std::istream& arStream, bool aDoNotSwap)
    {
        DataT data;
        read2(arStream, data, aDoNotSwap);
        return data;
    }

    template <typename DataT>
    bool read4(std::istream& arStream, DataT& arData, bool aDoNotSwap)
    {
        static_assert(4 == sizeof(arData), "read4() argument's size must be 4.");
        if (aDoNotSwap) {
            arStream.read(static_cast<char*>(static_cast<void*>(&arData)), 4);
            return arStream.good(); // it is faster to return from here.
        }
        else {
            char* buf = static_cast<char*>(static_cast<void*>(&arData));
            arStream.read(&buf[3], 1);
            arStream.read(&buf[2], 1);
            arStream.read(&buf[1], 1);
            arStream.read(&buf[0], 1);
            return arStream.good();
        }
    }

    template <typename DataT>
    DataT read4(std::istream& arStream, bool aDoNotSwap)
    {
        DataT data;
        read4(arStream, data, aDoNotSwap);
        return data;
    }

    template <typename DataT>
    bool read8(std::istream& arStream, DataT& arData, bool aDoNotSwap)
    {
        static_assert(8 == sizeof(arData), "read8() argument's size must be 8.");
        if (aDoNotSwap) {
            arStream.read(static_cast<char*>(static_cast<void*>(&arData)), 8);
            return arStream.good(); // it is faster to return from here.
        }
        else {
            char* buf = static_cast<char*>(static_cast<void*>(&arData));
            arStream.read(&buf[7], 1);
            arStream.read(&buf[6], 1);
            arStream.read(&buf[5], 1);
            arStream.read(&buf[4], 1);
            arStream.read(&buf[3], 1);
            arStream.read(&buf[2], 1);
            arStream.read(&buf[1], 1);
            arStream.read(&buf[0], 1);
            return arStream.good(); // it is faster to return from here.
        }
    }

    /// Reads 8 byte for the print function.
    template <typename DataT>
    DataT read8(std::istream& arStream, bool aDoNotSwap)
    {
        DataT data;
        read8(arStream, data, aDoNotSwap);
        return data;
    }

    std::uint64_t readSize(std::istream& arStream, bool aDoNotSwap)
    {
        std::uint64_t size = 0;
        std::uint8_t sizeCntrlByte = 0;
        arStream.read(reinterpret_cast<char*>(&sizeCntrlByte), sizeof(sizeCntrlByte));
        if (0 == (sizeCntrlByte & rOops::cSizeControl)) {
            // 7 bits was enough for storing the size.
            size = sizeCntrlByte;
        }
        else {
            switch (sizeCntrlByte) {
                case 0x82: {
                    std::uint16_t temp_size = 0;
                    read2(arStream, temp_size, aDoNotSwap);
                    size = temp_size;
                } break;
                case 0x84: {
                    std::uint32_t temp_size = 0;
                    read4(arStream, temp_size, aDoNotSwap);
                    size = temp_size;
                } break;
                case 0x88: {
                    read8(arStream, size, aDoNotSwap);
                } break;
                default:
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("This size length is not supported: " << sizeCntrlByte));
            }
        }
        return size;
    }

    /**
     * Read function for string, type and property name.
     * @param arStream  - binary input stream.
     * @param arStrSize - size of the string, the number of characters have to be read from the stream.
     * @param arStr     - result.
     */
    void readString(std::istream& arStream, std::uint64_t aStrSize, std::string& arStr)
    {
        if (0==aStrSize) {
            arStr.clear();
        }
        else {
            arStr.assign(aStrSize, ' ');
            arStream.read(reinterpret_cast<char *>(&arStr[0]), static_cast<std::streamsize>(aStrSize));
        }
    }

    /**
     * Read function for value with unknown type (blob).
     * The function read the given number of bytes to a buffer.
     * @param arStream    - binary input stream.
     * @param arValueSize - size of the value, the number of characters have to be read from the stream.
     * @param arBuf       - result.
     */
    void readValue(std::istream& arStream, std::uint64_t aValueSize, std::string& arBuf)
    {
        if (0==aValueSize) {
            arBuf.clear();
        }
        else {
            arBuf.assign(aValueSize, ' ');
            arStream.read(reinterpret_cast<char *>(&arBuf[0]), static_cast<std::streamsize>(aValueSize));
        }
    }

    const char* endian2String(bool endianness)
    {
        return endianness ? "big" : "little";
    }
}

namespace rOops
{

void printHead(std::ostream& os, const rOops::rOopsParserContext& arCtx, std::uint8_t aMarker, std::uint8_t aCntrl)
{
    os << aMarker
       << ' '
       << ((aCntrl & cFlagType)   ?'T':'_')
       << ((aCntrl & cFlagVersion)?'V':'_')
       << ((aCntrl & cFlagAddress)?'A':'_')
       << std::hex << (aCntrl & cControlByteMask) << std::dec;
    os << " Size:" << arCtx.size_;
    if (aCntrl & cFlagType) os << " TN:" << arCtx.typeName_;
    if (aCntrl & cFlagVersion) os << " V:" << arCtx.version_;
    if (aCntrl & cFlagAddress) os << " A:" << arCtx.address_ << std::dec;
    if (!arCtx.propName_.empty()) os << " Name:" << arCtx.propName_;
}

rOopsBinaryPrinter::rOopsBinaryPrinter()
{
    bigEndianInteger_ = !isIntegerLittleEndian();
    bigEndianFloat_ = !isFloatLittleEndian();
    if (isFloatLittleEndian() != isDoubleLittleEndian()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Endianness of float and double types are different."));
    }
}

bool rOopsBinaryPrinter::processMetaData(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arCntrl)
{
    // Process meta-data
    doNotSwapInteger_ = (1==(arCntrl & rOops::cFlagIntegerEndian)) == bigEndianInteger_;
    doNotSwapFloat_   = (1==(arCntrl & rOops::cFlagFloatEndian)) == bigEndianFloat_;
    if (!doNotSwapInteger_) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(
            "Integer endianness mismatch. The input stream is written in " << endian2String(arCntrl & rOops::cFlagIntegerEndian)
            << " endian, while the system is " << endian2String(bigEndianInteger_) << " endian." ));
    }
    if (!doNotSwapFloat_) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(
            "Integer endianness mismatch. The input stream is written in " << endian2String(arCntrl & rOops::cFlagFloatEndian)
            << " endian, while the system is " << endian2String(bigEndianFloat_) << " endian." ));
    }
    auto cntrl4 = static_cast<rOops::ControlByteMetaData>(arCntrl & rOops::cControlByteMask);
    switch (cntrl4) {
    case rOops::ControlByteMetaData::eProtocolVersion:
        if (!read1(arStream, arCtx.versionMajor)) return false;
        if (!read1(arStream, arCtx.versionMinor)) return false;
        rDbgWarning("Oops Binary protocol version: " << static_cast<unsigned>(arCtx.versionMajor) << '.' << static_cast<unsigned>(rCtx.versionMinor));
        break;
    case rOops::ControlByteMetaData::eComment:
    {
        std::string comment;
        readString(arStream, readSize(arStream, doNotSwapInteger_), comment);
        arCtx.comment_.push_back(comment);
    }
    break;
    default:
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unknown Meta Data type: " << rOops::enum2integral(cntrl4)));
    }
    return true;
}

bool rOopsBinaryPrinter::processControlByte(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arCntrl) const
{
    auto cntrl4 = static_cast<rOops::ControlByte>(arCntrl & rOops::cControlByteMask);
    // Do not read size after end marker.
    if (rOops::ControlByte::eBlockEnd == cntrl4 || rOops::ControlByte::eListEnd == cntrl4 ||
        rOops::ControlByte::eMapEnd == cntrl4) {
        return true;
    }
    arCtx.size_ = readSize(arStream, doNotSwapInteger_);
    if (!arStream.good()) return false;
    if (arCntrl & rOops::cFlagType) {
        // Read the type name.
        readString(arStream, readSize(arStream, doNotSwapInteger_), arCtx.typeName_);
    }
    if (arCntrl & rOops::cFlagVersion) {
        // Read type version. Binary format stores version number on 1 byte only.
        std::uint8_t version = 0;
        read1(arStream, version);
        arCtx.version_ = version;
    }
    if (arCntrl & rOops::cFlagAddress) {
        // Read object address.
        auto address_size = readSize(arStream, doNotSwapInteger_);
        arCtx.address_.read(arStream, address_size);
    }
    if (arCtx.isBlock()) {
        // Read property name.
        readString(arStream, readSize(arStream, doNotSwapInteger_), arCtx.propName_);
    }
    return true;
}

bool rOopsBinaryPrinter::readControlByte(std::istream& arStream, rOops::rOopsParserContext& arCtx, std::uint8_t& arMarker, std::uint8_t& arCntrl)
{
    while (arStream.good()) {
        if (!read1(arStream, arMarker)) return false;
        if (!read1(arStream, arCntrl)) return false;
        if (arCntrl & rOops::cFlagMetaData) {
            if (!processMetaData(arStream, arCtx, arCntrl)) return false;
            if (arCtx.versionMajor < rOops::cOopsBinaryFormatVersionMajor) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Protocol version error: " << arCtx.versionMajor << " < " << rOops::cOopsBinaryFormatVersionMajor));
            }
        }
        else {
            return processControlByte(arStream, arCtx, arCntrl);
        }
    }
    return false;
}

std::string rOopsBinaryPrinter::printOopsBinaryFile(const std::stringstream& arStrm)
{
    std::ofstream bin_file;
    bin_file.open("binary_format.dat", std::ios_base::binary);
    if (bin_file.good()) {
        bin_file << arStrm.str();
        bin_file.close();
    }
    else {
        std::cerr << "Error: " << strerror(errno) << std::endl;
    }
    std::stringstream is(arStrm.str());
    std::stringstream os;
    try {
        rOopsParserContext ctx;
        std::uint8_t marker{ 0 };
        std::uint8_t cntrl{ 0 };
        while (is.good() && !is.eof()) {
            ctx.clear();
            if (!read1(is, marker)) break;
            if (!read1(is, cntrl)) break;
            if (cntrl & rOops::cFlagMetaData) {
                if (!processMetaData(is, ctx, cntrl)) break;
                os << "MetaData integer is " << endian2String(cntrl & rOops::cFlagIntegerEndian) << " endian.\n";
                os << "MetaData float is " << endian2String(cntrl & rOops::cFlagFloatEndian) << " endian.\n";
                switch (static_cast<ControlByteMetaData>(cntrl & cControlByteMask)) {
                    case ControlByteMetaData::eProtocolVersion:
                        // Version number is byte (char) and has to be cast to unsigned integer for not being printed as a character.
                        os << "MetaData version: " << static_cast<std::uint32_t>(ctx.versionMajor) << '.' << static_cast<std::uint32_t>(ctx.versionMinor) << std::endl;
                        break;
                    case ControlByteMetaData::eComment:
                        for (const auto& comment : ctx.comment_) {
                            os << "MetaData version: " << comment << '\n';
                        }
                        break;
                    default:
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unknown Meta Data type: 0x" << std::hex << cntrl));
                }
            }
            else {
                if (!processControlByte(is, ctx, cntrl)) break;
                printHead(os, ctx, marker, cntrl);
                os << " Value: ";
                switch (static_cast<ControlByte>(cntrl & cControlByteMask)) {
                    case ControlByte::eSignedInt:
                        switch (ctx.size_) {
                            case 0: os << "0"; break;
                            case 1: os << static_cast<int>(read1<std::int8_t>(is)); break;
                            case 2: os << read2<std::int16_t>(is, doNotSwapInteger_); break;
                            case 4: os << read4<std::int32_t>(is, doNotSwapInteger_); break;
                            case 8: os << read8<std::int64_t>(is, doNotSwapInteger_); break;
                        }
                        break;
                    case ControlByte::eUnsigned:
                        switch (ctx.size_) {
                            case 0: os << "0"; break;
                            case 1: os << static_cast<unsigned>(read1<std::uint8_t>(is)); break;
                            case 2: os << read2<std::uint16_t>(is, doNotSwapInteger_); break;
                            case 4: os << read4<std::uint32_t>(is, doNotSwapInteger_); break;
                            case 8: os << read8<std::uint64_t>(is, doNotSwapInteger_); break;
                        }
                        break;
                    case ControlByte::eReal:
                        switch (ctx.size_) {
                            case 0: os << "0.0"; break;
                            case 4: os << read4<float>(is, doNotSwapFloat_); break;
                            case 8: os << read8<double>(is, doNotSwapFloat_); break;
                        }
                        break;
                    case ControlByte::eString: {
                        std::string str;
                        readString(is, ctx.size_, str);
                        os << '"' << str << '"';
                    } break;
                    case ControlByte::eValue:
                        if (0 < ctx.size_) {
                            std::string buf;
                            readValue(is, ctx.size_, buf);
                            os << std::hex << int(buf[0]);
                            std::size_t len = std::min(ctx.size_, std::size_t(8u));
                            for (std::size_t idx = 1; idx < len; ++idx) {
                                os << ' ' << int(buf[idx]);
                            }
                            os << " ...";
                        }
                        break;
                    case ControlByte::ePointer: {
                        rOopsAddressString ptr;
                        if (0 < ctx.size_) {
                            ptr.read(is, ctx.size_);
                        }
                        os << ptr << std::dec;
                    } break;
                    case ControlByte::eBool:
                        os << (ctx.size_ ? "true" : "false");
                        break;
                    case ControlByte::eEnum:
                        // Read the underlying integer.
                        switch (ctx.size_) {
                            case 0: os << "0"; break;
                            case 1: os << static_cast<unsigned>(read1<std::uint8_t>(is)); break;
                            case 2: os << read2<std::uint16_t>(is, doNotSwapInteger_); break;
                            case 4: os << read4<std::uint32_t>(is, doNotSwapInteger_); break;
                            case 8: os << read8<std::uint64_t>(is, doNotSwapInteger_); break;
                        }
                        break;
                    case ControlByte::eChar:
                        os << '\'' << ((0 < ctx.size_) ? read1<char>(is) : '\0') << '\'';
                        break;
                    case ControlByte::eBlockBgn:
                        ctx.pushParserState(ParserState::Block);
                        os << "Block BGN";
                        break;
                    case ControlByte::eBlockEnd:
                        ctx.popParserState("Unexpected BLOCK end marker.");
                        os << "Block END";
                        break;
                    case ControlByte::eListBgn:
                        ctx.pushParserState(ParserState::List);
                        os << "List BGN";
                        break;
                    case ControlByte::eListEnd:
                        ctx.popParserState("Unexpected LIST end marker.");
                        os << "List END";
                        break;
                    case ControlByte::eMapBgn:
                        ctx.pushParserState(ParserState::Map);
                        os << "Map BGN";
                        break;
                    case ControlByte::eMapEnd:
                        ctx.popParserState("Unexpected MAP end marker.");
                        os << "Map END";
                        break;
                    default:
                        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected control byte."));
                }
                os << std::endl;
            }
        } //while
    }
    catch (const std::runtime_error& ex) {
        os << "Error parsing binary file: " << ex.what() << std::endl;
    }
    catch (...) {
        os << "Error parsing binary file: unknown exception." << std::endl;
    }
    return os.str();
}

rOopsBinaryParser::rOopsBinaryParser(std::istream& arStream, const std::string& arFileName)
    : rOopsParserBase(arStream)
    , fileName_(arFileName)
{
}

ParserReturnCode rOopsBinaryParser::parse(rOopsParserContext& arCtx)
{
    std::uint8_t marker{ 0 };
    std::uint8_t cntrl{ 0 };
    while (istrm_.good() && !istrm_.eof()) {
        arCtx.clear();
        if (!readControlByte(istrm_, arCtx, marker, cntrl)) {
            break;
        }
        // For testing and debugging the parser:
        // printHead(std::cout, arCtx, marker, cntrl);
        // std::cout << std::endl;
        auto cntrl_byte = static_cast<ControlByte>(cntrl & cControlByteMask);
        switch (cntrl_byte) {
            case ControlByte::eSignedInt:
                return arCtx.returnCode_ = ParserReturnCode::eSignedInt;
            case ControlByte::eUnsigned:
                return arCtx.returnCode_ = ParserReturnCode::eUnsigned;
            case ControlByte::eReal:
                return arCtx.returnCode_ = ParserReturnCode::eReal;
            case ControlByte::eString:
                return arCtx.returnCode_ = ParserReturnCode::eString;
            case ControlByte::eValue:
                return arCtx.returnCode_ = ParserReturnCode::eValue;
            case ControlByte::eBool:
                return arCtx.returnCode_ = ParserReturnCode::eBool;
            case ControlByte::eEnum:
                return arCtx.returnCode_ = ParserReturnCode::eEnum;
            case ControlByte::eChar:
                return arCtx.returnCode_ = ParserReturnCode::eChar;
            case ControlByte::ePointer:
                if (0 < arCtx.size_) {
                    arCtx.pointer_.read(istrm_, arCtx.size_);
                }
                else {
                    arCtx.pointer_.clear();
                }
                return arCtx.returnCode_ = ParserReturnCode::ePointer;
            case ControlByte::eBlockBgn:
                arCtx.pushParserState(ParserState::Block);
                return arCtx.returnCode_ = ParserReturnCode::eBlockBgn;
            case ControlByte::eBlockEnd: {
                auto state = arCtx.popParserState("Unexpected block end marker.");
                if (ParserState::Block != state) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Block end marker found in Parser State " << static_cast<char>(state)));
                }
                return arCtx.returnCode_ = ParserReturnCode::eBlockEnd;
            }
            case ControlByte::eListBgn:
                arCtx.pushParserState(ParserState::List);
                return arCtx.returnCode_ = ParserReturnCode::eListBgn;
            case ControlByte::eListEnd: {
                auto state = arCtx.popParserState("Unexpected list end marker.");
                if (ParserState::List != state) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("List end marker found in Parser State " << static_cast<char>(state)));
                }
                return arCtx.returnCode_ = ParserReturnCode::eListEnd;
            }
            case ControlByte::eMapBgn:
                arCtx.pushParserState(ParserState::Map);
                return arCtx.returnCode_ = ParserReturnCode::eMapBgn;
            case ControlByte::eMapEnd: {
                auto state = arCtx.popParserState("Unexpected map end marker.");
                if (ParserState::Map != state) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Map end marker found in Parser State " << static_cast<char>(state)));
                }
                return arCtx.returnCode_ = ParserReturnCode::eMapEnd;
            }
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected control byte."));
        } //switch
    } //while
    return ParserReturnCode::eEndOfStream;
}

std::string rOopsBinaryParser::where() const
{
    return fileName_ + std::to_string(istrm_.tellg());
}

std::uint64_t rOopsBinaryParser::readSize()
{
    return ::readSize(istrm_, doNotSwapInteger_);
}

void rOopsBinaryParser::readString(std::uint64_t aStrSize, std::string& arString)
{
    ::readString(istrm_, aStrSize, arString);
}

} //namespace rOops

