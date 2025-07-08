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
Property Descriptor interface.
*/

#include <oops/rPropTypeDef.h>
#include <oops/rOopsParserContext.h>
#include <oops/rPropTypeTraits.h>
#include <oops/rPropLog.h>
#include <exception>
#include <deque>
//#include <memory>


namespace rOops
{

class rPropTypeInfo;

namespace rPropFlag
{
    /// Mark read-only property. It will be only loaded from the stream, but it is never saved.
    /// Useful for redundant properties and for backward compatibility when a property is renamed.
    constexpr flags_type cReadOnly  = 0x0100;
    constexpr flags_type cRequired = 0x2000; ///< Required properties must be in the input stream. The loader checks if it was loaded and throws an exception in any of the required properties are missing.
    constexpr flags_type cDefault   = 0;
}


/// Placeholder for having different overload for container flags and element flags.
struct ElementFlags_s
{
    flags_type flags;
    explicit ElementFlags_s( flags_type aFlags ) : flags(aFlags) {}
};

inline const rPropTypeInfo* rPropGetTypeInfoFromContext(const rOopsParserContext& arCtx)
{
    if (!arCtx.typeName_.empty()) return rPropGetTypeInfoByName(arCtx.typeName_);
    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("rPropGetTypeInfoFromContext(): both type name is empty."));
}

struct rPropDefault
{
    rPropDefault() = default;
    explicit rPropDefault(std::string aStr) : str(std::move(aStr)) {}
    std::string str;
    bool hasDefault = false;
};


///  Store information about a pointer member.
struct PointerInfo
{
    const rPropDescriptor* propDesc_ = nullptr;
    void* pointer_ = nullptr; ///< Address of the pointer: *pointer_ = createObject();
    PointerInfo() = default;
    PointerInfo(const rPropDescriptor* apPropDesc, void* apPointer)
        : propDesc_(apPropDesc)
        , pointer_(apPointer)
    {}
};

struct ConstPointerInfo
{
    const rPropDescriptor* propDesc_ = nullptr;
    const void* pointer_ = nullptr; ///< Address of the pointer: *pointer_ = createObject();
    ConstPointerInfo() = default;
    ConstPointerInfo(const rPropDescriptor* apPropDesc, const void* apPointer)
        : propDesc_(apPropDesc)
        , pointer_(apPointer)
    {}
    /// Type conversion from PointerInfo.
    /// It is intentionally not explicit!
    ConstPointerInfo(const PointerInfo& arPtrInfo)
        : propDesc_(arPtrInfo.propDesc_)
        , pointer_(arPtrInfo.pointer_)
    {}
};


/// <summary>
/// ApplyPointer base class.
/// It implements apply pointer functions for non pointer member.
/// </summary>
class rApplyPointer
{
public:
    virtual ~rApplyPointer() = default;
    virtual bool isOptional() const
    {
        return false;
    }
    virtual bool isPointer() const
    {
        return false;
    }
    virtual void* apply( void* apAddr ) const
    {
        return apAddr;
    }
    virtual const void* apply( const void* apAddr ) const
    {
        return apAddr;
    }
    virtual void set(void* /*apObjAddr*/, void* /*apAddr*/) const
    {
        throw std::runtime_error("This function should never be called.");
    }
    virtual void assign(void*, const ConstPointerInfo&) const
    {
        throw std::runtime_error("This function should never be called.");
    }
};


class rPropDescriptor final
{
public:
    /// Destructor.
    ~rPropDescriptor();
    /// Constructors.
    // Ancestor
    rPropDescriptor(const rPropTypeInfo* apTypeInfo, offset_type aBase, flags_type aFlags);
    // Copy for flat.
    rPropDescriptor(const rPropDescriptor& arPropDesc, offset_type aBase);
    // Member
    rPropDescriptor(const rPropTypeInfo* apTypeInfo, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, offset_type aOffset, flags_type aFlags, rPropDefault aDefaultValue = rPropDefault());
    // Runtime
    rPropDescriptor(const std::string& arTypeName, size_t aIdx, const std::string& arPropName, offset_type aOffset, flags_type aFlags);
    // Set-get
    // TODO: rPropDescriptor(const rPropTypeInfo* apTypeInfo, std::shared_ptr<rApplyPointer> apApplyPtr, const std::string& arPropName, SetMemberFuncPtr_t apSet, GetMemberFuncPtr_t apGet, flags_type aFlags);
    rPropDescriptor() = delete;
    rPropDescriptor(           const rPropDescriptor&) = default;
    rPropDescriptor& operator=(const rPropDescriptor&) = default;
    rPropDescriptor(           rPropDescriptor&&) = default;
    rPropDescriptor& operator=(rPropDescriptor&&) = default;

    /// Comma operators.
    rPropDescriptor& operator,( const std::string& arPropName ) { propName_ = arPropName; return *this; }
    rPropDescriptor& operator,( flags_type arFlags ) { flags_ |= arFlags; return *this; }
    rPropDescriptor& operator,( const ElementFlags_s& arFlags );
    rPropDescriptor& operator,(const rPropDefault& aDefaultValue) { defaultValue_ = aDefaultValue; defaultValue_.hasDefault = true; return *this; }

    /// Setter functions:
    rPropDescriptor& required() { flags_ |= rPropFlag::cRequired; return *this; }
    rPropDescriptor& name( const char* apPropName ) { propName_ = apPropName; return *this; }
    rPropDescriptor& defaultValue( const rPropDefault& aDefaultValue ) { defaultValue_ = aDefaultValue; defaultValue_.hasDefault = true; return *this; }

    /// Returns the type information record which belongs to this property. It does not count on polymorphism.
    const rPropTypeInfo* propTypeInfo() const;
    //void setTypeInfo(const rPropTypeInfo* apTypeInfo);
    //bool isInitialized() const;
    /// Returns the type information record which belongs to this object. It applies polymorphism, and returns the type info structure of the real object.
    const rPropTypeInfo* objTypeInfo( const void* apObjAddr ) const;
    /// Returns 'true' if the described property is container.
    /** Helper function. It returns true if the property is compound (struct or class) or container (e.g. std::list<>). */
    bool isContainer() const
        { return isCompound() || isSTLContainer(); }
    /// Returns 'true' if the described property is a class type object.
    bool isCompound() const
        { return propTypeInfo()->isCompound(); }
    /// Returns 'true' if the property is the base class.
    bool isAncestor() const
        { return Type::eAncestor==type_; }
    bool isSTLContainer() const
    {
        return propTypeInfo()->isSTLContainer();
    }
    bool isRunTime() const
    {
        return (Type::eRunTime==type_);
    }
    std::string propName() const
        { return propName_; }
    NameIdHashType propHash() const
        { return propNameId_; }
    const std::string& typeName() const
        { return propTypeInfo()->typeName(); }
    OopsConfig::WriteTypeName writeTypeNameAbove() const
        { return isPointer() ? OopsConfig::WriteTypeName::eNever : OopsConfig::WriteTypeName::eWhenNeeded; }

    /**
     * @brief: Get the size of the object.
     * This is the size, what the object reserve in the memory.
     * Same as the value of the size_of() operator.
     * For pointers, return the size of the pointer instead of the object itself.
     * In that case, for the size of the object,
     * call the \c Size() member function of the type info class.
     */
    size_type Size() const;
    
    /// Returns the flags of the property. These flags are assigned to the property when the Property Descriptor is created.
    flags_type Flags() const { return flags_; }
    /// Returns 'true', if the property is pointer. This function simple check the appropriate flag.
    bool isPointer() const
    {
        if (nullptr == pApplyPtr_) return false;
        return pApplyPtr_->isPointer();
    }
    /// Checks the given flag, and returns true if it is set.
    bool checkFlags( flags_type aFilter ) const { return 0 != (flags_ & aFilter); }
    /// Check if the property has default value or not.
    bool hasDefaultValue() const { return defaultValue_.hasDefault;  }
    const std::string& defaultValue() const { return defaultValue_.str;  }

    offset_type offset() const
        { return offset_; }

    size_t descTableIndex() const
        { return _descTableIndex; }
    void setDescTableIndex( size_t aIdx )
        {
            if (aIdx < _descTableIndex) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("It is unlikely to set smaller descriptor table index."));
            }
            _descTableIndex = aIdx;
        }

    void* propAddr( void* apBaseAddr ) const;
    const void* propAddr( const void* apBaseAddr ) const;
    void* objAddr( void* apBaseAddr ) const;
    const void* objAddr( const void* apBaseAddr ) const;
    void setObjAddr(void* apBaseAddr, void* apAddr) const;

    template <typename FormatT>
    bool save(FormatT& arFormat, const void *apCompoundAddr, bool aCanSaveAddress) const
    {
        const void* pVal = objAddr(apCompoundAddr);
        arFormat.setName(propName_);
        const rPropTypeInfo* pTI = pTypeInfo_;
        if (isPointer()) {
            if (nullptr == pVal) {
                arFormat.setType(pTI, OopsConfig::WriteTypeName::eAlways, 1);
                arFormat.writePointer(pVal);
            }
            else {
                pTI = pTypeInfo_->objTypeInfo(pVal);
                if (aCanSaveAddress && arFormat.config().saveAddress) {
                    // Save the pointer and add it to the list of unsaved objects.
                    arFormat.setType(pTI, OopsConfig::WriteTypeName::eAlways, 1);
                    arFormat.writePointer(pVal);
                    arFormat.addUnsavedObject(pTI, pVal);
                }
                else if (arFormat.isAlreadySaved(pVal)) {
                    // Save the pointer only.
                    arFormat.setType(pTI, OopsConfig::WriteTypeName::eAlways, 1);
                    arFormat.writePointer(pVal);
                }
                else {
                    arFormat.setAddress(pVal);
                    arFormat.setType(pTI, OopsConfig::WriteTypeName::eNever, 1);
                    pTI->save(arFormat, pVal);
                    arFormat.addAlreadySavedObject(pVal);
                }
            }
        }
        else {
            // objAddr() already called pApplyPtr_->apply().
            // No additional branch for optional is needed.
            // objAddr is null when std::optional does not contain a value.
            if (nullptr != pVal) {
                arFormat.setType(pTI, OopsConfig::WriteTypeName::eAlways, pTI->nrOfElements(pVal));
                pTI->save(arFormat, pVal);
            }
            else {
                // empty optional, nothing saved.
                return false;
            }
        }
        return true;
    }

    template <typename ParserT>
    void load(ParserT& arParser, void* apCompoundAddr, rOopsParserContext& arCtx) const
    {
        void* pMember = propAddr(apCompoundAddr);
        if (nullptr == pMember) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object address is null."));
        if (isPointer()) {
            if (!arCtx.pointer_.empty()) {
                // Object is saved somewhere else, arCtx.pointer_ contains the old address, which is an id for finding the new object.
                auto ctx_ptr = arCtx.pointer_; // save it, because calling the parser will change the context.
                if (handlePointer(arParser, pMember, arCtx)) {
                    // Object is not loaded yet, store this pointer for being updated later.
                    arParser.addToUnloadedObjects(ctx_ptr, this, pMember);
                }
            }
            else {
                if (ParserReturnCode::ePointer == arCtx.returnCode_) {
                    // Pointer was parsed, pointer_ is NULL, set pMember to nullptr.
                    pApplyPtr_->set(pMember, nullptr);
                }
                else {
                    // The object is saved-in-place, its address may be saved with the member name (e.g. MemberName&1234 = !MyObject {...}; ).
                    // Create the member and assign it to the pointer.
                    auto ctx_addr = arCtx.address_; // store it, because calling the parser will change the context.
                    loadSavedInPlacePointer(arParser, pMember, arCtx);
                    if (!ctx_addr.empty()) {
                        // 'ctx_addr' is NULL if address of the object is not stored in the input stream.
                        // It is not needed, if there is no other reference to the object (e.g. unique_ptr).
                        arParser.addToAlreadyLoadedObjects(ctx_addr, this, pMember);
                    }
                }
            }
        }
        else if (pApplyPtr_->isOptional()) {
            void* opt_data = pApplyPtr_->apply(pMember);
            pTypeInfo_->load(arParser, opt_data, arCtx);
        }
        else {
            pTypeInfo_->load(arParser, pMember, arCtx);
        }
    }

    /// addToUnloadedObjects() shall be called if returns true.
    template <typename ParserT>
    bool handlePointer(ParserT& arParser, void* apPointerAddr, rOopsParserContext& arCtx) const
    {
        if (nullptr == apPointerAddr) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object address is null."));
        if (arCtx.pointer_.empty()) throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object address is null."));
        // Object is saved somewhere else, arCtx.pointer_ contains the old address, which is an id for finding the new object.
        ConstPointerInfo src_ptr_info = arParser.getAlreadyLoadedObject(arCtx.pointer_);
        if (nullptr == src_ptr_info.pointer_) {
            // Object is not loaded yet, this pointer will be stored for being updated later.
            // Set apPointerAddr to nullptr.
            pApplyPtr_->set(apPointerAddr, nullptr);
            return true;
        }
        else {
            // The object already loaded earlier, just assign the owner to this pointer.
            pApplyPtr_->assign(apPointerAddr, src_ptr_info);
            return false;
        }
    }

    /// addToAlreadyLoadedObjects() shall be called after this function.
    template <typename ParserT>
    void loadSavedInPlacePointer(ParserT& arParser, void* apPointerAddr, rOopsParserContext& arCtx) const
    {
        if (nullptr == apPointerAddr) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Object address is null."));
        }
        // When the object is saved-in-place, its address must be saved with the member name (e.g. MemberName*1234 = !MyObject {...}; ).
        // Create the member and assign it to the pointer.
        const rPropTypeInfo* pTI = rPropGetTypeInfoFromContext(arCtx);
        void* pVal = pTI->createObj();
        pTI->load(arParser, pVal, arCtx);
        // Store this pointer for resolving references, e.g. shared pointer.
        pApplyPtr_->set(apPointerAddr, pVal);
    }

    void loadNullPointer(void* apPointerAddr) const
    {
        // Both pointer_ and address_ are NULL.
        // This can happen, if the member is saved-in-place, but the pointer was NULL.
        // When the object is saved-in-place, its address must be saved with the member name (e.g. MemberName*0 = nullptr; ).
        // NULL pointer, set apPointerAddr to nullptr.
        pApplyPtr_->set(apPointerAddr, nullptr);
    }

    rApplyPointer* getApplyPointer() const
    {
        if (!pApplyPtr_) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("pApplyPtr_ is NULL."));
        }
        return pApplyPtr_.get();
    }
private:
    const rPropTypeInfo *pTypeInfo_{ nullptr }; ///< Reference to the type info object.
    std::shared_ptr<rApplyPointer> pApplyPtr_{ nullptr };
    std::string propName_; ///< Property name given in the property descriptor table.
    NameIdHashType propNameId_{ cInvalidHash }; ///< Hash value calculated from property name. It may be used in binary format instead of the property name.
    flags_type flags_{ rPropFlag::cDefault }; ///< Type modifier flags, like pointer.
    rPropDefault defaultValue_; ///< The default value may be different as the value assigned in the default constructor.
    offset_type offset_{ -1 }; ///< Offset of this member from the 'this'.
    enum class Type : std::uint8_t
    {
        eNone = 0,
        eMember,
        eAncestor,
        //eSetGetFunctions,
        eRunTime
    };
    Type type_{ Type::eNone };
    std::size_t _descTableIndex{ 0 }; ///< Index of this entry in the Property Descriptor Table.
}; //class rPropDescriptor


class rApplyRawPointer : public rApplyPointer
{
public:
    ~rApplyRawPointer() override = default;
    bool isPointer() const override
    {
        return true;
    }
// -Warray-bounds has to be disabled here for gcc due to a compiler bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104657
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    void* apply( void* apAddr ) const override
    {
        return *static_cast<void**>(apAddr);
    }
    const void* apply( const void* apAddr ) const override
    {
        return *static_cast<const void* const *>(apAddr);
    }
    void set( void* apObjAddr, void* apAddr ) const override
    {
        *static_cast<const void**>(apObjAddr) = apAddr;
    }
    void assign( void* apObjAddr, const ConstPointerInfo& aPtrInfo ) const override
    {
        *(const void**)apObjAddr = aPtrInfo.propDesc_->getApplyPointer()->apply(aPtrInfo.pointer_);
    }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
};

template < class PointerT, class ObjectT >
class rApplyUniquePointer : public rApplyPointer
{
public:
    ~rApplyUniquePointer() override = default;
    bool isPointer() const override
    {
        return true;
    }
    void* apply(void* apPtrAddr) const override
    {
        // Revert the type of the parameter.
        auto* pPtr = static_cast<PointerT*>(apPtrAddr);
        // Dereference the pointer.
        ObjectT& obj = **pPtr;
        // Return the address of the object as void*.
        return static_cast<void*>(&obj);
    }
    const void* apply(const void* apPtrAddr) const override
    {
        // Restore the type of the parameter.
        const auto* pPtr = static_cast<const PointerT*>(apPtrAddr);
        // Dereference the pointer.
        const ObjectT& obj = **pPtr;
        // Return the address of the pointed object as void*.
        return static_cast<const void*>(&obj);
    }
    void set(void* apPtrAddr, void* apAddr) const override
    {
            // Restore the type of the property parameter.
            auto* pPtr = static_cast<PointerT*>(apPtrAddr);
            // Restore the type of apAddr.
            auto* pObj = static_cast<ObjectT*>(apAddr);
            // Reset the smart pointer, if it is not already set to this object.
            if (pPtr->get() != pObj) pPtr->reset(pObj);
    }
    void assign(void*, const ConstPointerInfo&) const override
    {
        throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Assigning unique pointer is not allowed. This should never happen."));
    }
};

template < class PointerT, class ObjectT >
class rApplySharedPointer : public rApplyPointer
{
public:
    ~rApplySharedPointer() override = default;
    bool isPointer() const override
    {
        return true;
    }
    void* apply( void* apPtrAddr ) const override
        {
            // Revert the type of the parameter.
            auto* pPtr = static_cast<PointerT*>(apPtrAddr);
            // Dereference the pointer.
            ObjectT& obj = **pPtr;
            // Return the address of the object as void*.
            return static_cast<void*>(&obj);
        }
    const void* apply( const void* apPtrAddr ) const override
        {
            // Restore the type of the parameter.
            const auto* pPtr = static_cast<const PointerT*>(apPtrAddr);
            // Dereference the pointer.
            const ObjectT& obj = **pPtr;
            // Return the address of the pointed object as void*.
            return static_cast<const void*>(&obj);
        }
    void set( void* apPtrAddr, void* apAddr ) const override
    {
            // Restore the type of the property parameter.
            auto* pPtr = static_cast<PointerT*>(apPtrAddr);
            // Restore the type of apAddr.
            auto* pObj = static_cast<ObjectT*>(apAddr);
            // Reset the smart pointer, if it is not already set to this object.
            if (pPtr->get() != pObj) pPtr->reset(pObj);
    }
    void assign(void* apObjAddr, const ConstPointerInfo& arPtrInfo) const override
    {
        const auto* apply_ptr = dynamic_cast<rApplySharedPointer<PointerT, ObjectT>*>(arPtrInfo.propDesc_->getApplyPointer());
        if (nullptr == apply_ptr) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Assigning different type of pointer is not allowed."));
        }
        const auto* pSrcPtr = static_cast<const PointerT*>(arPtrInfo.pointer_);
        auto* pDstPtr = static_cast<PointerT*>(apObjAddr);
        *pDstPtr = *pSrcPtr;
    }
};

template < class OptionalT >
class rApplyOptional : public rApplyPointer
{
public:
    ~rApplyOptional() override = default;
    bool isOptional() const override
    {
        return true;
    }
    void* apply( void* apAddr ) const override
    {
        // Revert the type of the parameter.
        auto* pOptional = static_cast<OptionalT*>(apAddr);
        // Be sure it contains a value object.
        *pOptional = typename OptionalT::value_type();
        // Return the address of the object as void*.
        return static_cast<void*>(pOptional->operator->());
    }
    const void* apply(const void* apAddr) const override
    {
        // Revert the type of the parameter.
        const auto* pOptional = static_cast<const OptionalT*>(apAddr);
        // Return the address of the object as void* or nullptr if the optional has no value.
        return (pOptional->has_value()) ? static_cast<const void*>(pOptional->operator->()) : nullptr;
    }
    void set(void*, void*) const override
    {
        throw std::runtime_error("Not implemented.");
    }
    void assign(void*, const ConstPointerInfo&) const override
    {
        throw std::runtime_error("Not implemented");
    }
};

template < class T >
typename std::enable_if<!isPointerType<T>() && !is_Optional<T>::value, std::shared_ptr<rApplyPointer>>::type
createApplyPointer()
{
    return std::make_shared<rApplyPointer>();
}

template < class PointerT >
typename std::enable_if<std::is_pointer<PointerT>::value, std::shared_ptr<rApplyPointer>>::type
createApplyPointer()
{
    return std::make_shared<rApplyRawPointer>();
}

template < class PointerT >
typename std::enable_if<is_UniquePtr<PointerT>::value, std::shared_ptr<rApplyPointer>>::type
createApplyPointer()
{
    return std::make_shared< rApplyUniquePointer<PointerT, typename PointerT::element_type> >();
}

template < class PointerT >
typename std::enable_if<is_SharedPtr<PointerT>::value, std::shared_ptr<rApplyPointer>>::type
createApplyPointer()
{
    return std::make_shared< rApplySharedPointer<PointerT, typename PointerT::element_type> >();
}

template < class OptionalT >
typename std::enable_if<is_Optional<OptionalT>::value, std::shared_ptr<rApplyPointer>>::type
createApplyPointer()
{
    return std::make_shared< rApplyOptional<OptionalT> >();
}

} //namespace rOops
