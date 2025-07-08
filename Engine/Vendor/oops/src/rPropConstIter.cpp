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
Property Constant Iterator handle class.
*/

#include <oops/rPropConstIter.h>
#include <oops/rPropIter_impl.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropFind.h>


namespace rOops
{

rPropConstIterator::~rPropConstIterator() = default;

rPropConstIterator::rPropConstIterator() = default;

rPropConstIterator::rPropConstIterator( std::unique_ptr<rPropConstIterBase>&& apIter )
    : _pIter(std::move(apIter))
{
}

rPropConstIterator::rPropConstIterator( const rPropConstIterator& arIter )
{
    if (arIter._pIter) {
        _pIter = arIter._pIter->clone();
    }
}

rPropConstIterator& rPropConstIterator::operator=( const rPropConstIterator& arIter )
{
    if( this != &arIter ) {
        if(arIter._pIter ) {
            _pIter = arIter._pIter->clone();
        }
        else {
            if (_pIter) {
                _pIter.reset();
            }
        }
    }
    return *this;
}

rPropConstIterator::rPropConstIterator( rPropConstIterator&& ) = default;

rPropConstIterator& rPropConstIterator::operator=( rPropConstIterator&& ) = default;


rPropConstIterator& rPropConstIterator::operator++()
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    _pIter->next();
    return *this;
}


rPropConstIterator rPropConstIterator::operator[]( const std::string& arPropName ) const
{
    return find1stImpl( this->begin(), arPropName );
}


//friend
bool operator==( const rPropConstIterator& arL, const rPropConstIterator& arR )
{
    if (nullptr == arL._pIter) return (nullptr == arL._pIter);
    if (nullptr == arR._pIter) return (nullptr == arR._pIter);
    if (arL.isEnd()) return arR.isEnd();
    return arL._pIter->isEqual(arR._pIter);
}


//friend
bool operator!=( const rPropConstIterator& arL, const rPropConstIterator& arR )
{
    if (nullptr == arL._pIter) return (nullptr != arL._pIter);
    if (nullptr == arR._pIter) return (nullptr != arR._pIter);
    if (arL.isEnd()) return !arR.isEnd();
    return !arL._pIter->isEqual(arR._pIter);
}


//friend
bool operator==( const rPropConstIterator& arL, const std::string& arR )
{
    std::string value = arL.value();
    return value == arR;
}


//friend
bool operator==( const std::string& arL, const rPropConstIterator& arR )
{
    std::string value = arR.value();
    return arL == value;
}


bool rPropConstIterator::isEnd() const
{
    if (!_pIter) return true;
    return _pIter->isEnd();
}


rPropConstIterator rPropConstIterator::begin() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    return itemObjTypeInfo()->begin(_pIter->objAddr(), "");
}


rPropConstIterator rPropConstIterator::keyIter() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initilized."));
    // TODO: if (polymorph()
    return _pIter->keyTypeInfo()->begin(_pIter->keyAddr(), "");
}


const void* rPropConstIterator::keyAddr() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initilized."));
    return _pIter->keyAddr();
}

const rPropTypeInfo* rPropConstIterator::containerPropTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    return _pIter->containerPropTypeInfo();
}


const rPropTypeInfo* rPropConstIterator::itemPropTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    return _pIter->itemPropTypeInfo();
}


const rPropTypeInfo* rPropConstIterator::itemObjTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->itemObjTypeInfo();
}


std::string rPropConstIterator::propName() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->propName();
}

NameIdHashType rPropConstIterator::propHash() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->propHash();
}


const std::string& rPropConstIterator::typeName() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->typeName();
}


std::string rPropConstIterator::value() const
{
    return itemObjTypeInfo()->value(objAddr());
}


void rPropConstIterator::value( std::string& arValue ) const
{
    itemObjTypeInfo()->value(objAddr(),arValue);
}


/*
void rPropConstIterator::setValue( const std::string& arValue ) const
{
    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Cannot set with const iterator."));
    itemObjTypeInfo()->setValue(objAddr(),arValue);
}
*/


const std::string& rPropConstIterator::getUnknownProperty() const
{
    if(!_pIter || !_pIter->containerPropTypeInfo()->isCompound()) {
        return static_empty_string;
    }
    else {
        //return containerPropTypeInfo()->getUnknownProperty(PropAddr(), getPropDescTableIndex());
        return containerPropTypeInfo()->getUnknownProperty(objAddr(), getPropDescTableIndex());
    }
}


const void* rPropConstIterator::propAddr() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    return _pIter->propAddr();
}


const void* rPropConstIterator::objAddr() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Propert iterator is not initilized."));
    return _pIter->objAddr();
}


bool rPropConstIterator::checkFlags( flags_type aFilter ) const
{
    if(nullptr == _pIter ) return false;
    return _pIter->checkFlags(aFilter);
}


bool rPropConstIterator::isPointer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isPointer();
}


bool rPropConstIterator::isValue() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isValue();
}


bool rPropConstIterator::isCompound() const
{
    if(nullptr == _pIter ) return false;
    // No need to find polymorph type info. Base class is compound as well.
    return _pIter->itemPropTypeInfo()->isCompound();
}


bool rPropConstIterator::isAncestor() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isAncestor();
}


bool rPropConstIterator::isSTLContainer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isSTLContainer();
}


bool rPropConstIterator::isAssociativeContainer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isAssociativeContainer();
}

bool rPropConstIterator::isRunTime() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isRunTime();
}

size_type rPropConstIterator::getPropDescTableIndex() const
{
    if(nullptr == _pIter ) return -1;
    return _pIter->getPropDescTableIndex();
}


std::size_t rPropConstIterator::nrOfElements() const
{
    if(nullptr == _pIter ) return 0;
    return _pIter->itemObjTypeInfo()->nrOfElements(_pIter->objAddr());
}


const rPropTypeInfo* rPropConstIterator::elementTypeInfo() const
{
    if(nullptr == _pIter ) return nullptr;
    return _pIter->itemObjTypeInfo()->elementTypeInfo();
}


const rPropTypeInfo* rPropConstIterator::keyTypeInfo() const
{
    if(nullptr == _pIter ) return nullptr;
    return _pIter->itemObjTypeInfo()->keyTypeInfo();
}


} //namespace rOops
