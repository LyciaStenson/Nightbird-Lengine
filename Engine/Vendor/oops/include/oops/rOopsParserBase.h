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

#include <oops/rPropDescriptor.h>
#include <oops/rPropTypeDef.h>
#include <oops/rPropLog.h>
#include <string>
#include <unordered_map>

namespace rOops
{

/**
* Base class of all parsers.
* It contains some common members and logic for handling the input stream and pointers.
*/
class rOopsParserBase
{
public:
    explicit rOopsParserBase(std::istream& arStream)
        : istrm_(arStream)
    {}
    ~rOopsParserBase() = default;
    void read(void* apVal, std::size_t aSize, bool aDoNotSwap)
    {
        if (aDoNotSwap) {
            istrm_.read(static_cast<char*>(apVal), static_cast<std::streamsize>(aSize));
        }
        else {
            char* buf = static_cast<char*>(apVal);
            std::size_t ii = aSize;
            while (0 < ii) {
                --ii;
                istrm_.read(&buf[ii], 1);
            }
        }
    }
    template <typename DataT>
    void read(DataT& arVal, bool aDoNotSwap)
    {
        if (aDoNotSwap) {
            istrm_.read(static_cast<char*>(static_cast<void*>(&arVal)), static_cast<std::streamsize>(sizeof(DataT)));
        }
        else {
            char* buf = static_cast<char*>(static_cast<void*>(&arVal));
            std::size_t ii = sizeof(DataT);
            while (0 < ii) {
                --ii;
                istrm_.read(&buf[ii], 1);
            }
        }
    }
    /**
    * Handle unloaded objects and update pointers.
    * It is called when all objects are loaded for parsing the objects saved at the end of the stream.
    * When objects were not saved in place or several pointer points to the same object
    * these objects have to be loaded and some pointers have to be updated at the end of the loading process.
    *
    * This function has the same parameters as the parse() function, because it is calling it.
    * The arParser is the same as this object, but not the base class, but the descendant text or binary parser.
    */
    template <typename ParserT>
    void createUnloadedObjects(ParserT& arParser, rOopsParserContext& arCtx)
    {
        while (!unloadedObjects_.empty()) {
            if (rOops::ParserReturnCode::eEndOfStream == arParser.parse(arCtx)) {
                // End of input stream.
                break;
            }
            auto old_addr = arCtx.address_;
            const rPropTypeInfo* pTI = rPropGetTypeInfoFromContext(arCtx);
            void* pVal = pTI->createObj();
            pTI->load(arParser, pVal, arCtx);
            updatePointer(old_addr, pVal);
            arParser.parseTopLevelSeparator(arCtx); // parse top level ';'.
        }
        if (ParserState::TopLevel!=arCtx.getParserState()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Parser should be at top level."));
        }
        while (!unloadedObjects_.empty()) {
            auto iCur = unloadedObjects_.begin();
            auto ptr_info = getAlreadyLoadedObject(iCur->first);
            if (nullptr == ptr_info.pointer_) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Some pointers are not updated."));
            }
            iCur->second.propDesc_->getApplyPointer()->assign(iCur->second.pointer_, ptr_info);
            unloadedObjects_.erase(iCur);
        }
    }
    void addToUnloadedObjects(const rOopsAddressString& arOldAddr, const rPropDescriptor* apPropDesc, void* apPointer)
    {
        unloadedObjects_.emplace(arOldAddr, PointerInfo(apPropDesc, apPointer));
    }
    void addToAlreadyLoadedObjects(const rOopsAddressString& arOldAddr, const rPropDescriptor* apPropDesc, const void* apPointer)
    {
        alreadyLoadedObjects_.emplace(arOldAddr, ConstPointerInfo(apPropDesc, apPointer));
    }
    ConstPointerInfo getAlreadyLoadedObject(const rOopsAddressString& arOldAddr)
    {
        auto iter = alreadyLoadedObjects_.find(arOldAddr);
        if (alreadyLoadedObjects_.end() == iter) {
            return {nullptr, nullptr};
        }
        else {
            return iter->second;
        }
    }
protected:
    void updatePointer(const rOopsAddressString& arOldAddr, void* apVal)
    {
        // TODO: This is not working correctly when unique_ptr and raw pointers point to the same object, and the unique pointer is not the first element of the range.
        auto range = unloadedObjects_.equal_range(arOldAddr);
        if (range.first == range.second) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Address " << std::hex << arOldAddr << " does not seen before."));
        }
        auto iBgn = range.first;
        auto iEnd = range.second;
        // Set the first pointer.
        iBgn->second.propDesc_->getApplyPointer()->set(iBgn->second.pointer_, apVal);
        // Assign the first pointer to all others for maintaining reference counted shared pointers.
        auto iCur = iBgn;
        ++iCur;
        while (iCur != iEnd) {
            // 2 unique pointers cannot point to the same object, so assign is not called for unique pointers.
            iCur->second.propDesc_->getApplyPointer()->assign(iCur->second.pointer_, iBgn->second);
            ++iCur;
        }
        // Remove the updated pointers from unloadedObjects_.
        unloadedObjects_.erase(iBgn, iEnd);
    }
protected:
    std::istream& istrm_; ///< Reference to the stream object containing data (text or binary) for parsing.
    std::unordered_multimap<rOopsAddressString, PointerInfo, rOopsAddressStringHash> unloadedObjects_; ///< Old address is the key, and the value is a reference to an empty pointer, which has to be updated later.
    std::unordered_map<rOopsAddressString, ConstPointerInfo, rOopsAddressStringHash> alreadyLoadedObjects_; ///< Old address is the key, and the value is a reference to a pointer already pointing to the object.
}; //class rOopsParserBase

class rOopsTextParserBase : public rOopsParserBase
{
public:
    explicit rOopsTextParserBase(std::istream& arStream)
        : rOopsParserBase(arStream)
    {}
    ~rOopsTextParserBase() = default;
}; //class rOopsTextParserBase

} //namespace rOops
