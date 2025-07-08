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

#include <oops/rPropIter.h>
#include <oops/rPropIter_impl.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropFind.h>


namespace rOops
{

rPropIterator::~rPropIterator() = default;

rPropIterator::rPropIterator() = default;

rPropIterator::rPropIterator( std::unique_ptr<rPropIteratorBase>&& apIter )
    : _pIter(std::move(apIter))
{
}

rPropIterator::rPropIterator( const rPropIterator& arIter )
{
    if (arIter._pIter) {
        _pIter = arIter._pIter->clone();
    }
}

rPropIterator& rPropIterator::operator=( const rPropIterator& arIter )
{
    if (this != &arIter) {
        if (arIter._pIter ) {
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

rPropIterator::rPropIterator( rPropIterator&& ) = default;
rPropIterator& rPropIterator::operator=( rPropIterator&& ) = default;


rPropIterator& rPropIterator::operator++()
{
    if (_pIter) {
        _pIter->next();
        return *this;
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initilized."));
    }
}


void rPropIterator::restart()
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initilized."));
    _pIter->restart();
}


rPropIterator rPropIterator::operator[]( const std::string& arPropName ) const
{
    return find1stImpl( this->begin(), arPropName );
}


//friend
bool operator==( const rPropIterator& arL, const rPropIterator& arR )
{
    if (nullptr == arL._pIter) return (nullptr == arL._pIter);
    if (nullptr == arR._pIter) return (nullptr == arR._pIter);
    if (arL.isEnd()) return arR.isEnd();
    return arL._pIter->isEqual(arR._pIter);
}


//friend
bool operator!=( const rPropIterator& arL, const rPropIterator& arR )
{
    if (nullptr == arL._pIter) return (nullptr != arL._pIter);
    if (nullptr == arR._pIter) return (nullptr != arR._pIter);
    if (arL.isEnd()) return !arR.isEnd();
    return !arL._pIter->isEqual(arR._pIter);
}


//friend
bool operator==( const rPropIterator& arL, const std::string& arR )
{
    std::string value = arL.value();
    return value == arR;
}


//friend
bool operator==( const std::string& arL, const rPropIterator& arR )
{
    std::string value = arR.value();
    return arL == value;
}


bool rPropIterator::isEnd() const
{
    if (_pIter) {
        return _pIter->isEnd();
    }
    else {
        return true;
    }
}


rPropIterator rPropIterator::begin() const
{
    if (!_pIter) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    }
    return itemObjTypeInfo()->begin(_pIter->objAddr(), "");
}


const rPropTypeInfo* rPropIterator::containerPropTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->containerPropTypeInfo();
}


const rPropTypeInfo* rPropIterator::itemPropTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->itemPropTypeInfo();
}


const rPropTypeInfo* rPropIterator::itemObjTypeInfo() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->itemObjTypeInfo();
}


std::string rPropIterator::propName() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->propName();
}

NameIdHashType rPropIterator::propHash() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->propHash();
}

const std::string& rPropIterator::typeName() const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initialized."));
    return _pIter->typeName();
}


std::string rPropIterator::value() const
{
    return itemObjTypeInfo()->value(objAddr());
}


void rPropIterator::value( std::string& arValue ) const
{
    itemObjTypeInfo()->value(objAddr(),arValue);
}

void rPropIterator::setValue( const std::string& arValue ) const
{
    if (!_pIter) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property iterator is not initilized."));
    if (_pIter->isEnd()) {
        // TODO: How to check if the container is STL container?
        // _pIter->parent()->PropTypeInfo()->isSTLContainer()
        _pIter->append(arValue);
    }
    else {
        _pIter->assign(arValue);   //itemObjTypeInfo()->setValue(objAddr(), arValue);
    }
}

//void rPropIterator::addUnknownProperty(const std::string& arType, const std::string& arName, const std::string& arValue) const
//{
//    if (isCompound()) {
//        itemObjTypeInfo()->addUnknownProperty(objAddr(), arType, arName, arValue);
//    }
//}


//void rPropIterator::setUnknownProperty(const size_t aDescTableIndex, const std::string& arValue) const
//{
//    if (isCompound()) {
//        itemObjTypeInfo()->setUnknownProperty(objAddr(), aDescTableIndex, arValue);
//    }
//}


const std::string& rPropIterator::getUnknownProperty() const
{
    if (!_pIter || !isCompound()) {
        return static_empty_string;
    }
    else {
        return itemObjTypeInfo()->getUnknownProperty(objAddr(), getPropDescTableIndex());
    }
}


void* rPropIterator::propAddr() const
{
    if(!_pIter) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE( "rPropIterator::propAddr(): invalid iterator: _pIter is null."));
    }
    if (_pIter->isEnd()) return nullptr;
    return _pIter->propAddr();
}


void* rPropIterator::objAddr() const
{
    if (!_pIter) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE( "rPropIterator::objAddr(): invalid iterator: _pIter is null."));
    }
    if (_pIter->isEnd()) return nullptr;
    return _pIter->objAddr();
}


void rPropIterator::setObjAddr( void* apAddr ) const
{
    if (!_pIter) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE( "rPropIterator::setObjAddr(): invalid iterator: _pIter is null."));
    }
    if( !_pIter->isPointer() ) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("setObjAddr() is called on non pointer property."));
    if (_pIter->isEnd()) {
        _pIter->append(apAddr);
    }
    else {
        _pIter->setObjAddr(apAddr);
    }
}


bool rPropIterator::checkFlags( flags_type aFilter ) const
{
    if(nullptr == _pIter ) return false;
    return _pIter->checkFlags(aFilter);
}


bool rPropIterator::isPointer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isPointer();
}


bool rPropIterator::isValue() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isValue();
}


bool rPropIterator::isCompound() const
{
    if(nullptr == _pIter ) return false;
    // No need to find polymorph type info. Base class is compound as well.
    return _pIter->itemObjTypeInfo()->isCompound();
}


bool rPropIterator::isAncestor() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isAncestor();
}

bool rPropIterator::isSTLContainer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isSTLContainer();
}


bool rPropIterator::isAssociativeContainer() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->itemObjTypeInfo()->isAssociativeContainer();
}

bool rPropIterator::isRunTime() const
{
    if(nullptr == _pIter ) return false;
    return _pIter->isRunTime();
}

size_type rPropIterator::getPropDescTableIndex() const
{
    if(nullptr == _pIter ) return -1;
    return _pIter->getPropDescTableIndex();
}


std::size_t rPropIterator::nrOfElements() const
{
    if(nullptr == _pIter ) return 0;
    return _pIter->itemObjTypeInfo()->nrOfElements(_pIter->objAddr());
}


void rPropIterator::setNrOfElements( std::size_t aSize ) const
{
    if(nullptr == _pIter ) return;
    _pIter->itemObjTypeInfo()->setNrOfElements(_pIter->objAddr(),aSize);
}


const rPropTypeInfo* rPropIterator::elementTypeInfo() const
{
    if(nullptr == _pIter ) return nullptr;
    return _pIter->itemObjTypeInfo()->elementTypeInfo();
}


const rPropTypeInfo* rPropIterator::keyTypeInfo() const
{
    if(nullptr == _pIter ) return nullptr;
    return _pIter->itemObjTypeInfo()->keyTypeInfo();
}


} //namespace rOops
