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

#include <oops/rPropLog.h>
#include <oops/rPropTypeInfoCompound.h>
#include <set>

namespace rOops
{


rPropTypeInfoCompoundTypeBase::rPropTypeInfoCompoundTypeBase() noexcept
    : rPropTypeInfo()
{
}

rPropTypeInfoCompoundTypeBase::rPropTypeInfoCompoundTypeBase(const std::string& arTypeName, const std::string& arTypeName2 ) noexcept
    : rPropTypeInfo()
{
    // parse arTypeName if it is template type.
    // e.g.: Point<ValueT> --> Point, ValueT
    auto parseTypeName = [](const std::string& arStr) -> std::vector<std::string>
    {
        std::vector<std::string> v;
        std::size_t p0 = arStr.find('<');
        v.push_back(arStr.substr(0, p0));
        if (std::string::npos == p0) {
            return v;
        }
        ++p0;
        unsigned stack = 0u; // counting enclosed <>.
        std::size_t p1 = p0;
        std::size_t p2 = p1;
        bool closing_brace_found = false;
        while (!closing_brace_found && std::string::npos != p0) {
            if (0u == stack) {
                p1 = p0;
                p2 = arStr.find_first_of(",<>", p1);
                switch (arStr[p2]) {
                case ',':
                    v.push_back(arStr.substr(p0, p2-p0));
                    p0 = p1 = p2 + 1;
                    break;
                case '<':
                    ++stack;
                    p1 = p2 + 1;
                    break;
                case '>':
                    v.push_back(arStr.substr(p0, p2-p0));
                    return v;
                }
            }
            else {
                p2 = arStr.find_first_of(",<>", p1);
                switch (arStr[p2]) {
                case ',':
                    break;
                case '<':
                    ++stack;
                    break;
                case '>':
                    --stack;
                    if (0 == stack) {
                        v.push_back(arStr.substr(p0, p2-p0+1));
                        p0 = p2 + 1;
                        closing_brace_found = true;
                    }
                    break;
                }
                p1 = p2 + 1;
            }
        }
        if (!closing_brace_found) {
            // The closing '>' character is missing.
            // Add the last part.
            v.push_back(arStr.substr(p0));
        }
        return v;
    };
    // Parse the given type name.
    std::vector<std::string> type_name_1 = parseTypeName(arTypeName);
    if (1 == type_name_1.size()) {
        // No '<' in typename, not a template type name.
        typeId_ = generateTypeId(arTypeName);
        typeName_ = arTypeName;
    }
    else {
        // Parse the generated type name.
        std::vector<std::string> type_name_2 = parseTypeName(arTypeName2);
        // Check the result. In case of any error use 'arTypeName2'.
        if (!type_name_1.empty() && type_name_1.size() <= type_name_2.size()) {
            // Re-build the type name by replacing the template parameters with the name of the instantiation type name.
            // Replace the template parameters in arTypeName with the actual types parsed from the output of getTypeName().
            std::string type_name = type_name_1[0];
            if (1 < type_name_1.size()) {
                char separator = '<';
                for (std::size_t idx = 1; idx < type_name_1.size(); ++idx) {
                    type_name += separator;
                    const auto* pTI2 = rPropGetTypeInfoByName(type_name_2[idx]);
                    if (nullptr == pTI2) {
                        type_name += type_name_2[idx];
                    }
                    else {
                        type_name += pTI2->typeName();
                    }
                    separator = ',';
                }
                type_name += '>';
            }
            typeId_ = generateTypeId(type_name);
            typeName_ = type_name;
        }
        else {
            typeName_ = arTypeName2;
        }
    }
    replaceSpaces(typeName_);
    rDbgTrace( "Type info of " << typeId_ << ":" << typeName_ << " created.")
    if(typeName_ != "RunTimeType") {
        _registerType();
    }
}

rPropTypeInfoCompoundTypeBase::~rPropTypeInfoCompoundTypeBase() noexcept
{
    // Delete all elements of propDescTable_.
    auto ii = propDescTable_.begin();
    while( ii != propDescTable_.end() ) {
        ii = propDescTable_.erase( ii );
    }
}

bool rPropTypeInfoCompoundTypeBase::hasRequiredProperties() const
{
    for (const auto& item : propDescTable_) {
        if (item.checkFlags(rPropFlag::cRequired)) {
            return true;
        }
    }
    return false;
}
void rPropTypeInfoCompoundTypeBase::addBaseProperties(const rPropTypeInfo* apBaseTypeInfo, offset_type aBase, flags_type)
{
    if (!apBaseTypeInfo->isCompound()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(apBaseTypeInfo->typeName() << " is not compound type."));
    }
    for (std::size_t idx = 0; idx < apBaseTypeInfo->getPropDescSize(); ++idx) {
        copyPropDesc(*apBaseTypeInfo->getPropDesc(idx), aBase);
    }
}

rPropDescriptor& rPropTypeInfoCompoundTypeBase::inheritBaseClass(const rPropTypeInfo* apType, offset_type aBase, flags_type aFlags)
{
    propDescTable_.emplace_front(apType, aBase, aFlags);
    auto& prop_desc = propDescTable_.front();
    // Check if the base class has any required properties.
    const auto* pPropTICompound = dynamic_cast<const rPropTypeInfoCompoundTypeBase*>(propDescTable_.front().propTypeInfo());
    if (pPropTICompound->hasRequiredProperties()) {
        // The base class shall be required here, if it has at least one required property.
        prop_desc.required();
    }
    // Update the index stored in the table, because push_front() invalidate them.
    // The inheritBaseClass() shall be called first, so this is generally do nothing, but there is no guarantee for that.
    for (size_t idx=0; idx<propDescTable_.size(); ++idx) {
        propDescTable_[idx].setDescTableIndex(idx);
    }
    return prop_desc;
}

rPropDescriptor& rPropTypeInfoCompoundTypeBase::addPropDesc(const rPropTypeInfo* apType, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, offset_type aOffset, flags_type aFlags, const rPropDefault aDefaultValue)
{
    auto idx = propDescTable_.size();
    propDescTable_.emplace_back(apType, std::move(apApplyPtr), arPropName, aOffset, aFlags, aDefaultValue);
    auto& prop_desc = propDescTable_.back();
    prop_desc.setDescTableIndex(idx);
    return prop_desc;
}

rPropDescriptor& rPropTypeInfoCompoundTypeBase::addPropDescRunTime(const std::string& arType, const std::string& arName, offset_type aOffs, flags_type aFlags) const
{
    auto idx = propDescTable_.size();
    propDescTable_.emplace_back(arType, idx, arName, aOffs, aFlags);
    return propDescTable_[idx];
}

rPropDescriptor& rPropTypeInfoCompoundTypeBase::copyPropDesc(const rPropDescriptor& arPropDesc, offset_type aBase)
{
    auto idx = propDescTable_.size();
    propDescTable_.emplace_back(arPropDesc, aBase);
    auto& prop_desc = propDescTable_.back();
    prop_desc.setDescTableIndex(idx);
    return prop_desc;
}

template <typename FormatT>
void saveTemplate(const rPropTypeInfoCompoundTypeBase* apThis, FormatT& arFormat, const void *apVal)
{
    auto iDesc = apThis->propDescTableBegin();
    auto iEnd = apThis->propDescTableEnd();
    arFormat.writeBlockBegin();
    while( iDesc != iEnd ) {
        if (iDesc->isRunTime()) {
            // Do not save runtime property, if it is not added to this object.
            const std::string& val = apThis->getUnknownProperty(apVal, iDesc->descTableIndex());
            if (!val.empty()) {
                iDesc->save(arFormat, apVal, true);
                arFormat.writeValueSeparator(false);
            }
        }
        else if (!iDesc->checkFlags(rPropFlag::cReadOnly)) {
            if (iDesc->save(arFormat, apVal, true)) {
                // Do not write separator, if nothing was saved: e.g. empty optional.
                arFormat.writeValueSeparator(false);
            }
        }
        ++iDesc;
    } //while
    arFormat.writeBlockEnd();
}

void rPropTypeInfoCompoundTypeBase::save(rOopsBinaryFormat& arFormat, const void *apVal) const
{
    saveTemplate(this, arFormat, apVal);
}

void rPropTypeInfoCompoundTypeBase::save(rOopsTextFormat& arFormat, const void *apVal) const
{
    saveTemplate(this, arFormat, apVal);
}

void rPropTypeInfoCompoundTypeBase::save(rOopsYamlFormat& arFormat, const void *apVal) const
{
    saveTemplate(this, arFormat, apVal);
}

bool rPropTypeInfoCompoundTypeBase::findProperty(std::size_t& arIdx, const std::string& arPropName) const
{
    // Nothing to do, if property descriptor table is empty.
    if (propDescTable_.empty()) {
        return false;
    }
    // Check the idx, and set it to 0 if it is over the end.
    if (propDescTable_.size() == arIdx) {
        arIdx = 0;
    }
    auto idxEnd = arIdx;
    if (!arPropName.empty()) {
        while (arPropName != propDescTable_[arIdx].propName()) {
            ++arIdx;
            // arIter may point to the end because the previous if was false, or because it became end after incrementing.
            if (propDescTable_.size() <= arIdx) {
                arIdx = 0;
            }
            // Avoid infinite loop, if name is not found.
            if (idxEnd == arIdx) {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}

template <typename ParserT>
void loadTemplate(const rPropTypeInfoCompoundTypeBase* apThis, ParserT& arParser, void* apVal, rOopsParserContext& arCtx)
{
    if (!arCtx.isBlock()) {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropTypeInfoCompoundTypeBase::load() is called when parser context is not block."));
    }
    // Container for storing the descriptor table index of the properties already set.
    std::set<rPropDescContainer_t::size_type> property_set;
    std::uint32_t context_version = arCtx.version_;
    std::size_t idx = 0;
    while (true) {
        auto parse_result = arParser.parse(arCtx);
        if (ParserReturnCode::eBlockEnd == parse_result) {
            break;
        }
        if (ParserReturnCode::eEndOfStream == parse_result) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unexpected end of file."));
        }
        if (apThis->findProperty(idx, arCtx.propName_)) {
            apThis->getPropDesc(idx)->load(arParser, apVal, arCtx);
            property_set.insert(idx);
        }
        else {
            if (rOops::globalConfig().errorIfPropNameNotFound) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property name not found: " << arCtx.propName_));
            }
            else {
                if (!arCtx.propName_.empty()) {
                    rDbgWarning("Property name not found: " << Type << " " << arCtx.propName_);
                    // Add to unknown property.
                    apThis->addUnknownProperty(apVal, arCtx.typeName_, arCtx.propName_, arCtx.value_);
                }
            }
        }
    }
    // Check required properties and set default values.
    for (idx = 0; idx < apThis->getPropDescSize(); ++idx) {
        auto& desc = *apThis->getPropDesc(idx);
        if (desc.hasDefaultValue()) {
            // If this property has default value, and has not read from the stream,
            // set it to the default value stored in the Property Descriptor Table.
            if (property_set.end()==property_set.find(idx)) {
                if (desc.isPointer()) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Pointer member cannot have default value: " << arCtx.propName_));
                }
                desc.propTypeInfo()->setValue(desc.propAddr(apVal), desc.defaultValue());
            }
        }
        else if (desc.checkFlags(rPropFlag::cRequired)) {
            // If this property is required, and it is not in the set of properties just read from the stream.
            // This is an error, throw the appropriate exception.
            if (property_set.end()==property_set.find(idx)) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Required property (" << desc.propName() << ") is missing."));
            }
        }
    }
    apThis->validate(apVal, context_version);
}

void rPropTypeInfoCompoundTypeBase::load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const
{
    loadTemplate(this, arParser, apVal, arCtx);
}

void rPropTypeInfoCompoundTypeBase::load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const
{
    loadTemplate(this, arParser, apVal, arCtx);
}

void rPropTypeInfoCompoundTypeBase::load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const
{
    loadTemplate(this, arParser, apVal, arCtx);
}


rPropIterator rPropTypeInfoCompoundTypeBase::begin( void* apVal, const std::string& ) const
{
    rPropIterator iter( std::make_unique<rPropIteratorCompound>(this,apVal) );
    return iter;
}

rPropConstIterator rPropTypeInfoCompoundTypeBase::begin( const void* apVal, const std::string& ) const
{
    rPropConstIterator iter( std::make_unique<rPropConstIterCompound>(this,apVal) );
    return iter;
}

} //namespace rOops
