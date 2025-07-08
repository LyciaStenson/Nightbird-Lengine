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
Class and structures Type Info Classes.
*/


#include <oops/rPropTypeInfoBase.h>
#include <functional>


namespace rOops
{

    using rPropDescContainer_t = std::deque<rPropDescriptor>;

    class rPropTypeInfoCompoundTypeBase : public rPropTypeInfo
    {
    public:
        rPropTypeInfoCompoundTypeBase() noexcept;
        rPropTypeInfoCompoundTypeBase(const std::string& arTypeName, const std::string& arTypeName2) noexcept;
        ~rPropTypeInfoCompoundTypeBase() noexcept override;
        /// Creating property descriptors.
        void addBaseProperties( const rPropTypeInfo* apBaseTypeInfo, offset_type aBase, flags_type aFlags );
        rPropDescriptor& inheritBaseClass(const rPropTypeInfo* apType, offset_type aBase, flags_type aFlags);
        rPropDescriptor& addPropDesc(const rPropTypeInfo* apType, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, offset_type aOffset, flags_type aFlags, rPropDefault aDefaultValue=rPropDefault());
        rPropDescriptor& addPropDescRunTime(const std::string& arType, const std::string& arName, offset_type aOffs, flags_type aFlags) const;
        rPropDescriptor& copyPropDesc(const rPropDescriptor& arPropDesc, offset_type aBase);

        std::string value( const void* /*apVal*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented.")); }
        void value( const void* /*apVal*/, std::string& /*arValue*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented.")); }
        void setValue( void* /*apVal*/, const std::string& /*arValue*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented.")); }

        void save(rOopsBinaryFormat& arFormat, const void *apVal) const override;
        void save(rOopsTextFormat& arFormat, const void *apVal) const override;
        void save(rOopsYamlFormat& arFormat, const void *apVal) const override;

        void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;
        void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const override;

        rPropIterator begin( void* apVal, const std::string& arName) const override;
        rPropConstIterator begin( const void* apVal, const std::string& arName) const override;
        bool isValue() const override
            { return false; }
        /// Returns true, if this type is class or struct.
        bool isCompound() const override
            { return true; }
        std::size_t getPropDescSize() const override
            { return propDescTable_.size(); }
        const rPropDescriptor* getPropDesc( rOops::size_type aIdx ) const override
            { return (aIdx<propDescTable_.size()) ? &propDescTable_[aIdx] : nullptr; }
        rPropDescContainer_t::const_iterator propDescTableBegin() const
            { return propDescTable_.begin(); }
        rPropDescContainer_t::const_iterator propDescTableEnd() const
            { return propDescTable_.end(); }
        bool findProperty(std::size_t& arIdx, const std::string& arPropName) const;
    protected:
        bool hasRequiredProperties() const;
    protected:
        mutable rPropDescContainer_t propDescTable_; ///< container of property descriptors for members and ancestors.
    }; //class rPropTypeInfoCompoundTypeBase

    template< class CompoundT >
    class rPropTypeInfoCompoundAbstract : public rPropTypeInfoCompoundTypeBase
    {
    public:
        explicit rPropTypeInfoCompoundAbstract( const std::string& arTypeName ) noexcept
            : rPropTypeInfoCompoundTypeBase(arTypeName, rOops::getTypeName<CompoundT>())
            {}
        /// Create single object with 'new'.
        void *createObj() const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Creating abstract object.")); }
        /// Create an object with placement new.
        void *createObj( void* /*apData*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Creating abstract object.")); }
        /// Create array of objects with 'new[aN]'.
        void *createObj( size_type /*aN*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Creating abstract object.")); }
        /// Destroy object allocated with new.
        void destroyObj( void* /*apObj*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Destroying abstract object.")); }
        /// Destroy object allocated with placement new.
        void destroyObj( void* /*apObj*/, void* /*apData*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Destroying abstract object.")); }
        /// Destroy array of objects with 'delete[]'.
        void destroyObj( void* /*apObjArr*/, size_type /*aN*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Destroying abstract object.")); }
    }; //class rPropTypeInfoCompoundAbstract

    template< class CompoundT >
    class rPropTypeInfoCompoundCreateDestroy : public rPropTypeInfoCompoundTypeBase
    {
        static_assert(!std::is_abstract<CompoundT>::value,
                      "CompoundT is abstract, cannot be created. Use rOOPS_ADD_PROPERTY_INTERFACE_ABSTRACT() for adding property interface to this class!");
        static_assert(std::is_default_constructible<CompoundT>::value,
                      "CompoundT has no public default constructor. Use rOOPS_ADD_PROPERTY_INTERFACE_ABSTRACT() for adding property interface to this class!");
    public:
        explicit rPropTypeInfoCompoundCreateDestroy( const std::string& arTypeName ) noexcept
            : rPropTypeInfoCompoundTypeBase(arTypeName, rOops::getTypeName<CompoundT>())
            {}
        /// Create single object with 'new'.
        void *createObj() const override
            { return new CompoundT(); }
        /// Create an object with placement new.
        void *createObj( void *apData ) const override
            { return new(apData) CompoundT(); }
        /// Create array of objects with 'new[aN]'.
        void *createObj( size_type aN ) const override
            { return new CompoundT[aN]; }
        /// Destroy object allocated with new.
        void destroyObj( void* apObj ) const override
            { delete static_cast<CompoundT*>(apObj); }
        /// Destroy object allocated with placement new.
        void destroyObj( void* apObj, void* /*apData*/ ) const override
            { static_cast<CompoundT*>(apObj)->~CompoundT(); }
        /// Destroy array of objects with 'delete[]'.
        void destroyObj( void* apObjArr, size_type /*aN*/ ) const override
            { delete[] static_cast<CompoundT*>(apObjArr); }
    }; //class rPropTypeInfoCompoundCreateDestroy


    // Non-invasive type descriptor.
    // createPropDescTableStaticImplementation() is a global function.
    template< class CompoundT, class PropTypeInfoBaseT >
    class rPropTypeInfoCompoundType : public PropTypeInfoBaseT
    {
    public:
        explicit rPropTypeInfoCompoundType( const std::string& arTypeName ) noexcept
            : PropTypeInfoBaseT(arTypeName)
            {}
        size_type Size() const override
            { return sizeof(CompoundT); }
        const rPropTypeInfo* objTypeInfo( const void* /*apObjAddress*/ ) const override
            { return this; } // non-polymorphic type, cannot do anything better.
    }; //rPropTypeInfoCompoundType


    template< class CompoundT, class PropTypeInfoBaseT >
    class rPropTypeInfoPolymorphicType : public PropTypeInfoBaseT
    {
        static_assert(std::is_polymorphic<CompoundT>::value, "CompoundT must be polymorphic type.");
    public:
        explicit rPropTypeInfoPolymorphicType( const std::string& arTypeName ) noexcept
            : PropTypeInfoBaseT(arTypeName)
            {
                CompoundT::createPropDescTableStaticImplementation(this);
            }
        size_type Size() const override
            { return sizeof(CompoundT); }
        const rPropTypeInfo* objTypeInfo( const void* /*apObjAddress*/ ) const override
            { throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Not implemented!!!")); } // TODO
    }; //rPropTypeInfoPolymorphicType


    template< class CompoundT, class PropTypeInfoBaseT >
    class rPropTypeInfoPropInterfaceType : public PropTypeInfoBaseT
    {
    public:
        explicit rPropTypeInfoPropInterfaceType( const std::string& arTypeName )
            : PropTypeInfoBaseT(arTypeName)
            {
                CompoundT::createPropDescTableStaticImplementation(this);
            }
        size_type Size() const override
            { return sizeof(CompoundT); }
        const rPropTypeInfo* objTypeInfo( const void* apObjAddress ) const override
            { return static_cast<const CompoundT*>(apObjAddress)->OOPSGetTypeInfo(); }
        void addUnknownProperty(void *apObjAddress, const std::string& arType, const std::string& arName, const std::string& arValue) const override
            {
                rDbgTrace("addUnknownProperty(): " << arName << " to " << objTypeInfo(apObjAddress)->typeName())
                auto& desc = this->addPropDescRunTime(arType, arName, static_cast<CompoundT*>(apObjAddress)->getUnknownPropTableOffset(), rPropFlag::cDefault);
                static_cast<CompoundT*>(apObjAddress)->addToUnknownPropTbl(desc.descTableIndex(), arValue);
            }
        void setUnknownProperty(void *apObjAddress, size_t aIndex, const std::string& arValue) const override
            {
                static_cast<CompoundT*>(apObjAddress)->addToUnknownPropTbl(aIndex, arValue);
            }
        const std::string& getUnknownProperty(const void *apObjAddress, size_t aIndex) const override
            {
                return static_cast<const CompoundT*>(apObjAddress)->getUnknownProperty(aIndex);
            }
        void validate(void* apObjAddress, std::uint32_t aVersion) const override
            {
                if (_validatorFunction) {
                    _validatorFunction(*static_cast<CompoundT*>(apObjAddress), aVersion);
                }
                else {
                    // Call type descriptor base for checking version numbers.
                    PropTypeInfoBaseT::validate(apObjAddress, aVersion);
                }
            }
        void setValidator( std::function<void(CompoundT&, std::uint32_t)> aValidator)
            {
                _validatorFunction = aValidator;
            }
    private:
        std::function<void(CompoundT&, std::uint32_t)> _validatorFunction;
    }; //rPropTypeInfoPropInterfaceType


    template< class CompoundT, bool AbstractT, bool PolymorphicT, bool PropInterfaceT >
    struct CompoundTypeInfoSelector
    {
        typedef void TypeInfo_type;
    };


    // Simple classes without virtual functions: rPropTypeInfoCompoundType<>
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,false,false,false>
        { typedef rPropTypeInfoCompoundType< CompoundT, rPropTypeInfoCompoundCreateDestroy<CompoundT> > TypeInfo_type; };
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,true,false,false>
        { typedef rPropTypeInfoCompoundType< CompoundT, rPropTypeInfoCompoundAbstract<CompoundT> > TypeInfo_type; };
    // Polymorphic classes: rPropTypeInfoPolymorphicType<>
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,false,true,false>
        { typedef rPropTypeInfoPolymorphicType< CompoundT, rPropTypeInfoCompoundCreateDestroy<CompoundT> > TypeInfo_type; };
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,true,true,false>
        { typedef rPropTypeInfoPolymorphicType< CompoundT, rPropTypeInfoCompoundAbstract<CompoundT> > TypeInfo_type; };
    // The class has property interface.
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,false,false,true>
        { typedef rPropTypeInfoPropInterfaceType< CompoundT, rPropTypeInfoCompoundCreateDestroy<CompoundT> > TypeInfo_type; };
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,true,false,true>
        { typedef rPropTypeInfoPropInterfaceType< CompoundT, rPropTypeInfoCompoundAbstract<CompoundT> > TypeInfo_type; };
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,false,true,true>
        { typedef rPropTypeInfoPropInterfaceType< CompoundT, rPropTypeInfoCompoundCreateDestroy<CompoundT> > TypeInfo_type; };
    template< class CompoundT > struct CompoundTypeInfoSelector<CompoundT,true,true,true>
        { typedef rPropTypeInfoPropInterfaceType< CompoundT, rPropTypeInfoCompoundAbstract<CompoundT> > TypeInfo_type; };


} //namespace rOops
