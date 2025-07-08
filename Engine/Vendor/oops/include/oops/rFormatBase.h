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

#include <oops/rPropTypeInfo.h>
#include <oops/rPropLog.h>
#include <deque>
#include <unordered_set>
#include <cstring>
#include <ostream>

namespace rOops
{

/**
 * Common base class for all format classes, both binary and text.
 * Collects common members.
 */
class rFormatBase
{
    struct UnsavedObject
    {
        UnsavedObject(const rPropTypeInfo* apTI, const void* apObj)
            : pTypeInfo_(apTI)
            , pObject_(apObj)
        {}
        const rPropTypeInfo* pTypeInfo_;
        const void* pObject_;
    };
public:
    rFormatBase(std::ostream& arStrm, const OopsConfig& arConfig)
        : strm_(arStrm)
        , config_(arConfig)
    {
    }
    const OopsConfig& config() const
    {
        return config_;
    }
    void setAddress(const void* apObjAddr)
    {
        static_assert(sizeof(uintptr_t)<=sizeof(std::uint64_t), "Address must fit into an uint64.");
        objAddress_ = reinterpret_cast<std::uint64_t>(apObjAddr);
    }
    void addAlreadySavedObject(const void* apObj)
    {
        objectIdSet_.insert(apObj);
    }
    bool isAlreadySaved(const void* apObj) const
    {
        return (objectIdSet_.end() != objectIdSet_.find(apObj));
    }
    void addUnsavedObject(const rPropTypeInfo* apTI, const void* apObj)
    {
        if (nullptr != apObj && objectIdSet_.end()==objectIdSet_.find(apObj) ) {
            unsaved_.emplace_back(apTI, apObj);
        }
    }
    template <typename FormatT>
    void saveAllUnsaved(FormatT& arFormat)
    {
        while (!unsaved_.empty()) {
            const auto& o = unsaved_.front();
            if( objectIdSet_.end() == objectIdSet_.find(o.pObject_) ) {
                // Have not saved.
                const rPropTypeInfo* pTI = o.pTypeInfo_->objTypeInfo(o.pObject_);
                arFormat.setPointedObject(pTI, o.pObject_);
                pTI->save(arFormat, o.pObject_);
                objectIdSet_.insert(o.pObject_);
                arFormat.writeValueSeparator(false);
            } //if
            unsaved_.pop_front();
        } //while
    }
    std::ostream& strm() { return strm_; }
    const rPropTypeInfo* typeInfo() const { return typeInfo_; }
    void setPropName(std::string aPropName) { propName_ = std::move(aPropName); }
protected:
    std::ostream& strm_;
    OopsConfig config_;
    std::string propName_;
    std::uintptr_t objAddress_{ 0u };
    const rPropTypeInfo* typeInfo_{ nullptr };
    /// Set of address and node of objects.
    /// It is used for finding multiple references to the same object.
    std::unordered_set<const void*> objectIdSet_;
    std::deque<UnsavedObject> unsaved_;
}; //class rFormatBase

} //namespace rOops
