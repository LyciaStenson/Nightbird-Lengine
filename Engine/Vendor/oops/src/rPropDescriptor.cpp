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
Property Descriptor.
*/

#include <oops/rPropDescriptor.h>
#include <oops/rPropLog.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropTypeInfoBase.h>

namespace rOops
{

rPropDescriptor::~rPropDescriptor()
{
    if (Type::eRunTime == type_) {
        delete pTypeInfo_;
    }
}

rPropDescriptor::rPropDescriptor(const rPropTypeInfo* apTypeInfo, offset_type aBase, flags_type aFlags)
    : pTypeInfo_(apTypeInfo)
    , pApplyPtr_(std::make_shared<rApplyPointer>())
    , propName_(apTypeInfo->typeName())
    , propNameId_(apTypeInfo->TypeId())
    , flags_(aFlags)
    , defaultValue_(rPropDefault())
    , offset_(aBase)
    , type_(Type::eAncestor)
{
    if (nullptr == pTypeInfo_) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    }
}

rPropDescriptor::rPropDescriptor(const rPropDescriptor& arPropDesc, offset_type aBase)
    : pTypeInfo_(arPropDesc.pTypeInfo_)
    , pApplyPtr_(arPropDesc.pApplyPtr_)
    , propName_(arPropDesc.propName_)
    , propNameId_(arPropDesc.propNameId_)
    , flags_(arPropDesc.flags_)
    , defaultValue_(arPropDesc.defaultValue_)
    , offset_(arPropDesc.offset_+aBase)
    , type_(arPropDesc.type_)
{
}

rPropDescriptor::rPropDescriptor(const rPropTypeInfo* apTypeInfo, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, offset_type aOffset, flags_type aFlags, const rPropDefault aDefaultValue)
    : pTypeInfo_(apTypeInfo)
    , pApplyPtr_(std::move(apApplyPtr))
    , propName_(arPropName)
    , propNameId_(generateNameId(arPropName))
    , flags_(aFlags)
    , defaultValue_(aDefaultValue)
    , offset_(aOffset)
    , type_(Type::eMember)
{
    if (nullptr == pTypeInfo_) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    }
}

//rPropDescriptor::rPropDescriptor(const rPropTypeInfo* apType, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, SetMemberFuncPtr_t apSet, GetMemberFuncPtr_t apGet, flags_type aFlags)
//    : rPropDescriptor(apType, std::move(apApplyPtr), arPropName, aFlags),
//    _pSetMemFn(apSet),
//    _pGetMemFn(apGet)
//{}

rPropDescriptor::rPropDescriptor(const std::string& arTypeName, size_t aIdx, const std::string& arPropName, offset_type aOffset, flags_type aFlags)
    : pTypeInfo_(nullptr)
    , pApplyPtr_(std::make_shared<rApplyPointer>())
    , propName_(arPropName)
    , propNameId_(generateNameId(arPropName))
    , flags_(aFlags)
    , defaultValue_(rPropDefault())
    , offset_(aOffset)
    , type_(Type::eRunTime)
{
    // Do not use existing type info, even if the type is a known and registered type name.
    //pTypeInfo_ = rPropGetTypeInfoByName(arTypeName);
    // TODO: The value is stored as string in _unknowPropTbl, and the existing type info classes cannot handle that.
    // Create new type info class. This type info object will belong to this type descriptor, and it will not be registered, so rPropGetTypeInfoByName() will not find it.
    // This is fine, because unknown properties rarely introduce new types.
    pTypeInfo_ = new rPropTypeInfoRunTimeType(arTypeName);
    offset_ = aOffset;
    _descTableIndex = aIdx;
}

size_type rPropDescriptor::Size() const
{
    if (nullptr==pTypeInfo_) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    if( isPointer() ) return sizeof(void*);
    else return pTypeInfo_->Size();
}

rPropDescriptor& rPropDescriptor::operator,( const ElementFlags_s& arFlags )
{
    if (nullptr==pTypeInfo_) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    auto* pDesc = const_cast<rPropDescriptor*>(pTypeInfo_->elementPropDesc());
    if(nullptr != pDesc ) {
        pDesc->flags_ |= arFlags.flags;
    }
    return *this;
}

const rPropTypeInfo* rPropDescriptor::propTypeInfo() const
{
    if (nullptr == pTypeInfo_) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    return pTypeInfo_;
}

//void rPropDescriptor::setTypeInfo(const rPropTypeInfo* apTypeInfo)
//{
//    pTypeInfo_ = apTypeInfo;
//}
//
//bool rPropDescriptor::isInitialized() const
//{
//    return (nullptr != pTypeInfo_);
//}

const rPropTypeInfo* rPropDescriptor::objTypeInfo( const void* apObjAddr ) const
{
    if (nullptr==pTypeInfo_) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pTypeInfo_ is null."));
    }
    const rPropTypeInfo* pTI = nullptr;
    if( isPointer() && nullptr!=apObjAddr ) {
        pTI = pTypeInfo_->objTypeInfo(apObjAddr);
    } else {
        pTI = pTypeInfo_;
    }
    return pTI;
}

void* rPropDescriptor::propAddr( void* apBaseAddr ) const
{
    if (nullptr == apBaseAddr) return nullptr;
    void* pPropAddr = static_cast<ptr_type>(apBaseAddr) + offset_;
    //rDbgTrace("PropAddr(" << apBaseAddr << "): " << pTypeInfo_->typeName() << " " << pPropAddr);
    switch (type_) {
        case Type::eAncestor:
        case Type::eMember:
            return pPropAddr;
        case Type::eRunTime: {
            auto pUnknowPropTbl = static_cast<UnknownPropertyTable_t*>(pPropAddr);
            return &((*pUnknowPropTbl)[_descTableIndex]);
        }
        case Type::eNone:
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Uninitialized rPropDescriptor: type_ is eNone."));
    }
    return nullptr;
}

const void* rPropDescriptor::propAddr( const void* apBaseAddr ) const
{
    if (nullptr == apBaseAddr) return nullptr;
    const void* pPropAddr = static_cast<const_ptr_type>(apBaseAddr) + offset_;
    //rDbgTrace("PropAddr(" << apBaseAddr << "): " << pTypeInfo_->typeName() << " " << pPropAddr);
    switch (type_) {
        case Type::eAncestor:
        case Type::eMember:
            return pPropAddr;
        case Type::eRunTime: {
            const void* pPropAddr = static_cast<const_ptr_type>(apBaseAddr) + offset_;
            auto pUnknowPropTbl = static_cast<const UnknownPropertyTable_t*>(pPropAddr);
            auto iter = pUnknowPropTbl->find(_descTableIndex);
            if (pUnknowPropTbl->end() == iter) {
                // It may happen that a property is added to an instance but not added to another instance of the same type.
                return &static_empty_string;
            }
            else {
                return &(iter->second);
            }
        }
        case Type::eNone:
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Uninitialized rPropDescriptor: type_ is eNone."));
    }
    return nullptr;
}

void* rPropDescriptor::objAddr( void *apBaseAddr ) const
{
    if (nullptr == apBaseAddr) return nullptr;
    void* pObjAddr = pApplyPtr_->apply(propAddr(apBaseAddr));
    //rDbgTrace("PropAddr(" << apBaseAddr << "): " << pTypeInfo_->typeName() << " " << pObjAddr);
    return pObjAddr;
}


const void* rPropDescriptor::objAddr( const void *apBaseAddr ) const
{
    if (nullptr == apBaseAddr) return nullptr;
    const void* pObjAddr = pApplyPtr_->apply(propAddr(apBaseAddr));
    //rDbgTrace("PropAddr(" << apBaseAddr << "): " << pTypeInfo_->typeName() << " " << pObjAddr);
    return pObjAddr;
}

void rPropDescriptor::setObjAddr(void* apBaseAddr, void* apAddr) const
{
    pApplyPtr_->set(propAddr(apBaseAddr), apAddr);
    //rDbgTrace("PropAddr(" << apBaseAddr << "): " << pTypeInfo_->typeName() << " " << pObjAddr);
}

} //namespace rOops
