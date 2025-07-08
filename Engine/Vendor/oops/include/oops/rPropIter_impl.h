#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2019:
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
Property Iterator implementations.
*/


#include <oops/rPropIter.h>
#include <oops/rPropDescriptor.h>
#include <oops/rPropTypeTraits.h>
#include <memory>
#include <string>


namespace rOops
{


/// Interface class of all iterators.
class rPropIteratorBaseCommon
{
public:
    virtual ~rPropIteratorBaseCommon() = default;
    rPropIteratorBaseCommon() = delete;
    rPropIteratorBaseCommon(const rPropIteratorBaseCommon&) = default;
    rPropIteratorBaseCommon(rPropIteratorBaseCommon&&) = delete;
    rPropIteratorBaseCommon& operator=(const rPropIteratorBaseCommon&) = delete;
    rPropIteratorBaseCommon& operator=(rPropIteratorBaseCommon&&) = delete;
    explicit rPropIteratorBaseCommon( const rPropTypeInfo* apTypeInfo )
        : _pTypeInfo(apTypeInfo)
        {}
    virtual void next() = 0;
    virtual void restart() = 0;
    virtual bool isEnd() const = 0;
    /// Returns a pointer to the type info structure of the container where the itarator is iterating over.
    /** When iterating through a compound object the type info of the compound object is returned.
    When iterating through a container the type info of the container is returned. */
    virtual const rPropTypeInfo* containerPropTypeInfo() const = 0;
    /// Returns a pointer to the type info structure of the pointed element or object.
    /** When iterating through a compound object the type info of the pointed member is returned.
    When iterating through a container the type info of the pointed element is returned. */
    virtual const rPropTypeInfo* itemPropTypeInfo() const = 0;
    virtual const rPropTypeInfo* itemObjTypeInfo() const = 0;
    virtual const rPropTypeInfo* keyTypeInfo() const { return _pTypeInfo->keyTypeInfo(); }
    virtual std::string propName() const = 0;
    virtual NameIdHashType propHash() const = 0;
    virtual const std::string& typeName() const
        { return this->_pTypeInfo->typeName(); }
    virtual bool isPointer() const = 0;
    virtual bool isAncestor() const
        { return false; }
    virtual bool isRunTime() const
        { return false; }
    virtual bool checkFlags( flags_type aFilter ) const = 0;
    /// Return the index of the property in the Property Descriptor table.
    /**
    Only compound types implement this function. All others return -1.
    It is used for checking if required properties are read from the input stream.
    */
    virtual size_type getPropDescTableIndex() const
        { return -1; }
protected:
    const rPropTypeInfo* _pTypeInfo{ nullptr }; /// Type info of the parent (compound or container) object.
}; //class rPropIteratorBaseCommon


/// Base class for non const iterators.
/**
Declare the iterator interface for non const iterators.
It inherits the common iterator interface and adds functions to modify the container.
*/
class rPropIteratorBase : public rPropIteratorBaseCommon
{
public:
    rPropIteratorBase( const rPropTypeInfo* apTypeInfo, void* apPropAddr );
    ~rPropIteratorBase() override;
    virtual std::unique_ptr<rPropIteratorBase> clone() const = 0;
    virtual bool isEqual(const std::unique_ptr<rPropIteratorBase>& apR) const = 0;
    virtual void* propAddr() const = 0;
    virtual void* objAddr() const = 0;
    //virtual void setObjAddr(void* /*apAddr*/) const = 0;
    virtual void setObjAddr(void* /*apAddr*/) const
    {
        throw std::runtime_error("Not implemented. Program error: this function should not be called.");
    }
    virtual const void* keyAddr() const { return nullptr; }
    /// Assign the value to the pointed member or element. Called from setValue().
    virtual void assign(const std::string& arValue)
        { itemObjTypeInfo()->setValue(objAddr(), arValue); }
    /// Push or insert a new element to the container.
    /// Const iterators cannot implement it.
    /// The parameter is the value of the new element.
    virtual void append(void*)
        {}
    virtual void append(const std::string&)
        {}
protected:
    void* _pPropAddr; /// Address of the compound or container object.
}; //class rPropIteratorBase


/// Base class for const iterators.
/**
Declare the iterator interface for const iterators.
It inherits the common iterator interface and adds functions using const references to container elements.
*/
class rPropConstIterBase : public rPropIteratorBaseCommon
{
public:
    rPropConstIterBase( const rPropTypeInfo* apTypeInfo, const void* apPropAddr );
    ~rPropConstIterBase() override;
    virtual std::unique_ptr<rPropConstIterBase> clone() const = 0;
    virtual bool isEqual(const std::unique_ptr<rPropConstIterBase>& apR) const = 0;
    virtual const void* propAddr() const = 0;
    virtual const void* objAddr() const = 0;
    // setObjAddr(void* /*apAddr*/) is not exists for const iterator.
    virtual const void* keyAddr() const { return nullptr; }
protected:
    const void* _pPropAddr; /// Address of the compound or container object.
}; //class rPropConstIterBase


template < typename BaseT, class VoidPtrT >
class rPropIteratorValueTemplate : public BaseT
{
public:
    rPropIteratorValueTemplate( const rPropTypeInfo* apTypeInfo, VoidPtrT apPropAddr, std::string arName )
        : BaseT(apTypeInfo,apPropAddr)
        , _pStartAddr(apPropAddr)
        , _name(std::move(arName))
        {}
    ~rPropIteratorValueTemplate() override = default;
    std::unique_ptr<BaseT> clone() const override
        { return std::make_unique<rPropIteratorValueTemplate>(*this); }
    void next() override
        { this->_pPropAddr = nullptr; }
    void restart() override
        { this->_pPropAddr = _pStartAddr; }
    bool isEnd() const override
        { return (nullptr==this->_pPropAddr); }
    bool isEqual(const std::unique_ptr<BaseT>& apR) const override
        {
            auto pR = dynamic_cast<const rPropIteratorValueTemplate*>(apR.get());
            if (nullptr == pR) return false;
            return (this->_pPropAddr == pR->_pPropAddr);
        }
    const rPropTypeInfo* containerPropTypeInfo() const override
        { return this->_pTypeInfo; }
    const rPropTypeInfo* itemPropTypeInfo() const override
        { return this->_pTypeInfo; }
    const rPropTypeInfo* itemObjTypeInfo() const override
        { return this->_pTypeInfo; }
    std::string propName() const override
        { return _name; }
    NameIdHashType propHash() const override
        { return generateNameId(_name); }
    VoidPtrT propAddr() const override
        { return this->_pPropAddr; }
    VoidPtrT objAddr() const override
        { return this->_pPropAddr; } // TODO: ???
    bool isPointer() const override
        { return false; }
    bool checkFlags( flags_type /*aFilter*/ ) const override
        { return false; }
protected:
    VoidPtrT _pStartAddr{ nullptr }; /// copy of _pPropAddr for restart.
    std::string _name;
}; //class rPropIteratorValueTemplate

using rPropIteratorValue = rPropIteratorValueTemplate< rPropIteratorBase, void* >;
using rPropConstIterValue = rPropIteratorValueTemplate< rPropConstIterBase, const void* >;


class rPropIteratorCompound : public rPropIteratorBase
{
public:
    rPropIteratorCompound( const rPropTypeInfo* apTypeInfo, void* apPropAddr );
    ~rPropIteratorCompound() override;
    std::unique_ptr<rPropIteratorBase> clone() const override
        { return std::make_unique<rPropIteratorCompound>(*this); }
    void next() override
        { ++_idx; }
    void restart() override
        { _idx = 0; }
    bool isEnd() const override
        { return (0==getCur()); }
    inline bool isEqual(const std::unique_ptr<rPropIteratorBase>& apR) const override;
    const rPropTypeInfo* containerPropTypeInfo() const override
        { return _pTypeInfo; }
    //const rPropTypeInfo* ContainerObjTypeInfo() const override
    //    { return _pTypeInfo->objTypeInfo(_pPropAddr); }
    const rPropTypeInfo* itemPropTypeInfo() const override
        { return getCur()->propTypeInfo(); }
    const rPropTypeInfo* itemObjTypeInfo() const override
        { return getCur()->objTypeInfo(objAddr()); }
    std::string propName() const override
        { return getCur()->propName(); }
    NameIdHashType propHash() const override
        { return getCur()->propHash(); }
    const std::string& typeName() const override
        { return getCur()->typeName(); }
    void* propAddr() const override
        { return getCur()->propAddr(_pPropAddr); }
    void* objAddr() const override
        { return getCur()->objAddr(_pPropAddr); }
    void setObjAddr(void* apAddr) const override
        { getCur()->setObjAddr(_pPropAddr, apAddr); }
    void assign(const std::string& arValue) override
        { itemObjTypeInfo()->setValue(objAddr(), arValue); }
    bool isPointer() const override
        { return getCur()->isPointer(); }
    bool isAncestor() const override
        { return getCur()->isAncestor(); }
    bool isRunTime() const override
        { return getCur()->isRunTime(); }
    bool checkFlags( flags_type aFilter ) const override
        { return getCur()->checkFlags(aFilter); }
    size_type getPropDescTableIndex() const override
        { return _idx; }
protected:
    const rPropDescriptor* getCur() const
        { return _pTypeInfo->getPropDesc(_idx); }
protected:
    size_type _idx;
}; //class rPropIteratorCompound


class rPropConstIterCompound : public rPropConstIterBase
{
public:
    rPropConstIterCompound( const rPropTypeInfo* apTypeInfo, const void* apPropAddr );
    ~rPropConstIterCompound() override;
    std::unique_ptr<rPropConstIterBase> clone() const override
        { return std::make_unique<rPropConstIterCompound>(*this); }
    void next() override
        { ++_idx; }
    void restart() override
        { _idx = 0; }
    bool isEnd() const override
        { return (0==getCur()); }
    inline bool isEqual(const std::unique_ptr<rPropConstIterBase>& apR) const override;
    const rPropTypeInfo* containerPropTypeInfo() const override
        { return _pTypeInfo; }
    const rPropTypeInfo* itemPropTypeInfo() const override
        { return getCur()->propTypeInfo(); }
    const rPropTypeInfo* itemObjTypeInfo() const override
        { return getCur()->objTypeInfo(objAddr()); }
    std::string propName() const override
        { return getCur()->propName(); }
    NameIdHashType propHash() const override
        { return getCur()->propHash(); }
    const std::string& typeName() const override
        { return getCur()->typeName(); }
    const void* propAddr() const override
        { return getCur()->propAddr(_pPropAddr); }
    const void* objAddr() const override
        { return getCur()->objAddr(_pPropAddr); }
    bool isAncestor() const override
        { return getCur()->isAncestor(); }
    bool isRunTime() const override
        { return getCur()->isRunTime(); }
    bool isPointer() const override
        { return getCur()->isPointer(); }
    bool checkFlags( flags_type aFilter ) const override
        { return getCur()->checkFlags(aFilter); }
    size_type getPropDescTableIndex() const override
        { return _idx; }
protected:
    const rPropDescriptor* getCur() const
        { return _pTypeInfo->getPropDesc(_idx); }
protected:
    size_type _idx;
}; //class rPropConstIterCompound


template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropIteratorSTLBase : public PropIterBaseT
{
public:
    rPropIteratorSTLBase(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : PropIterBaseT(apTypeInfo, apCntr)
        , _pCntr(apCntr)
        {
            _iCur = _pCntr->begin();
        }
    ~rPropIteratorSTLBase() override = default;
    void next() override
        { if (_iCur!=_pCntr->end()) ++_iCur; }
    void restart() override
        { _iCur = _pCntr->begin(); }
    bool isEnd() const override
        { return (_iCur==_pCntr->end()); }
    bool isEqual(const std::unique_ptr<PropIterBaseT>& apR) const override
        {
            auto pR = dynamic_cast<const rPropIteratorSTLBase*>(apR.get());
            if (nullptr == pR) return false;
            return (this->_pCntr == pR->_pCntr) && (this->_iCur == pR->_iCur);
        }
    const rPropTypeInfo* containerPropTypeInfo() const override
        { return this->_pTypeInfo; }
    const rPropTypeInfo* itemPropTypeInfo() const override
        { return this->_pTypeInfo->elementPropDesc()->propTypeInfo(); }
    const rPropTypeInfo* itemObjTypeInfo() const override
        { return this->_pTypeInfo->elementPropDesc()->objTypeInfo(this->objAddr()); }
    std::string propName() const override
        { return {""}; }
    NameIdHashType propHash() const override
        { return 0u; }
    bool isPointer() const override
        { return this->_pTypeInfo->elementPropDesc()->isPointer(); }
    bool checkFlags( flags_type aFilter ) const override
        { return this->_pTypeInfo->elementPropDesc()->checkFlags(aFilter); }
protected:
    ContainerPtrT _pCntr;
    IterT _iCur;
}; //class rPropIteratorSTLBase


/// Array Iterator.
template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropIteratorArray : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropIteratorArray(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropIteratorArray>(*this);
    }
    VoidPtrT propAddr() const override
    {
        return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(*(this->_iCur)));
    }
    void setObjAddr(void* /*apAddr*/) const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented. Program error: this function should not be called."));
    }
    void assign(const std::string& arValue) override
    {
        if (this->_iCur == this->_pCntr->end()) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("assign() is called when iterator points to the end."));
        }
        // How to assign depends on the value_type: base type, pointer, or smart pointer.
        this->itemObjTypeInfo()->setValue(&(*(this->_iCur)), arValue);
        // How to assign depends on the value_type: base type, pointer, or smart pointer.
        // ???? assignHelper<typename rOops::rPropRemovePointer<ContainerPtrT>::type::value_type>(*(this->_iCur), arValue);
    }
}; //class rPropIteratorArray

template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropConstIterArray : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropConstIterArray(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropConstIterArray>(*this);
    }
    VoidPtrT propAddr() const override
    {
        return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(*(this->_iCur)));
    }
};


/// List iterator.
template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropIteratorSTLList : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropIteratorSTLList(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropIteratorSTLList>(*this);
    }
    void assign(const std::string&) override
        { throw std::runtime_error("Program error: assign() should not be called for std::list style containers."); }
    void append(void* apValue) override
    {
        if (this->_iCur != this->_pCntr->end()) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("append() is called when iterator does not point to the end."));
        // If the list contains smart pointers the apValue has to be assigned to a smart pointer object,
        // and he smart pointer should be pushed to the list.
        // The value type of the container may be the same as the pointed type, or some kind of pointer to the pointed type.
        typename IterT::value_type val;
        // How to assign depends on the value_type: base type, pointer, or smart pointer.
        assignHelper<typename IterT::value_type>(val, apValue);
        this->_pCntr->push_back( std::move(val) );
        this->_iCur = this->_pCntr->end();
        --(this->_iCur); // TODO: do we need this? All the others leave _iCur at the end. Try to remove and run all tests. Yes, it fails without it, but I do not know why.
    }
    void append(const std::string& arValue) override
    {
        if (this->_iCur == this->_pCntr->end()) {
            this->_pCntr->push_back(typename IterT::value_type());
            this->_iCur = this->_pCntr->end();
            --(this->_iCur); // TODO: do we need this? All the others leave _iCur at the end. Try to remove and run all tests. Yes, the iterator is used to set the value.
            this->itemObjTypeInfo()->setValue(&(*(this->_iCur)), arValue);
        }
    }
    VoidPtrT propAddr() const override
    {
        return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(*(this->_iCur)));
    }
    void setObjAddr(void* /*apAddr*/) const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented. Program error: this function should not be called."));
    }
}; //class rPropIteratorSTLList

template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropConstIterSTLList : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropConstIterSTLList(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropConstIterSTLList>(*this);
    }
    VoidPtrT propAddr() const override
    {
        return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(*(this->_iCur)));
    }
};


/// Set iterator.
template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropIteratorSTLSet : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropIteratorSTLSet(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropIteratorSTLSet>(*this);
    }
    void assign(const std::string&) override
        { throw std::runtime_error("Program error: assign() should not be called for std::set style containers."); }
    void append(void* apValue) override
    {
        typename IterT::value_type val;
        assignHelper<typename IterT::value_type>(val, apValue);
        this->_pCntr->insert( std::move(val) );
        this->_iCur = this->_pCntr->end();
    }
    void append(const std::string& arValue) override
    {
        // Value_type may be basic type, pointer type, or smart pointer, but this code has to compile and work in all cases.
        // TODO: cannot instantiate abstract class
        // TODO: find another solution which handles polymorhism.
        typename rPropRemovePointer<typename IterT::value_type>::type tmp_val;
        this->_pTypeInfo->elementTypeInfo()->setValue(&tmp_val, arValue);
        typename IterT::value_type val;
        assignHelper<typename IterT::value_type>(val, &tmp_val);
        this->_pCntr->insert(val);
        this->_iCur = this->_pCntr->end();
    }
    VoidPtrT propAddr() const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented."));
        // Elements cannot be modified in std::set.
        //return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented."));
        // Elements cannot be modified in std::set.
        //const void* value_address = &(*(this->_iCur));
        //return this->_pTypeInfo->elementPropDesc()->objAddr(const_cast<void*>(value_address));
    }
    void setObjAddr(void* /*apAddr*/) const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented. Program error: this function should not be called."));
    }
}; //class rPropIteratorSTLSet

template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropConstIterSTLSet : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropConstIterSTLSet(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropConstIterSTLSet>(*this);
    }
    VoidPtrT propAddr() const override
    {
        return &(*(this->_iCur));
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(*(this->_iCur)));
    }
}; //class rPropConstIterSTLSet

namespace {
    template < typename IteratorT >
    IteratorT& getIteratorFromResult(IteratorT& arIter) { return arIter; }
    template < typename IteratorT >
    IteratorT& getIteratorFromResult(std::pair<IteratorT, bool>& arPair) { return arPair.first; }
}

template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropIteratorAssociative : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
    using key_type = typename std::remove_const< typename IterT::value_type::first_type >::type;
    using mapped_type = typename IterT::value_type::second_type;
public:
    rPropIteratorAssociative(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropIteratorAssociative>(*this);
    }
    void assign(const std::string& arValue) override
        {
            // Assign is called to set the value in the value of the pair already inserted in the map.
            // Value_type may be basic type, pointer type, or smart pointer, but this code has to compile and work in all cases.
            mapped_type tmp_val;
            this->_pTypeInfo->elementTypeInfo()->setValue(&tmp_val, arValue);
            assignHelper<mapped_type>(this->_iCur->second, &tmp_val);
            this->_iCur = this->_pCntr->end();
        }
    // Associative containers has a different append for adding a key value pair.
    void append(void*) override
        { throw std::runtime_error("Program error: append() should not be called for associative containers."); }
    void append(const std::string& arValue) override
    {
        // Append if called for inserting a new key.
        // Value_type may be basic type, pointer type, or smart pointer, but this code has to compile and work in all cases.
        // TODO: Pointer?
        key_type tmp_key{};
        this->_pTypeInfo->keyTypeInfo()->setValue(&tmp_key, arValue);
        key_type key{};
        mapped_type val{}; // Note, that val may be a pointer, and in that case a nullptr is stored in the map.
        assignHelper<key_type>(key, &tmp_key);
        auto result = this->_pCntr->emplace(key, std::move(val));
        this->_iCur = getIteratorFromResult(result);
    }
    VoidPtrT propAddr() const override
    {
        return &(this->_iCur->second);
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(this->_iCur->second));
    }
    const void* keyAddr() const override
    {
        return &(this->_iCur->first);
    }
    void setObjAddr(void* /*apAddr*/) const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented. Program error: this function should not be called."));
    }
}; //class rPropIteratorAssociative

template < typename ContainerPtrT, typename IterT, class PropIterBaseT, class VoidPtrT >
class rPropConstIterAssociative : public rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>
{
public:
    rPropConstIterAssociative(const rPropTypeInfo* apTypeInfo, ContainerPtrT apCntr)
        : rPropIteratorSTLBase<ContainerPtrT, IterT, PropIterBaseT, VoidPtrT>(apTypeInfo, apCntr)
    {}
    std::unique_ptr<PropIterBaseT> clone() const override
    {
        return std::make_unique<rPropConstIterAssociative>(*this);
    }
    VoidPtrT propAddr() const override
    {
        return &(this->_iCur->second);
    }
    VoidPtrT objAddr() const override
    {
        return this->_pTypeInfo->elementPropDesc()->objAddr(&(this->_iCur->second));
    }
    const void* keyAddr() const override
    {
        return &(this->_iCur->first);
    }
}; //class rPropConstIterAssociative




inline
bool rPropIteratorCompound::isEqual(const std::unique_ptr<rPropIteratorBase>& apR) const
{
    {
        auto pR = dynamic_cast<const rPropIteratorCompound*>(apR.get());
        if (nullptr != pR) {
            return (this->getPropDescTableIndex() == pR->getPropDescTableIndex());
        }
    }
    {
        auto pConstR = dynamic_cast<const rPropConstIterCompound*>(apR.get());
        if (nullptr == pConstR) return false;
        return (this->getPropDescTableIndex() == pConstR->getPropDescTableIndex());
    }
}


inline
bool rPropConstIterCompound::isEqual(const std::unique_ptr<rPropConstIterBase>& apR) const
{
    {
        auto pConstR = dynamic_cast<const rPropConstIterCompound*>(apR.get());
        if (nullptr != pConstR) {
            return (this->getPropDescTableIndex() == pConstR->getPropDescTableIndex());
        }
    }
    {
        auto pR = dynamic_cast<const rPropIteratorCompound*>(apR.get());
        if (nullptr == pR) return false;
        return (this->getPropDescTableIndex() == pR->getPropDescTableIndex());
    }
}


} //namespace rOops
