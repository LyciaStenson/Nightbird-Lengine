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
Type Info Interface Class.
*/

#include <oops/rOopsParserContext.h>
#include <oops/rPropConstIter.h>
#include <oops/rPropIter.h>
#include <oops/rPropTypeDef.h>
#include <oops/rPropLog.h>

#include <chrono>
#include <exception>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace rOops
{

    class rPropDescriptor;
    class rOopsBinaryFormat;
    class rOopsTextFormat;
    class rOopsYamlFormat;
    class rOopsBinaryParser;
    class rOopsTextParser;
    class rOopsYamlParser;

    /// Reference to this string is returned when the required string does not exists for some reason.
    /// e.g. getUnknownProperty() does not find the requested property.
    /// This is should never happen, but for returning empty string is more friendly than throwing an exception.
    static const std::string static_empty_string;

    /// Type Information class for Run-time Type Identification.
    /**
     * Every class having rOops Type Description support
     * must have a global constant instantiation of
     * the appropriate descendant class of 'rPropTypeInfo'.
     */
    class rPropTypeInfo
    {
    public:
	    /// Type identifier is used as a numeric type id.
        /**
         * How to generate it? Shall be unique only in an application or worldwide?
         * Its size should not be increased for efficient storage.
         */
        using id_type = TypeIdHashType;
        /// Map for storing type info pointers by type name.
        typedef std::unordered_map< std::string, const rPropTypeInfo* > NameMap_t;
        /// Map for storing type info pointers by type id.
        typedef std::unordered_map< id_type, const rPropTypeInfo* > IdMap_t;
        /// Get type info by name.
        friend const rPropTypeInfo* rPropGetTypeInfoByName( const std::string& arName );
        /// Get type info by type identifier.
	    friend const rPropTypeInfo* rPropGetTypeInfoById( const rPropTypeInfo::id_type& arId );
    public:
    // Constructors:
        virtual ~rPropTypeInfo();
        rPropTypeInfo() noexcept;
        explicit rPropTypeInfo( const std::string& arTypeName ) noexcept;
        rPropTypeInfo(const rPropTypeInfo&) = delete;
        rPropTypeInfo(const rPropTypeInfo&&) = delete;
        rPropTypeInfo& operator=(const rPropTypeInfo&) = delete;
        rPropTypeInfo& operator=(const rPropTypeInfo&&) = delete;
    // Type identification:
        /// Get the string identifier of the type.
        const std::string& typeName() const { return typeName_; }
        /// Add alias name to the typename map.
        /// Type name is passed by value and spaces are replaced with '_'.
        void addAlias(std::string aliasTypeName) const;
        /// Get the numeric identifier of the type.
        /// 0 is invalid, means id not initialized.
        id_type TypeId() const { return typeId_; }
        /// 'Size()' returns the size of the object the type descriptor belongs to.
        /** This is the size, what the object reserve in the memory.
        It is used to calculate the address of the next item. */
        virtual size_type Size() const = 0;

    // Create and destroy objects:
        /// Create single object with 'new'.
        virtual void *createObj() const = 0;
        /// Create an object with placement new.
        virtual void *createObj( void *apData ) const = 0;
        /// Create array of objects with 'new[aN]'.
        virtual void *createObj( size_type aN ) const = 0;
        /// Destroy single object with 'delete'.
        virtual void destroyObj(void* apObj) const = 0;
        /// Destroy object allocated with placement new, i.e. call the destructor only.
        virtual void destroyObj( void* apObj, void* apData ) const = 0;
        /// Destroy array of objects with 'delete[]'.
        virtual void destroyObj(void* apObjArr, size_type aN) const = 0;

    // Functions for getting and setting the value of variable:
        /// Converts the value of the variable pointed by the argument to string.
        virtual std::string value( const void *apVariable ) const = 0;
        virtual void value( const void *apVariable, std::string& arValue ) const = 0;
        /// Set the variable pointed by the first argument to the value converted from the string passed in the 2nd argument.
        virtual void setValue(void *apVariable, const std::string& arValue) const = 0;
        virtual void addUnknownProperty(void* /*apObjAddr*/, const std::string& /*arType*/, const std::string& /*arName*/, const std::string& /*arValue*/) const
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("addUnknownProperty() is not implemented for " << typeName_));
        }
        virtual void setUnknownProperty(void* /*apObjAddr*/, size_t /*aIndex*/, const std::string& /*arValue*/) const
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("setUnknownProperty() is not implemented for " << typeName_));
        }
        virtual const std::string& getUnknownProperty(const void* /*apObjAddr*/, size_t /*aIndex*/) const
        {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("getUnknownProperty() is not implemented for " << typeName_));
        }
        /// Get value of property and write it to a stream using the arFormat class.
        virtual void save(rOopsBinaryFormat& arFormat, const void *apVal) const = 0;
        virtual void save(rOopsTextFormat& arFormat, const void *apVal) const = 0;
        virtual void save(rOopsYamlFormat& arFormat, const void *apVal) const = 0;
        /// Load value of property (*apVal) using the given parser.
        virtual void load(rOopsBinaryParser& arParser, void* apVal, rOopsParserContext& arCtx) const = 0;
        virtual void load(rOopsTextParser& arParser, void* apVal, rOopsParserContext& arCtx) const = 0;
        virtual void load(rOopsYamlParser& arParser, void* apVal, rOopsParserContext& arCtx) const = 0;

        /// Get a property iterator pointing to this object.
        virtual rPropIterator getIter( void* apVal, const std::string& arName ) const;
        virtual rPropConstIterator getIter(const void* apVal, const std::string& arName) const;
        rPropConstIterator getConstIter(const void* apVal, const std::string& arName) const { return getIter(apVal,arName);  }
        /// If the property is a container, returns a property iterator pointing to the first element in the container.
        /** Returns an end iterator if this is not a container. */
        virtual rPropIterator begin( void* apVal, const std::string& arName ) const = 0;
        virtual rPropConstIterator begin( const void* apVal, const std::string& arName ) const = 0;
        virtual rPropConstIterator cbegin( const void* apVal, const std::string& arName ) const { return begin(apVal, arName); }
        /// Find the property having the given name or type.
        /** If the property is a compound type,
        \c find() returns a property iterator pointing to the first property having the given name.
        \c find() only search in the property description table of the given type.
        It does not search in descendant types.
        Returns an end iterator if the property with given name is not found. */
        //rPropIterator find( void* apVal, const std::string& arPropName, rPropIterator aiProp=rPropIterator() ) const;
        rPropIterator find1st( void* apVal, const std::string& arPropName, rPropIterator aiProp=rPropIterator() ) const;
        //size_type findAll( void* apVal, const std::string& arPropName, rPropIterVector_t& arResults ) const;
        //rPropIterator find( void* apVal, const rPropTypeInfo* apPropType ) const;
        //size_type findAll( void* apVal, const rPropTypeInfo* apPropType, rPropIterVector_t& arResults ) const;
        rPropConstIterator find( const void* apVal, const std::string& arPropName, rPropConstIterator aiProp=rPropConstIterator() ) const;
        rPropConstIterator find1st( const void* apVal, const std::string& arPropName, rPropConstIterator aiProp=rPropConstIterator() ) const;
        //size_type findAll( const void* apVal, const std::string& arPropName, rPropConstIterVector_t& arResults ) const;
        //rPropConstIterator find( const void* apVal, const rPropTypeInfo* apPropType, rPropIterator aiProp=rPropIterator() ) const;
        //size_type findAll( const void* apVal, const rPropTypeInfo* apPropType, rPropConstIterVector_t& arResults ) const;
        bool isClass( const rOops::rPropTypeInfo* apTIClass ) const;
        bool isClass(const std::string& aClass) const;

        /// Returns true, if this type is simple data and cannot contain other properties or elements.
        virtual bool isValue() const = 0;
        /// Returns the suggested bounding character. " for strings and ' for character. It is just a suggestion used in serialization.
        virtual char getOpeningCharacter() const { return '\0'; }
        virtual char getClosingCharacter() const { return '\0'; }
        /// Returns true, if this type is class or struct.
        virtual bool isCompound() const { return false; }
        /// Returns true, if the described type is an abstract class.
        virtual bool isAbstract() const { return false; }
        /// Returns true, if the described type is a polymorphic class.
        virtual bool isPolymorphic() const { return false; }
        /// Returns true, if the described type is has property interface. (Its declaration contains the rOOPS_ADD_PROPERTY_INTERFACE() macro.)
        virtual bool isIntrusive() const { return false; }
        /// Returns the type descriptor of the real object in case of polymorphic types.
        virtual const rPropTypeInfo* objTypeInfo( const void* /*apObjAddr*/ ) const { return this; }

        virtual std::size_t getPropDescSize() const { return 0; }
        virtual const rPropDescriptor* getPropDesc( rOops::size_type /*aIdx*/ ) const { return nullptr; }
        void setVersionNumber(std::uint8_t aVersion)
        {
            version_ = aVersion;
        }
        std::uint32_t version() const { return version_; }
        virtual void validate(void* /*apObjAddr*/, std::uint32_t aVersion) const
        {
            if (0 < version_) {
                if (version_ != aVersion) {
                    throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Version numbers are different: " << static_cast<unsigned>(version_) << " != " << static_cast<unsigned>(aVersion)));
                }
            }
        }

        // Only containers override the following member functions:
        /// Returns true, if this type is a standard container.
        virtual bool isSTLContainer() const { return false; }
        /// Returns the number of elements stored in the container.
        virtual std::size_t nrOfElements( const void* /*apContainer*/ ) const { return 1u; }
        /// Sets the size of a container. Nothing to do in all other cases.
        virtual void setNrOfElements( void* /*apContainer*/, std::size_t /*aSize*/ ) const
            {}
        /// Returns the type info object of elements stored in the container.
        virtual const rPropTypeInfo* elementTypeInfo() const
            { return nullptr; }
        virtual const rPropDescriptor* elementPropDesc() const
            { return nullptr; }
        virtual rPropDescriptor* elementPropDesc()
            { return nullptr; }

        // Only associative containers override the following member functions:
        /// Returns true, if this type is an associative container.
        virtual bool isAssociativeContainer() const
            { return false; }
        /// Returns the type info object of keys.
        virtual const rPropTypeInfo* keyTypeInfo() const
            { return nullptr; }
        virtual const rPropDescriptor* keyPropDesc() const
            { return nullptr; }
        virtual rPropDescriptor* keyPropDesc()
            { return nullptr; }

        bool operator==( const rPropTypeInfo &a2 );
        bool operator!=( const rPropTypeInfo &a2 );
        bool operator==( const std::string& arTypeName );
        static NameMap_t* getNameMap() { return gpNameMap; }
    protected:
        void _registerType();
        void _unregisterType();
    protected:
        /// Binary identifier of the type. It is used to check the validity of an address.
        id_type typeId_{ 0 };
        /// ASCII identifier of the type.
        std::string typeName_;
        /// Store the version number of the type. Binary stream stores version number as uint8 only, but other formats can support higher version numbers as well.
        std::uint32_t version_{ 0 };
        /// Link 'rPropTypeInfo' structures.
        rPropTypeInfo *_pNext{ nullptr };
        /// Map of type descriptors for quickly find them by name, and check if the name is unique.
        static NameMap_t* gpNameMap;
        /// Map of type descriptors for quickly find them by id, and check if the id is unique.
        static IdMap_t* gpIdMap;
    }; //class rPropTypeInfo

    using rPropTypeInfoPtr_t = rPropTypeInfo*;

    /// Create the object on the heap.
    /** The function looks up for the type info object of the given type
    and call its createObj() member for creating the object on the heap.
    Return nullptr, if the type name is not registered. */
    void* rPropCreateObj( const std::string& arTypeName );

    const rPropTypeInfo* rPropGetTypeInfoByName( const std::string& arName );
    const rPropTypeInfo* rPropGetTypeInfoById( const rPropTypeInfo::id_type& arId );

    void replaceSpaces(std::string& arTypeName);
} // namespace rOops


#define rOOPS_DO_JOIN( a1, a2 ) a1##a2
#define rOOPS_JOIN( a1, a2 ) rOOPS_DO_JOIN( a1, a2 )

// Integral types. Global namespace.

#define rOOPS_DECLARE_TYPEINFO(aaTypeName)                               \
    const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName *);    \
    const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName * const *);

rOOPS_DECLARE_TYPEINFO(bool)
rOOPS_DECLARE_TYPEINFO(char)
//rOOPS_DECLARE_TYPEINFO(signed char)
//rOOPS_DECLARE_TYPEINFO(unsigned char)

rOOPS_DECLARE_TYPEINFO(int8_t)
rOOPS_DECLARE_TYPEINFO(int16_t)
rOOPS_DECLARE_TYPEINFO(int32_t) // int
rOOPS_DECLARE_TYPEINFO(int64_t) // long long

rOOPS_DECLARE_TYPEINFO(uint8_t)
rOOPS_DECLARE_TYPEINFO(uint16_t)
rOOPS_DECLARE_TYPEINFO(uint32_t) // unsigned int
rOOPS_DECLARE_TYPEINFO(uint64_t) // unsigned long lon
// The int*_t typedefs are compiler dependent.
// Some additional definitions are required to cover all integer types,
// but depending on the compiler they are different.
#if defined(__clang__)
rOOPS_DECLARE_TYPEINFO(long long)
rOOPS_DECLARE_TYPEINFO(unsigned long long)
#elif defined(__GNUC__)
rOOPS_DECLARE_TYPEINFO(long long)
rOOPS_DECLARE_TYPEINFO(unsigned long long)
#elif defined(_MSC_VER)
rOOPS_DECLARE_TYPEINFO(long)
rOOPS_DECLARE_TYPEINFO(unsigned long)
#endif

rOOPS_DECLARE_TYPEINFO(float)
rOOPS_DECLARE_TYPEINFO(double)
rOOPS_DECLARE_TYPEINFO(long double)

rOOPS_DECLARE_TYPEINFO(std::string)

rOOPS_DECLARE_TYPEINFO(std::chrono::nanoseconds)
rOOPS_DECLARE_TYPEINFO(std::chrono::microseconds)
rOOPS_DECLARE_TYPEINFO(std::chrono::milliseconds)
rOOPS_DECLARE_TYPEINFO(std::chrono::seconds)
rOOPS_DECLARE_TYPEINFO(std::chrono::minutes)
rOOPS_DECLARE_TYPEINFO(std::chrono::hours)

rOOPS_DECLARE_TYPEINFO(std::chrono::system_clock::time_point)
rOOPS_DECLARE_TYPEINFO(std::chrono::steady_clock::time_point)


template<class T>
struct rPropTypeInfoFactory
{
    rPropTypeInfoFactory() = default;
    static const rOops::rPropTypeInfo* _pTypeInfo;
};

// SFINAE test
template <typename T>
class rPropHasCreatePropTypeInfo
{
private:
    typedef char YesType[1];
    typedef char NoType[2];
    template <typename C> static YesType& test( decltype(&C::createPropTypeInfo) ) ;
    template <typename C> static NoType& test(...);
public:
    enum { value = sizeof(test<T>(nullptr)) == sizeof(YesType) };
};

template<class T>
const rOops::rPropTypeInfo* rPropTypeInfoFactory<T>::_pTypeInfo = T::createPropTypeInfo();

template<class T>
typename std::enable_if<rPropHasCreatePropTypeInfo<T>::value, const rOops::rPropTypeInfo*>::type
rPropGetTypeInfo(const T *)
{
    rPropTypeInfoFactory<T>::_pTypeInfo = T::createPropTypeInfo();
    return rPropTypeInfoFactory<T>::_pTypeInfo;
}

template<class T>
typename std::enable_if<rPropHasCreatePropTypeInfo<T>::value, const rOops::rPropTypeInfo*>::type
rPropGetTypeInfo(const T * const *)
{
    rPropTypeInfoFactory<T>::_pTypeInfo = T::createPropTypeInfo();
    return rPropTypeInfoFactory<T>::_pTypeInfo;
}


/// Helper macro for creating the a unique name for global function: createTypeInfoXXX.
/// The type name may contain space, colon, and other characters not allowed in variable names.
#define rOOPS_createTypeInfoName(aUniqueName) rOOPS_JOIN(createTypeInfo,aUniqueName)

/// This macro creates the rPropGetTypeInfo() functions and the property descriptor for the given type.
#define rOOPS_CreateNamedTypeInfoObject(aaTypeName, aTypeInfo, aUniqueName)      \
    const rOops::rPropTypeInfo* rOOPS_createTypeInfoName(aUniqueName)() {        \
            static aTypeInfo sTypeInfo(#aaTypeName);                             \
            sTypeInfo.addAlias(rOops::getTypeName<aaTypeName>());                \
            return &sTypeInfo;                                                   \
        }                                                                        \
    const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName *)             \
        { return rOOPS_createTypeInfoName(aUniqueName)(); }                      \
    const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName * const *)     \
        { return rOOPS_createTypeInfoName(aUniqueName)(); }

/// This macro creates the rPropGetTypeInfo() functions and the property descriptor for the given type.
/// This version is not recommended, because the unique function name is generated using the __LINE__ macro.
/// It may happen, that this macro is used on the same line of 2 independent source files,
/// resulting strange linker errors.
#define rOOPS_CreateTypeInfoObject(aaTypeName, aTypeInfo)                        \
    rOOPS_CreateNamedTypeInfoObject(aaTypeName, aTypeInfo, rOOPS_JOIN(createTypeInfo,__LINE__))

template <typename TypeT>
const rOops::rPropTypeInfo* rPropGetTypeInfo()
{
    TypeT* ptr = nullptr;
    return rPropGetTypeInfo(ptr);
}

/// Helper function for converting value to string.
template < typename ValueT >
std::string rPropGetValue(const ValueT& aVal)
{
    return rPropGetTypeInfo(&aVal)->value(&aVal);
}


/*
Implementation notes.
=====================

The type info object provides all necessary information about all types handled by the rOops library.
Built in type info class is provided for the internal types (int, double, string, etc.).
Template type info classes are provided for compound and template classes, as well as for containers.
The goal is to minimize the usage overhead.

Compound types have properties (data or function members) and the description table of these members has to be added to the type info object.
The createPropDescTableStaticImplementation() static function creates the descriptor table.
It's second argument differentiates between functions for different classes.
Compound types have several types requiring different type info object: abstract, polymorphic, etc.
The boost type traits library is used to select the appropriate type descriptor at compile time.
The CompoundTypeInfoSelector class returns the appropriate type. It cannot be used inside the class declaration,
e.g. the type of the first argument for the createPropDescTableStaticImplementation() function
cannot be automatically detected at the friend declaration. This is why we need the second argument.

Handling pointers and polymorphic objects.
Pointers are handled as type modifiers, it belongs to the property descriptor, but needs some help from the type info object.
For handling polymorphic types, we need to get the type info object of the real object rather than the type info of the pointer type.
There are 3 different situation here:
 - The fastest is the intrusive solution, when a virtual function is added to the class for returning the type info object of the object.
 - Polymorphic classes can be identified with the address of their virtual member table.
   This is slower, because the type info object has to be searched in an associative container indexed by the address of the vtable.
   Optimization: store independent class hierarchies in independent containers.
 - Classes with no virtual functions cannot get the real type info object, but it is ok, because it means, that the class is not polymorph.
   In theory, it is possible to store a descendant object in the base class type pointer, but it does not make sense, because the program also cannot handle it.
*/
