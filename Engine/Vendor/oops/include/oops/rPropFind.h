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
Helper functions for finding properties.
*/

#include <oops/rPropTypeInfo.h>
#include <oops/rPropIter.h>
#include <oops/rPropConstIter.h>
#include <vector>


namespace rOops
{

    template < typename IterT >
    IterT find1stImpl( IterT aiProp, const std::string& arPropName )
    {
        while( !aiProp.isEnd() ) {
            if( aiProp.isAncestor() ) {
                IterT iResult = find1stImpl( aiProp.begin(), arPropName );
                if( !iResult.isEnd() ) return iResult;
            } //if
            if( arPropName == aiProp.propName() ) {
                return aiProp;
            }
            ++aiProp;
        } //while
        if (aiProp.isEnd() && rOops::globalConfig().errorIfPropNameNotFound) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property name not found: " << arPropName));
        }
        else {
            rDbgWarning("Property name not found: " << arPropName);
        }
        return aiProp;
    } //find1stImpl()

    template < class ObjectT >
    rOops::rPropConstIterator find1st( const ObjectT& arObj, const std::string& arPropName)
    {
        return rOops::find1stImpl<rPropConstIterator>(rPropGetTypeInfo(&arObj)->getIter(&arObj, "").begin(), arPropName);
    }

    template < class ObjectT >
    rOops::rPropIterator find1st(ObjectT& arObj, const std::string& arPropName)
    {
        return rOops::find1stImpl<rPropIterator>(rPropGetTypeInfo(&arObj)->getIter(&arObj).begin(), arPropName);
    }


    template < typename IterT, typename ContainerT >
    size_type findAll( IterT aiProp, const std::string& arPropName, ContainerT& arResults )
    {
        while( !aiProp.isEnd() ) {
            if( arPropName == aiProp.PropName() ) {
                arResults.push_back( aiProp );
            }
            if( aiProp.isAncestor() ) {
                findAll( aiProp.begin(), arPropName, arResults );
            } //if
            ++aiProp;
        } //while
        return arResults.size();
    } //findAll()


    template < typename IterT, typename ContainerT >
    size_type findAll( IterT aiProp, const rPropTypeInfo* apPropType, ContainerT& arResults )
    {
        while( !aiProp.isEnd() ) {
            if( apPropType == aiProp.ItemObjTypeInfo() ) {
                arResults.push_back( aiProp );
            }
            if( aiProp.isAncestor() ) {
                findAll( aiProp.begin(), apPropType, arResults );
            } //if
            ++aiProp;
        } //while
        return arResults.size();
    } //findAll()


    /// Parse structured path into a vector of keys.
    /**
    The input string is a '.' separated list of keys.
    Every key describes a property in a structure.
    The left most key belongs to the outermost data structure.
    e.g.:  'cfg/threads[1]/id' or 'cfg/simulator/address'.
    */
    inline std::vector<std::string> parseKeyPath(const std::string& arKeyPath, char aSeparator='.')
    {
        std::vector<std::string> keys;
        std::size_t idx{ 0 }, bgn{ 0 };
        while (idx != std::string::npos) {
            idx = arKeyPath.find(aSeparator, bgn);
            keys.push_back(arKeyPath.substr(bgn, idx-bgn));
            bgn = idx + 1;
        } //while
        return keys;
    } //parseKeyPath()


    template < class IterT >
    IterT& findPath(IterT& arIter, const std::vector<std::string>& keys, std::size_t& keys_idx)
    {
        std::string prop_name;
        while (!arIter.isEnd()) {
            prop_name = arIter.propName();
            if (keys[keys_idx] == prop_name) {
                if (arIter.isCompound()) {
                    ++keys_idx;
                    arIter = arIter.begin();
                }
                else if (arIter.isSTLContainer()) {
                    // Iterate over the container and call findPath() for every element.
                    ++keys_idx;
                    auto cntnr_iter = arIter.begin(); // iterate over the container
                    while (!cntnr_iter.isEnd()) {
                        arIter = cntnr_iter.begin(); // find the next key element in the n-th element of the container.
                        findPath(arIter, keys, keys_idx);
                        if (!arIter.isEnd()) {
                            return arIter;
                        }
                        ++cntnr_iter;
                    }
                    break;
                }
                else if (arIter.isAssociativeContainer()) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Associative containers have not supported yet."));
                    // TODO: How to handle associative containers in property path?
                    // arIter = arIter.begin();
                    break;
                }
                else if (arIter.isValue()) {
                    // Found.
                    if (++keys_idx < keys.size()) {
                        arIter = arIter.begin();
                    }
                    else {
                        return arIter;
                    }
                }
                else {
                    // This should never happen. One and only one of the above isXxx() function must always be true.
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Program error. Unknown container type."));
                }
            }
            else {
                ++arIter;
            }
        } // while
        return arIter;
    }

    /// IterT is rOops::rPropIterator or rOops::rPropConstIterator
    template < class IterT >
    IterT& findPath(IterT& arIter, const std::string& arKeyPath, char aSeparator='.') {
        std::vector<std::string> keys = parseKeyPath(arKeyPath, aSeparator);
        std::size_t keys_idx=0;
        return findPath(arIter, keys, keys_idx);
    }

    template < typename CompoundT, typename ValueT >
    void updateProperty( CompoundT* apThis, const std::string& arOldName, ValueT& arNewMember)
    {
        const rPropTypeInfo* pTI = rPropGetTypeInfo(apThis);
        rOops::rPropIterator iter = pTI->begin(apThis, "");
        rOops::findPath(iter, arOldName);
        if (!iter.isEnd()) {
            pTI = rPropGetTypeInfo(&arNewMember);
            pTI->setValue(&arNewMember, iter.value());
        }
    }

} //namespace rOops
