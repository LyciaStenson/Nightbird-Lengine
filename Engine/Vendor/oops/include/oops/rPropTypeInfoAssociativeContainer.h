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

/**
Associative Containers Type Info class.
std::map and std::unordered_map.
*/

#include <oops/rPropTypeInfoContainer.h>
#include <oops/rPropIter.h>
#include <memory>
#include <type_traits>

namespace rOops
{

    template <typename ContainerT>
    class rPropTypeInfoAssociative : public rPropTypeInfoSTLContainerBase<ContainerT, typename ContainerT::mapped_type>
    {
    public:
        rPropTypeInfoAssociative(const std::string& arTypeName, const rPropTypeInfo* apKeyTypeInfo, const rPropTypeInfo* apElementTypeInfo)
            : rPropTypeInfoSTLContainerBase<ContainerT, typename ContainerT::mapped_type>(arTypeName, apElementTypeInfo, false)
            , pKeyTypeInfo_(apKeyTypeInfo)
            , keyPropDesc_(keyTypeInfo(), createApplyPointer<typename ContainerT::key_type>(), "", 0, rPropFlag::cDefault)
        {}
        /// When the size is know reserve memory to store all elements without reallocation.
        void setNrOfElements(void* /*apContainer*/, std::size_t /*aSize*/) const override
        {
            // std::map has no reserve().
            //ContainerT* pSet = static_cast<ContainerT*>(apContainer);
            //pSet->reserve(aSize);
        }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override
        {
            save_(arFormat, apVal);
        }
        void save(rOopsTextFormat& arFormat, const void *apVal) const override
        {
            save_(arFormat, apVal);
        }
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override
        {
            save_(arFormat, apVal);
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            if (!arCtx.isMap()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoAssociative::load() is called when parser context is not Map."));
            }
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            // Text parser cannot make difference of list and map at this point.
            if (!arCtx.isMap()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoAssociative::load() is called when parser context is not Map."));
            }
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            // Yaml parser cannot make difference of list and map at this point.
            if (!arCtx.isMap() && !arCtx.isList()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoAssociative::load() is called when parser context is not Map."));
            }
            load_(arParser, apVal, arCtx); 
        }
    private:
        template <typename FormatT>
        void save_(FormatT& arFormat, const void *apVal) const
        {
            const auto* pMap = static_cast<const ContainerT*>(apVal);
            const rPropDescriptor* pPDKey = this->keyPropDesc();
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            const rPropTypeInfo* pTIKey = pPDKey->propTypeInfo();
            const auto key_write_type_name_above = pPDKey->writeTypeNameAbove();
            const rPropTypeInfo* pTIValue = pPDValue->propTypeInfo();
            const auto val_write_type_name_above = pPDValue->writeTypeNameAbove();
            // Map has no reserve(), no need to write size().
            arFormat.writeMapBegin();
            auto iValue = pMap->begin();
            auto iEnd = pMap->end();
            while( iValue != iEnd ) {
                arFormat.doIndent();
                const void* keyAddr = &(iValue->first);
                arFormat.setType(pTIKey, key_write_type_name_above, pTIKey->nrOfElements(keyAddr));
                pPDKey->save(arFormat, keyAddr, false);
                arFormat.writeKeyValueSeparator();
                const void* objAddr = &(iValue->second);
                arFormat.setType(pTIValue, val_write_type_name_above, pTIValue->nrOfElements(objAddr));
                pPDValue->save(arFormat, objAddr, true);
                ++iValue;
                arFormat.writeListSeparator(iValue==iEnd);
            } //while
            arFormat.writeMapEnd();
        }
        template <typename ParserT>
        void load_(ParserT& arParser, void* apVal, rOopsParserContext& arCtx) const
        {
            auto isContainerEnd = [](const ParserReturnCode aParserReturnCode)
            {
                return (ParserReturnCode::eListEnd == aParserReturnCode || ParserReturnCode::eMapEnd == aParserReturnCode);
            };
            auto* pMap = static_cast<ContainerT*>(apVal);
            const rPropDescriptor* pPDKey = this->keyPropDesc();
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            // Text parsers cannot make difference of list and map. Therefor accept both eListEnd and eMapEnd in the loop exit condition.
            while (!isContainerEnd(arParser.parse(arCtx))) {
                // Load the key into a local variable, but allocate it first, if it is a pointer.
                typename ContainerT::key_type key;
                pPDKey->load(arParser, &key, arCtx);
                arParser.parse(arCtx);
                typename ContainerT::mapped_type val;
                pPDValue->load(arParser, &val, arCtx);
                // Insert the key - value pair. 'val' is temporary local variable, so the best is to move it.
                pMap->insert( std::move( typename ContainerT::value_type(std::move(key), std::move(val)) ) );
            } //while
        }

    public:
        rPropIterator begin(void* apVal, const std::string&) const override
        {
            auto* pMap = static_cast<ContainerT*>(apVal);
            rPropIterator iMap(std::make_unique<rPropIteratorAssociative<ContainerT*, typename ContainerT::iterator, rPropIteratorBase, void*>>(this, pMap));
            return iMap;
        }
        rPropConstIterator begin(const void* apVal, const std::string&) const override
        {
            const auto* pMap = static_cast<const ContainerT*>(apVal);
            rPropConstIterator iMap(std::make_unique<rPropConstIterAssociative<const ContainerT*, typename ContainerT::const_iterator, rPropConstIterBase, const void*>>(this, pMap));
            return iMap;
        }
        bool isSTLContainer() const override
            { return false; }
        bool isAssociativeContainer() const override
            { return true; }
        //const rPropTypeInfo* elementTypeInfo() const override
        //    {
        //        if (nullptr==this->pElementTypeInfo_) {
        //            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info of mapped type not found."));
        //        }
        //        return this->pElementTypeInfo_;
        //    }
        const rPropTypeInfo* keyTypeInfo() const override
            {
                if (nullptr==pKeyTypeInfo_) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info of key type not found."));
                }
                return pKeyTypeInfo_;
            }
        const rPropDescriptor* keyPropDesc() const override
        {
            //initKeyPropDesc();
            return &keyPropDesc_;
        }
        rPropDescriptor* keyPropDesc() override
        {
            //initKeyPropDesc();
            return &keyPropDesc_;
        }
    protected:
        //virtual void initKeyPropDesc() const
        //    {
        //        // TODO: make it thread safe!
        //        // Created at first access, because keyTypeInfo() may not exist at construction time.
        //        if (!keyPropDesc_.isInitialized()) {
        //            keyPropDesc_.setTypeInfo(keyTypeInfo());
        //        }
        //    }
    protected:
        const rPropTypeInfo* pKeyTypeInfo_=nullptr;
        mutable rPropDescriptor keyPropDesc_;
    }; //rPropTypeInfoAssociative

} //namespace rOops
