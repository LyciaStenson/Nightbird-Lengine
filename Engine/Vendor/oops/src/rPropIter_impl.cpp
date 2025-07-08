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

#include <oops/rPropIter_impl.h>
#include <oops/rPropTypeInfo.h>

namespace rOops
{

rPropIteratorBase::rPropIteratorBase( const rPropTypeInfo* apTypeInfo, void* apPropAddr )
    : rPropIteratorBaseCommon(apTypeInfo)
    , _pPropAddr(apPropAddr)
{
}

rPropIteratorBase::~rPropIteratorBase() = default;

rPropConstIterBase::rPropConstIterBase( const rPropTypeInfo* apTypeInfo, const void* apPropAddr )
    : rPropIteratorBaseCommon(apTypeInfo)
    , _pPropAddr(apPropAddr)
{
}

rPropConstIterBase::~rPropConstIterBase() = default;

rPropIteratorCompound::rPropIteratorCompound( const rPropTypeInfo* apTypeInfo, void* apPropAddr )
    : rPropIteratorBase(apTypeInfo,apPropAddr)
    , _idx(0)
{
}

rPropIteratorCompound::~rPropIteratorCompound() = default;

rPropConstIterCompound::rPropConstIterCompound( const rPropTypeInfo* apTypeInfo, const void* apPropAddr )
    : rPropConstIterBase(apTypeInfo,apPropAddr)
    , _idx(0)
{
}

rPropConstIterCompound::~rPropConstIterCompound() = default;

} //namespace rOops

