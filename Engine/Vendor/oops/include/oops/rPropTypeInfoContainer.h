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
Container Type Info Classes.
*/

#include <oops/rPropTypeInfoBase.h>
#include <oops/rPropIter.h>
#include <oops/rPropTypeTraits.h>


namespace rOops
{

    // In case of associative containers ValueT is mapped_type.
    template <typename ContainerT, typename ValueT=typename ContainerT::value_type>
    class rPropTypeInfoSTLContainerBase : public rPropTypeInfo
    {
    public:
        explicit rPropTypeInfoSTLContainerBase( const std::string& arTypeName, const rPropTypeInfo* apElementTypeInfo, bool aCanSaveAddress )
            : rPropTypeInfo(arTypeName)
            , pElementTypeInfo_(apElementTypeInfo)
            , elementPropDesc_(apElementTypeInfo, createApplyPointer<ValueT>(), "", 0, rPropFlag::cDefault)
            , canSaveAddress_(aCanSaveAddress)
            {}
        size_type Size() const override { return sizeof(ContainerT); }
        /// Create single object with 'new'.
        void* createObj() const override
        {
            return new ContainerT();
        }
        /// Create an object with placement new.
        void *createObj( void *apData ) const override
        {
            return new(apData) ContainerT();
        }
        /// Create array of objects with 'new[aN]'.
        void* createObj( size_type aN ) const override
        {
            return new ContainerT[aN];
        }
        /// Destroy object allocated with new.
        void destroyObj( void* apObj ) const override
        {
            delete ((ContainerT*)apObj);
        }
        /// Destroy object allocated with placement new.
        void destroyObj( void* apObj, void* /*apData*/ ) const override
        {
            ((ContainerT*)apObj)->~ContainerT();
        }
        /// Destroy array of objects with 'delete[]'.
        void destroyObj( void* apObjArr, size_type /*aN*/ ) const override
        {
            delete[] ((ContainerT*)apObjArr);
        }
        bool isValue() const override { return false; }
        /// Returns true, if this type is a standard container.
        bool isSTLContainer() const override { return true; }

        std::string value( const void* /*apVal*/ ) const override
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("value() is not implemented."));
        }
        void value( const void* /*apVal*/, std::string& /*arValue*/ ) const override
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("value() is not implemented."));
        }
        void setValue( void* /*apVal*/, const std::string& /*arValue*/ ) const override
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("setValue() is not implemented."));
        }
        const rPropTypeInfo* elementTypeInfo() const override
        {
            if (nullptr == this->pElementTypeInfo_) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info of value type not found."));
            }
            return this->pElementTypeInfo_;
        }
        const rPropDescriptor* elementPropDesc() const override
        {
            return &elementPropDesc_;
        }
        rPropDescriptor* elementPropDesc() override
        {
            return &elementPropDesc_;
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

    private:
        template <typename FormatT>
        void save_(FormatT& arFormat, const void *apVal) const
        {
            const auto* pContainer = static_cast<const ContainerT*>(apVal);
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            arFormat.writeListBegin();
            auto iValue = pContainer->begin();
            auto iEnd = pContainer->end();
            while( iValue != iEnd ) {
                pPDValue->save(arFormat, &(*iValue), canSaveAddress_);
                ++iValue;
                arFormat.writeListSeparator(iValue==iEnd);
            } //while
            arFormat.writeListEnd();
        }
    protected:
        const rPropTypeInfo* pElementTypeInfo_ = nullptr;
        mutable rPropDescriptor elementPropDesc_;
        const bool canSaveAddress_ = true; ///< Array, deque, vector, list can, but set and map cannot, because the object loaded later cannot be assigned, it must be inserted into the container.
    }; //rPropTypeInfoSTLContainerBase


    /// Type info for std::array.
    /**
    ArrayT is a type having operator [] for accessing its elements and has a fixed size: std::array.
    */
    template< typename ArrayT >
    class rPropTypeInfoArray : public rPropTypeInfoSTLContainerBase<ArrayT>
    {
    public:
        rPropTypeInfoArray(const std::string& arTypeName, const rPropTypeInfo* apElementTypeInfo)
            : rPropTypeInfoSTLContainerBase<ArrayT>(arTypeName, apElementTypeInfo, true)
        {}
        /// Sets the size of the container. This functions ensures that the container will contains the given number of elements.
        void setNrOfElements(void* apContainer, std::size_t aSize) const override
        {
            // Size is compile time constant. Check if it is the same as the size given here.
            auto* pArray = static_cast<ArrayT*>(apContainer);
            if (pArray->size() != aSize) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Array size is not the same as the size in the stream: " << pArray->size() << " != : " << aSize));
            }
        }
        rPropIterator begin(void* apVal, const std::string&) const override
        {
            auto* pArray = static_cast<ArrayT*>(apVal);
            rPropIterator arrayIter(std::make_unique<rPropIteratorArray<ArrayT*, typename ArrayT::iterator, rPropIteratorBase, void*>>(this, pArray));
            return arrayIter;
        }
        rPropConstIterator begin(const void* apVal, const std::string&) const override
        {
            const auto* pArray = static_cast<const ArrayT*>(apVal);
            rPropConstIterator iList(std::make_unique<rPropConstIterArray<const ArrayT*, typename ArrayT::const_iterator, rPropConstIterBase, const void*>>(this, pArray));
            return iList;
        }
        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
    private:
        template <typename ParserT>
        void load_(ParserT& arParser, void* apVal, rOopsParserContext& arCtx) const
        {
            if (!arCtx.isList()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoCompoundTypeBase::load() is called when parser context is not block."));
            }
            auto* pArray = static_cast<ArrayT*>(apVal);
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            std::size_t idx = 0;
            while (ParserReturnCode::eListEnd != arParser.parse(arCtx)) {
                typename ArrayT::value_type val;
                pPDValue->load(arParser, &val, arCtx);
                if (idx < pArray->size()) {
                    assignAtHelper(*pArray, idx, val);
                }
                ++idx;
            }
        }
    }; //rPropTypeInfoArray


    /// Type info of simple STL containers like deque, vector, list.
    /**
    New elements are pushed back, and value_type shall be a type definition for the value type.
    Sets and associative containers are different because new elements shall be inserted.
    Furthermore the value_type of associative containers are std::pair<key_type, mapped_type>.
    */
    template< typename ContainerT >
    class rPropTypeInfoSTLList : public rPropTypeInfoSTLContainerBase<ContainerT>
    {
    public:
        rPropTypeInfoSTLList(const std::string& arTypeName, const rPropTypeInfo* apElementTypeInfo)
            : rPropTypeInfoSTLContainerBase<ContainerT>(arTypeName, apElementTypeInfo, false)
        {}
        rPropIterator begin(void* apVal, const std::string&) const override
        {
            auto* pList = static_cast<ContainerT*>(apVal);
            rPropIterator iList(std::make_unique<rPropIteratorSTLList<ContainerT*, typename ContainerT::iterator, rPropIteratorBase, void*>>(this, pList));
            return iList;
        }
        rPropConstIterator begin(const void* apVal, const std::string&) const override
        {
            const auto* pList = static_cast<const ContainerT*>(apVal);
            rPropConstIterator iList(std::make_unique<rPropConstIterSTLList<const ContainerT*, typename ContainerT::const_iterator, rPropConstIterBase, const void*>>(this, pList));
            return iList;
        }
        /// Returns the number of elements stored in the container.
        std::size_t nrOfElements( const void *apContainer ) const override
        {
            const auto* pList = static_cast<const ContainerT*>(apContainer);
            return pList->size();
        }
        /// When the size is known reserve memory to store all elements without reallocation.
        void setNrOfElements(void* /*apContainer*/, std::size_t /*aSize*/) const override
        {
            // std::list and std::set have no reserve().
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
    private:
        template <typename ParserT>
        void load_(ParserT& arParser, void* apVal, rOopsParserContext& arCtx) const
        {
            if (!arCtx.isList()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoSTLList::load() is called when parser context is not list."));
            }
            auto* pContainer = static_cast<ContainerT*>(apVal);
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            if (pPDValue->isPointer()) {
                while (ParserReturnCode::eListEnd != arParser.parse(arCtx)) {
                    typename ContainerT::value_type ptr;
                    if (!arCtx.pointer_.empty()) {
                        auto ctx_ptr = arCtx.pointer_; // store it, because calling the parser will change the context.
                        bool unloaded_object = pPDValue->handlePointer(arParser, &ptr, arCtx);
                        pContainer->push_back(std::move(ptr));
                        typename ContainerT::value_type* pPtrInContainer = &(pContainer->back());
                        if (unloaded_object) {
                            arParser.addToUnloadedObjects(ctx_ptr, pPDValue, pPtrInContainer);
                        }
                    }
                    else {
                        if (ParserReturnCode::ePointer == arCtx.returnCode_) {
                            // Pointer was parsed, pointer_ is NULL, set pMember to nullptr.
                            pPDValue->loadNullPointer(&ptr);
                        }
                        else {
                            auto ctx_addr = arCtx.address_; // store it, because calling the parser will change the context.
                            pPDValue->loadSavedInPlacePointer(arParser, &ptr, arCtx);
                            pContainer->push_back(std::move(ptr));
                            if (!ctx_addr.empty()) {
                                typename ContainerT::value_type *pPtrInContainer = &(pContainer->back());
                                arParser.addToAlreadyLoadedObjects(ctx_addr, pPDValue, pPtrInContainer);
                            }
                        }
                    }
                }
            }
            else {
                while (ParserReturnCode::eListEnd != arParser.parse(arCtx)) {
                    typename ContainerT::value_type val;
                    pPDValue->load(arParser, &val, arCtx);
                    pushBackHelper(*pContainer, val);
                }
            }
        }
    }; //rPropTypeInfoSTLList


    template< class ContainerT >
    class rPropTypeInfoSTLSet : public rPropTypeInfoSTLContainerBase<ContainerT>
    {
    public:
        rPropTypeInfoSTLSet(const std::string& arTypeName, const rPropTypeInfo* apElementTypeInfo)
            : rPropTypeInfoSTLContainerBase<ContainerT>(arTypeName, apElementTypeInfo, false)
            {}
        rPropIterator begin(void* apVal, const std::string&) const override
        {
            // std::set only has constant iterator.
            auto* pSet = static_cast<ContainerT*>(apVal);
            rPropIterator iSet(std::make_unique<rPropIteratorSTLSet<ContainerT*, typename ContainerT::const_iterator, rPropIteratorBase, void*>>(this, pSet));
            return iSet;
        }
        rPropConstIterator begin(const void* apVal, const std::string&) const override
        {
            const auto* pSet = static_cast<const ContainerT*>(apVal);
            rPropConstIterator iSet(std::make_unique<rPropConstIterSTLSet<const ContainerT*, typename ContainerT::const_iterator, rPropConstIterBase, const void*>>(this, pSet));
            return iSet;
        }
        /// Returns the number of elements stored in the container.
        std::size_t nrOfElements( const void *apContainer ) const override
        {
            const auto* pList = static_cast<const ContainerT*>(apContainer);
            return pList->size();
        }
        /// When the size is known reserve memory to store all elements without reallocation.
        void setNrOfElements(void* /*apContainer*/, std::size_t /*aSize*/) const override
        {
            // std::list and std::set have no reserve().
        }

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override
        {
            load_(arParser, apVal, arCtx); 
        }
    private:
        template <typename ParserT>
        void load_(ParserT& arParser, void* apVal, rOopsParserContext& arCtx) const
        {
            if (!arCtx.isList()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoSTLSet::load() is called when parser context is not list."));
            }
            auto* pContainer = static_cast<ContainerT*>(apVal);
            const rPropDescriptor* pPDValue = this->elementPropDesc();
            if (pPDValue->isPointer()) {
                while (ParserReturnCode::eListEnd != arParser.parse(arCtx)) {
                    typename ContainerT::value_type ptr;
                    if (!arCtx.pointer_.empty()) {
                        //auto ctx_ptr = arCtx.pointer_; // save it, because calling the parser will change the context.
                        bool unloaded_object = pPDValue->handlePointer(arParser, &ptr, arCtx);
                        //auto insert_ret =
                        pContainer->insert(std::move(ptr));
                        //const typename ContainerT::value_type* pPtrInContainer = &(*getIteratorFromResult(insert_ret));
                        if (unloaded_object) {
                            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Cannot save address in std::set."));
                            //arParser.addToUnloadedObjects(ctx_ptr, pPDValue, pPtrInContainer);
                        }
                    }
                    else {
                        if (ParserReturnCode::ePointer == arCtx.returnCode_) {
                            // Pointer was parsed, pointer_ is NULL, set pMember to nullptr.
                            pPDValue->loadNullPointer(&ptr);
                        }
                        else {
                            auto ctx_addr = arCtx.address_; // store it, because calling the parser will change the context.
                            pPDValue->loadSavedInPlacePointer(arParser, &ptr, arCtx);
                            auto insert_ret = pContainer->insert(std::move(ptr));
                            const typename ContainerT::value_type *pPtrInContainer = &(*getIteratorFromResult(insert_ret));
                            if (!ctx_addr.empty()) {
                                // 'ctx_addr' is NULL if address of the object is not stored in the input stream.
                                // It is not needed, if there is no other reference to the object (e.g. unique_ptr).
                                arParser.addToAlreadyLoadedObjects(ctx_addr, pPDValue, pPtrInContainer);
                            }
                        }
                    }
                }
            }
            else {
                while (ParserReturnCode::eListEnd != arParser.parse(arCtx)) {
                    typename ContainerT::value_type val;
                    pPDValue->load(arParser, &val, arCtx);
                    insertHelper(*pContainer, val);
                }
            }
        }
    }; //rPropTypeInfoSTLSet

} //namespace rOops
