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
Property Iterator handle class.
*/

#include <oops/rPropLog.h>
#include <oops/rPropTypeDef.h>
#include <oops/rOopsConfig.h>
#include <memory>
#include <string>
#include <vector>


namespace rOops
{


class rPropTypeInfo;
class rPropIteratorBase;


/** Constructed from the container object address and descriptor,
and provide information and access to the pointed element. */
class rPropIterator final
{
public:
    ~rPropIterator();
    rPropIterator();
    rPropIterator( std::unique_ptr<rPropIteratorBase>&& apIter );
    rPropIterator( const rPropIterator& arIter );
    rPropIterator& operator=( const rPropIterator& arIter );
    rPropIterator( rPropIterator&& arIter );
    rPropIterator& operator=( rPropIterator&& arIter );
    rPropIterator& operator++();
    /// Move the iterator to the first element. It is used to iterate over the last element and continue the iteration at the beginning.
    void restart();
    rPropIterator operator[]( const std::string& arMemberName ) const;
    friend bool operator==( const rPropIterator& arL, const rPropIterator& arR );
    friend bool operator!=( const rPropIterator& arL, const rPropIterator& arR );
    friend bool operator==( const rPropIterator& arL, const std::string& arR );
    friend bool operator==( const std::string& arL, const rPropIterator& arR );
    bool isValid() const
        { return nullptr!=_pIter; }
    bool isEnd() const;
    rPropIterator begin() const;
    /// Get the type information of the container the iterator is moving over.
    const rPropTypeInfo* containerPropTypeInfo() const;
    /// Get the type information of the pointed element.
    const rPropTypeInfo* itemPropTypeInfo() const;
    const rPropTypeInfo* itemObjTypeInfo() const;
    std::string propName() const;
    NameIdHashType propHash() const;
    const std::string& typeName() const;
    std::string value() const;
    void value( std::string& arValue ) const;
    void setValue( const std::string& arValue ) const;
    //void addUnknownProperty(const std::string& arType, const std::string& arName, const std::string& arValue) const;
    //void setUnknownProperty(const size_t aDescTableIndex, const std::string& arValue) const;
    const std::string& getUnknownProperty() const;
    void* propAddr() const;
    void* objAddr() const;
    template <typename T> T* ObjPtr() const
        {
            if( itemObjTypeInfo() != rPropGetTypeInfo(static_cast<const T*>(nullptr)) ) return static_cast<const T*>(nullptr);
            return reinterpret_cast<T*>( objAddr() );
        }
    void setObjAddr( void* apAddr ) const;
    bool checkFlags( flags_type aFilter ) const;
    bool isPointer() const;
    bool isValue() const;
    bool isCompound() const;
    bool isAncestor() const;
    bool isSTLContainer() const;
    bool isAssociativeContainer() const;
    bool isRunTime() const;
    /// Return the index of the property in the Property Descriptor table.
    /**
    Only compound types implement this function. All others return -1.
    It is used for checking if required properties are read from the input stream.
    */
    size_type getPropDescTableIndex() const;
    /// Returns the number of elements stored in the container.
    std::size_t nrOfElements() const;
    /// Sets the size of a container. Nothing to do in all other cases.
    void setNrOfElements( std::size_t aSize ) const;
    /// Returns the type info object of elements stored in the container.
    const rPropTypeInfo* elementTypeInfo() const;
    const rPropTypeInfo* keyTypeInfo() const;
protected:
    std::unique_ptr<rPropIteratorBase> _pIter;
}; //class rPropIterator

} //namespace rOops
