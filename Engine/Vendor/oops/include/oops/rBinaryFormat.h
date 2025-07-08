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

#include <oops/rFormatBase.h>
#include <oops/rPropLog.h>
#include <array>
#include <cstring>
#include <ostream>

namespace rOops
{

/**
 Binary format tag class.
 Used as a base class for binary formats.
 */
class rBinaryFormat : public rFormatBase
{
public:
    rBinaryFormat(std::ostream& arStrm, const OopsConfig& arConfig)
        : rFormatBase(arStrm, arConfig)
    {
        buffer_ = std::make_unique< std::array<char, 1024 * 1024> >();
        reset();
    }
    void reset()
    {
        outputPointer_ = &(*buffer_)[0];
        // strm_.clear();
    }
    inline void flush()
    {
        if (useMemoryBuffer_) {
            strm_.rdbuf()->sputn(buffer_->data(), static_cast<std::streamsize>(outputPointer_-buffer_->data()) );
        }
        strm_ << std::flush;
        reset();
    }
    template <typename DataT>
    inline void write(const DataT& arData)
    {
        write(&arData, sizeof(DataT));
    }
    inline void write(const std::string& arData)
    {
        write(arData.data(), arData.size());
    }
    template <typename DataT>
    inline void write(const DataT* apData, std::uint64_t size)
    {
        if (useMemoryBuffer_) {
            reserve(size);
            memcpy(outputPointer_, apData, size);
            outputPointer_ += size;
        }
        else {
            // write() is significantly slower than rdbuf()->sputn().
            //strm_.write( static_cast<const char*>(static_cast<const void*>(apData)), static_cast<std::streamsize>(size) );
            strm_.rdbuf()->sputn( static_cast<const char*>(static_cast<const void*>(apData)), static_cast<std::streamsize>(size) );
        }
    }
    void reserve(std::size_t size)
    {
        if (&(*buffer_)[0]+buffer_->size() <= outputPointer_+size) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Out of memory."));
        }
    }
    /**
     * writeEndOfLine() only added for having the same interface for all Format classes.
     * It is called from save() after writing a top level object.
     */
    void writeEndOfLine()
    {
        // Nothing to do here.
    }
protected:
    bool useMemoryBuffer_{ true };
    std::unique_ptr< std::array<char, 1024 * 1024> > buffer_;
    char* outputPointer_{ nullptr };
}; //class rBinaryFormat

} //namespace rOops
