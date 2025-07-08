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

#include <oops/rOops2Python.h>
#include <oops/rPropDescriptor.h>
#include <oops/rOopsConfig.h>
#include <algorithm>

namespace
{

/// Returns a clean version of the type name: all special characters (e.g. ':', '<', '>') is replaced with '_'.
std::string makePythonIdentifier(const std::string& arName)
{
    std::string py_name = arName;
    const char* chars_to_replace = ":.<,> *";
    std::size_t found = py_name.find_first_of(chars_to_replace);
    while (found!=std::string::npos) {
        if ('*' == py_name[found]) {
            py_name[found] = 'P';
        }
        else {
            py_name[found] = '_';
        }
        found = py_name.find_first_of(chars_to_replace, found+1);
    }
    return py_name;
}

std::string makePythonTypeName(const std::string& arName)
{
    // Static map for C++ to Python type name conversion:
    static std::unordered_map<std::string, std::string> cppType2Python = []() {
        std::unordered_map<std::string, std::string> cpp2py;
        cpp2py["double"] = "float";
        cpp2py["std::int8_t"] = "int";
        cpp2py["std::int16_t"] = "int";
        cpp2py["std::int32_t"] = "int";
        cpp2py["std::int64_t"] = "int";
        cpp2py["std::uint8_t"] = "int";
        cpp2py["std::uint16_t"] = "int";
        cpp2py["std::uint32_t"] = "int";
        cpp2py["std::uint64_t"] = "int";
        cpp2py["int8_t"] = "int";
        cpp2py["int16_t"] = "int";
        cpp2py["int32_t"] = "int";
        cpp2py["int64_t"] = "int";
        cpp2py["uint8_t"] = "int";
        cpp2py["uint16_t"] = "int";
        cpp2py["uint32_t"] = "int";
        cpp2py["uint64_t"] = "int";
        cpp2py["std::string"] = "str";
        return cpp2py;
    }();
    auto iter = cppType2Python.find(arName);
    if (iter != cppType2Python.end()) {
        return iter->second;
    }
    return makePythonIdentifier(arName);
}

void printImportStatements(rOops::ostream_type& arStrm)
{
    arStrm << "from dataclasses import dataclass\n";
    arStrm << "from typing import List\n";
    arStrm << "from typing import Dict\n";
    arStrm << "import string\n";
    arStrm << "\n# Class definitions:\n\n";
}

}

namespace rOops
{

rOops2Python::rOops2Python( ostream_type& arStrm)
    : strm_(arStrm)
{
} //Ctor()

rOops2Python::~rOops2Python() = default;

void rOops2Python::saveObj( const rPropConstIterator& arPropIter )
{
    // Probably never happen.
    if (nullptr == arPropIter.objAddr() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object address is NULL!"));
    }
    // Clean items of previous calls.
    classTypeList_.clear();
    alreadySaved_.clear();
    // Create root node. Cannot get the name of the object through the iterator.
    typeInfo_ = arPropIter.itemObjTypeInfo();
    propName_ = makePythonIdentifier(arPropIter.propName());
    writeBlockBegin_(typeInfo_);
    saveObjRecursive_( arPropIter, SaveState_e::eBlock );
    writeBlockEnd_();
    writeValueSeparator_(true);
}

void rOops2Python::saveClassDefinitions(ostream_type& arStrm)
{
    if (classTypeList_.empty()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Call saveObj() for collecting types before calling saveClassDefinitions."));
    }
    printImportStatements(arStrm);
    for (const auto* tip : classTypeList_) {
        saveClassDefinition(arStrm, tip);
    }
}

void rOops2Python::saveAllClassDefinitions(ostream_type& arStrm)
{
    alreadySaved_.clear();
    printImportStatements(arStrm);
    for (const auto& name_tip : *rPropTypeInfo::getNameMap()) {
        if (name_tip.second->isCompound()) {
            saveClassDefinition(arStrm, name_tip.second);
        }
    }
}

void rOops2Python::saveClassDefinition(ostream_type& arStrm, const rPropTypeInfo* apTI)
{
    if (alreadySaved_.end() != std::find(alreadySaved_.begin(), alreadySaved_.end(), apTI)) {
        return; // This type already printed.
    }
    alreadySaved_.push_back(apTI);

    // Print dataclass definition.
    if (0 == apTI->getPropDescSize()) {
        arStrm << "\n@dataclass\nclass " << makePythonIdentifier(apTI->typeName()) << ":\n    pass\n";
        return;
    }

    // Collect base classes and print class definitions before using them.
    std::size_t idx = 0;
    std::vector<const rPropDescriptor*> base_classes;
    while (idx<apTI->getPropDescSize()) {
        const auto* prop_desc = apTI->getPropDesc(idx);
        const auto* tip = prop_desc->propTypeInfo();
        if (tip->isAssociativeContainer()) {
            saveClassDefinition(arStrm, tip->keyTypeInfo());
            saveClassDefinition(arStrm, tip->elementTypeInfo());
        }
        else if (tip->isSTLContainer()) {
            saveClassDefinition(arStrm, tip->elementTypeInfo());
        }
        else if (prop_desc->isAncestor()) {
            base_classes.push_back(prop_desc);
            saveClassDefinition(arStrm, tip);
        }
        else if (tip->isCompound()) {
            saveClassDefinition(arStrm, tip);
        }
        ++idx;
    }

    // Print base classes.
    arStrm << "\n@dataclass\nclass " << makePythonIdentifier(apTI->typeName());
    idx=0;
    if (!base_classes.empty()) {
        arStrm << '(';
        bool print_coma = false;
        for (const auto* base_desc : base_classes) {
            if (!base_desc->isAncestor()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Program error."));
            }
            if (print_coma) {
                arStrm << ", ";
            }
            print_coma = true;
            arStrm << makePythonIdentifier(base_desc->propTypeInfo()->typeName());
        }
        arStrm << ')';
    }
    arStrm << ":\n";

    // Print members.
    idx = 0;
    while (idx<apTI->getPropDescSize()) {
        const auto* prop_desc = apTI->getPropDesc(idx);
        const auto* tip = prop_desc->propTypeInfo();
        if (tip->isAssociativeContainer()) {
            arStrm << "    " << makePythonIdentifier(prop_desc->propName()) << ": Dict[" << makePythonTypeName(tip->keyTypeInfo()->typeName()) << ", "
                                                                                         << makePythonTypeName(tip->elementTypeInfo()->typeName()) << ']';
            arStrm << '\n';
        }
        else if (tip->isSTLContainer()) {
            arStrm << "    " << makePythonIdentifier(prop_desc->propName()) << ": List[" << makePythonTypeName(tip->elementTypeInfo()->typeName()) << ']';
            arStrm << '\n';
        }
        else if (!prop_desc->isAncestor()) {
            arStrm << "    " << makePythonIdentifier(prop_desc->propName()) << ": " << makePythonTypeName(prop_desc->propTypeInfo()->typeName());
            if (prop_desc->hasDefaultValue()) {
                arStrm << " = " << prop_desc->defaultValue();
            }
            arStrm << '\n';
        }
        // No else branch, because nothing to print in case of base class definition.
        ++idx;
    }
    // Print load function.
    arStrm << "    def load(self, yaml_node):\n";
    idx = 0;
    while (idx<apTI->getPropDescSize()) {
        const auto* prop_desc = apTI->getPropDesc(idx);
        if (prop_desc->isAncestor()) {
            arStrm << "        " << makePythonIdentifier(prop_desc->propName()) << ".load(yaml_node[\"" << prop_desc->propName() << "\"])\n";
        }
        else {
            arStrm << "        " << makePythonIdentifier(prop_desc->propName()) << " = yaml_node[\"" << prop_desc->propName() << "\"]\n";
        }
        ++idx;
    }
}

void rOops2Python::saveObjRecursive_(const rPropConstIterator &arPropIter, SaveState_e aSaveState)
{
    // Iterate through all properties of *arPropIter.
    // Open branch.
    rPropConstIterator iP = arPropIter.begin();
    bool data_is_written = false;
    while( !iP.isEnd() ) {
        // Get object address.
        const void* objAddr = iP.objAddr();
        if (nullptr != iP.keyAddr()) {
            saveObjRecursive_(iP.keyIter(), SaveState_e::eKey);
            writeKeyValueSeparator_();
        }
        // Get property name.
        propName_ = makePythonIdentifier(iP.propName());
        //if( iP.isPointer() && (iP.checkFlags(rPropFlag::cSaveAddress) || objIdSet_.end()!=objIdSet_.find(objAddr)) ) {
        if (iP.isPointer()) {
            // Pointer, create reference node.
            typeInfo_ = iP.itemObjTypeInfo();
            writePointer_(objAddr);
            data_is_written = true;
        } //if
        else {
            if(nullptr == objAddr ) {
                if( iP.isPointer() ) {
                    typeInfo_ = iP.itemObjTypeInfo();
                    writePointer_(objAddr);
                    data_is_written = true;
                }
                else {
                    // objAddr is null when std::optional does not contain a value.
                    // Skip this member.
                    data_is_written = false;
                }
            }
            else if( iP.isValue() ) {
                // Simple property or key of a map. Store value.
                typeInfo_ = iP.itemObjTypeInfo();
                if (iP.isRunTime()) {
                    std::string value = iP.itemObjTypeInfo()->value(objAddr);
                    if (value.empty()) {
                        data_is_written = false;
                    }
                    else {
                        writeData_(objAddr);
                        data_is_written = true;
                    }
                }
                else {
                    writeData_(objAddr);
                    data_is_written = true;
                }
            }
            else if (iP.isCompound()) {
                // Compound property (struct or class).
                typeInfo_ = iP.itemObjTypeInfo();
                // Ancestor members are listed as simple members according to the __init__() function of dataclasses in case of inheritance.
                if (!iP.isAncestor()) {
                    writeBlockBegin_(typeInfo_);
                }
                saveObjRecursive_( iP, SaveState_e::eBlock );
                if (!iP.isAncestor()) {
                    writeBlockEnd_();
                }
                data_is_written = true;
            } else if( iP.isSTLContainer() ) {
                // Container property.
                typeInfo_ = iP.itemObjTypeInfo();
                writeListBegin_();
                saveObjRecursive_( iP, SaveState_e::eList );
                writeListEnd_();
                data_is_written = true;
            } else if( iP.isAssociativeContainer() ) {
                // Container property.
                typeInfo_ = iP.itemObjTypeInfo();
                writeMapBegin_();
                saveObjRecursive_( iP, SaveState_e::eMap );
                writeMapEnd_();
                data_is_written = true;
            } else {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Program error"));
            } //if ... else
        }
        ++iP;
        // Write separator according to the current state.
        // It may need to take into account that the item written before the separator
        // is the last element of a container, and skip writing the separator in that case.
        switch (aSaveState) {
            case SaveState_e::eBlock:
                if (data_is_written) {
                    writeValueSeparator_(iP.isEnd());
                }
                break;
            case SaveState_e::eList:
            case SaveState_e::eMap: // there is no writeMapSeparator(aEnd) function.
                writeListSeparator_(iP.isEnd());
                break;
            case SaveState_e::eKey:
                // key separator is already written.
                break;
            default:
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("ERROR: unexpected state!"));
        } //switch
    } //while
} //rOops2Python::saveObjRecursive_()

void rOops2Python::writeData_(const void* apObjAddr)
{
    std::string value = typeInfo_->value(apObjAddr);
    if (!value.empty()) {
        // Write value when it is not empty.
        // If property name is empty, and type name is not required only the value is written!
        writeSizeTypeAndName_();
        // String and character values may need bounding characters.
        char separator1 = typeInfo_->getOpeningCharacter();
        char separator2 = typeInfo_->getClosingCharacter();
        if (separator1) strm_ << separator1;
        strm_ << value;
        if (separator2) strm_ << separator2;
    }
}

void rOops2Python::writePointedData_(const void* apObjAddr)
{
    // Use address as name if name is empty.
    propName_ = '"';
    propName_ += std::to_string(reinterpret_cast<std::uintptr_t>(apObjAddr));
    propName_ += '"';
    writeSizeTypeAndName_();
    char separator1 = typeInfo_->getOpeningCharacter();
    char separator2 = typeInfo_->getClosingCharacter();
    if (separator1) strm_ << separator1;
    strm_ << typeInfo_->value(apObjAddr);
    if (separator2) strm_ << separator2;
}

void rOops2Python::writePointer_(const void* apObjAddr)
{
    writeSizeTypeAndName_();
    strm_ << '"' << std::to_string(reinterpret_cast<std::uintptr_t>(apObjAddr)) << '"';
}

void rOops2Python::clearContext_()
{
    propName_.clear();
}

void rOops2Python::writeSizeTypeAndName_()
{
    // If property name is empty, and type name is not required only the value is written!
    doIndent_();
    if( !propName_.empty() ) {
        strm_ << propName_ << "=";
    }
    clearContext_();
}

void rOops2Python::writeBlockBegin_(const rPropTypeInfo* apTI)
{
    if( !apTI->isCompound() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error: writeBlockBegin_() is called for non compound type!"));
    }
    if (classTypeList_.end() == std::find(classTypeList_.begin(), classTypeList_.end(), apTI)) {
        classTypeList_.push_back(apTI);
    }
    writeSizeTypeAndName_();
    strm_ << makePythonIdentifier(apTI->typeName()) << '(';
    writeEndOfLine_();
    ++inBlock_;
    ++indent_;
}

void rOops2Python::writeBlockEnd_()
{
    if (0 < inBlock_) {
        --inBlock_;
        --indent_;
        doIndent_();
        strm_ << ')';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of block."));
    }
}

void rOops2Python::writeListBegin_()
{
    if( !typeInfo_->isSTLContainer() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error! Type should be an STL container."));
    }
    writeSizeTypeAndName_();
    strm_ << '[';
    ++inCntnr_;
    ++indent_;
    writeEndOfLine_();
}

void rOops2Python::writeListEnd_()
{
    --indent_;
    doIndent_();
    if (0 < inCntnr_) {
        --inCntnr_;
        strm_ << ']';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of list."));
    }
}

void
rOops2Python::writeMapBegin_()
{
    if( !typeInfo_->isAssociativeContainer() ) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type info error! Type should be an associative container."));
    }
    writeSizeTypeAndName_();
    strm_ << '{';
    ++inCntnr_;
    ++indent_;
    writeEndOfLine_();
}

void rOops2Python::writeMapEnd_()
{
    --indent_;
    doIndent_();
    if (0 < inCntnr_) {
        --inCntnr_;
        strm_ << '}';
    }
    else {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of map."));
    }
}

void rOops2Python::writeValueSeparator_(bool aEnd)
{
    if (!aEnd) {
        strm_ << ',';
    }
    if (!endOfLineAlreadyWritten_) {
        writeEndOfLine_();
    }
}

void rOops2Python::writeListSeparator_( bool aEndOf )
{
    // Coma separated list of values.
    if( !aEndOf ) {
        doIndent_();
        strm_ << ',';
    }
    writeEndOfLine_();
}

void rOops2Python::writeKeyValueSeparator_()
{
    // Separator between the key and value.
    strm_ << ": ";
    doNotIdentNow_ = true;
}

void rOops2Python::doIndent_()
{
    endOfLineAlreadyWritten_ = false;
    if (!doNotIdentNow_) {
        for (int ii = 0; ii < indent_; ++ii) strm_ << "  ";
    }
    doNotIdentNow_ = true;
}

void rOops2Python::writeEndOfLine_()
{
    strm_ << "\n";
    doNotIdentNow_ = false;
    endOfLineAlreadyWritten_ = true;
}

} //namespace rOops

