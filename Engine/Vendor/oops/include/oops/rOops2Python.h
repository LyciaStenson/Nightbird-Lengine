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
Output Stream class based on oops iterators for testing if iterators work correctly.
*/

#include <oops/rPropConstIter.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropLog.h>
#include <oops/rPropDescriptor.h>
#include <list>
#include <set>
#include <string>

namespace rOops
{

class rOops2Python
{
public:
    explicit rOops2Python(ostream_type& arStrm);
    ~rOops2Python();
    void saveObj( const rPropConstIterator& arPropIter );
    void saveClassDefinitions(ostream_type& arStrm);
    void saveAllClassDefinitions(ostream_type& arStrm);
    void saveClassDefinition(ostream_type& arStrm, const rPropTypeInfo* tip);
protected:
    enum class SaveState_e {eKey, eBlock, eList, eMap};
    void saveObjRecursive_( const rPropConstIterator &arPropIter, SaveState_e aSaveState );
protected:
    void writeData_(const void* apObjAddr);
    void writePointedData_(const void* apObjAddr);
    void writePointer_(const void* apObjAddr);
    void writeSizeTypeAndName_();
    void writeBlockBegin_(const rPropTypeInfo* apTI);
    void writeBlockEnd_();
    void writeListBegin_();
    void writeListEnd_();
    void writeMapBegin_();
    void writeMapEnd_();
    void writeValueSeparator_(bool aEnd);
    void writeListSeparator_(bool aEndOf);
    void writeKeyValueSeparator_();
    void clearContext_();
    void doIndent_();
    void writeEndOfLine_();
protected:
    // State:
    unsigned inBlock_{ 0 }; ///< writing compound data, between {}
    unsigned inCntnr_{ 0 }; ///< writing container, between []
    int indent_{ 0 }; ///<  indentation level.
    bool endOfLineAlreadyWritten_{ false }; ///< Avoid writing new lines several times.
    bool doNotIdentNow_{ false }; ///< Avoid ident in the middle of the line. E.g. after list separator.
    // Context:
    const rPropTypeInfo* typeInfo_{ nullptr };
    std::string propName_;
    std::ostream& strm_;
    /// Map of address and node of objects.
    /// It is used for finding multiple references to the same object.
//    std::set<const void*> objIdSet_;
//    std::list<rPropConstIterator> unsaved_;
    std::vector<const rPropTypeInfo*> classTypeList_;
    std::vector<const rPropTypeInfo*> alreadySaved_;
}; //class rOops2Python

} //namespace rOops
