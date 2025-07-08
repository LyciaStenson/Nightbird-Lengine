#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2012:
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
Property Interface
Main header file including all other headers
and defining the public macros.
*/


#include <oops/rPropTypeDef.h>
#include <oops/rPropTypeInfo.h>
#include <oops/rPropTypeInfoBase.h>
#include <oops/rPropTypeInfoCompound.h>
#include <oops/rPropTypeInfoContainer.h>
#include <oops/rPropTypeInfoAssociativeContainer.h>
#include <oops/rPropDescriptor.h>
#include <oops/rPropTypeTraits.h>
#include <type_traits>
#include <deque>
#include <list>
#include <map>
#include <vector>


/// This macro has to be added to the class declaration.
/** It implements the property interface functions and
creates an instance of the property type information object.
The OOPSGetTypeInfo() function is used to get the type info of the run time object
rather than the declared property. This is the fastest way
to get the type info of polymorphic classes.
*/
#define rOOPS_ADD_PROPERTY_INTERFACE_IMPL(aClassName, aAbstract, aPolymorph)  \
private:                                                                      \
    rOops::UnknownPropertyTable_t _unknowPropTbl;                             \
public:                                                                       \
    inline void addToUnknownPropTbl(size_t aIdx, const std::string& arValue)  \
        { _unknowPropTbl[aIdx] = arValue; }                                   \
    inline const std::string& getUnknownProperty(const size_t aIdx) const     \
        { auto iter = _unknowPropTbl.find(aIdx);                              \
          if (_unknowPropTbl.end()!=iter) { return iter->second; }            \
          else { return rOops::static_empty_string; }  }                      \
    inline size_t getUnknownPropTableOffset() const                           \
        { return offsetof(aClassName, _unknowPropTbl); }                      \
private:                                                                      \
    using type_info_type = typename rOops::CompoundTypeInfoSelector<          \
        aClassName,aAbstract,aPolymorph,true>::TypeInfo_type;                 \
    friend type_info_type;                                                    \
    rPropTypeInfoFactory<aClassName> _dummyTypeInfoFactory;                   \
public:                                                                       \
    static const rOops::rPropTypeInfo* createPropTypeInfo() {                 \
        static type_info_type sTypeInfo(#aClassName);                         \
        sTypeInfo.addAlias(rOops::getTypeName<aClassName>());                 \
        rPropTypeInfoFactory<aClassName>::_pTypeInfo = &sTypeInfo;            \
        return &sTypeInfo;                                                    \
    }                                                                         \
    using class_type = aClassName;                                            \
    static void createPropDescTableStaticImplementation(type_info_type* apTypeInfo)

// The rOOPS_ADD_PROPERTY_INTERFACE* macros are needed,
// because std::is_abstract() and std::is_polymorph() cannot be used,
// since these macros are used inside the class declaration,
// and the type (aClassName) is incomplete.

/// Add Property Interface to an abstract class.
#define rOOPS_ADD_PROPERTY_INTERFACE_ABSTRACT(aClassName)                     \
    virtual const rOops::rPropTypeInfo* OOPSGetTypeInfo() const               \
        { return aClassName::createPropTypeInfo(); }                          \
    rOOPS_ADD_PROPERTY_INTERFACE_IMPL(aClassName, true, true)

/// Add property interface to any non-abstract, polymorph class.
/**
 * For properly handling polymorphism a virtual function (OOPSGetTypeInfo())
 * is added to the class. Therefore the class should be a virtual destructor.
 */
#define rOOPS_ADD_PROPERTY_INTERFACE(aClassName)                              \
    virtual const rOops::rPropTypeInfo* OOPSGetTypeInfo() const               \
        { return aClassName::createPropTypeInfo(); }                          \
    rOOPS_ADD_PROPERTY_INTERFACE_IMPL(aClassName, false, true)

/// Add property interface to a non-polymorph class.
/**
 * The class must not have any virtual function, even virtual destructor.
 * The OOPSGetTypeInfo() is not virtual and this class cannot be referenced through base type pointers.
 * Use rOOPS_ADD_PROPERTY_INTERFACE() in case of any doubt.
 */
#define rOOPS_ADD_PROPERTY_INTERFACE_NON_POLYMORPH(aClassName)                \
    const rOops::rPropTypeInfo* OOPSGetTypeInfo() const                       \
        { return aClassName::createPropTypeInfo(); }                          \
    rOOPS_ADD_PROPERTY_INTERFACE_IMPL(aClassName, false, false)

/// Create the type information for the given type without touching the declaration or definition of the class.
#define rOOPS_DECLARE_TYPEINFO_CLASS(aaPropDescClassName, aaTypeName)                                           \
    using aaPropDescClassName##_BaseClass = rOops::CompoundTypeInfoSelector< aaTypeName,                        \
        std::is_abstract<aaTypeName>::value, std::is_polymorphic<aaTypeName>::value, false >::TypeInfo_type;    \
    template < class ClassT >                                                                                   \
    class aaPropDescClassName : public aaPropDescClassName##_BaseClass {                                        \
    public:                                                                                                     \
        aaPropDescClassName() : aaPropDescClassName##_BaseClass(#aaTypeName)                                    \
            { createPropDescTable(this); addAlias(rOops::getTypeName<aaTypeName>()); }                          \
    private:                                                                                                    \
        inline void createPropDescTable(aaPropDescClassName##_BaseClass* apTypeInfo);                           \
        static const rOops::rPropTypeInfo* createPropTypeInfo()                                                 \
        {                                                                                                       \
            static aaPropDescClassName<aaTypeName> sTypeInfo;                                                   \
            return &sTypeInfo;                                                                                  \
        }                                                                                                       \
        static const rOops::rPropTypeInfo* _pTypeInfo;                                                          \
        friend const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName*);                                 \
        friend const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName* const *);                         \
    };                                                                                                          \
    template<class ClassT>                                                                                      \
    const rOops::rPropTypeInfo* aaPropDescClassName<ClassT>::_pTypeInfo =                                       \
        aaPropDescClassName<ClassT>::createPropTypeInfo();                                                      \
    inline const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName*)                                      \
    {                                                                                                           \
        aaPropDescClassName<aaTypeName>::_pTypeInfo = aaPropDescClassName<aaTypeName>::createPropTypeInfo();    \
        return aaPropDescClassName<aaTypeName>::_pTypeInfo;                                                     \
    }                                                                                                           \
    inline const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName* const *)                              \
    {                                                                                                           \
        aaPropDescClassName<aaTypeName>::_pTypeInfo = aaPropDescClassName<aaTypeName>::createPropTypeInfo();    \
        return aaPropDescClassName<aaTypeName>::_pTypeInfo;                                                     \
    }                                                                                                           \
    template<class ClassT>                                                                                      \
    void aaPropDescClassName<ClassT>::createPropDescTable(aaPropDescClassName##_BaseClass* apTypeInfo)

// https://blog.galowicz.de/2016/02/20/short_file_macro/

#define rOOPS_TypeInfoFactoryName(aaPrefix) rOOPS_JOIN(rPropTypeInfoFactory_##aaPrefix,__LINE__)



/// This macro creates the createPropTypeInfo() function for the given type.
/** The type info object is created when the createPropTypeInfo() is called first.
 However if the rPropGetTypeInfoByName() function is called before of that it will return a nullptr.
 Therefore the rPropTypeInfoFactory instance ensures, that the type info object is created at static initialization time.
 Params:
    aaPrefix: helps to make unique name for TypeInfo class. e.g. SET_
    aaTypeName: creating PropTypeInfo for this type. e.g. std::set<int>.
    aaTypeInfo: template type info base class. e.g. rOops::rPropTypeInfoSTLSet
    1st ...: type name string, generally #aaTypeName
    2nd, 3rd, ...: Additional constructor parameters for the type info class. e.g. element type info
*/

#define rOOPS_InlineCreatePropTypeInfo(aaPrefix, aaTypeName, aaTypeInfo, ...)                    \
    template<class T>                                                                            \
    struct rOOPS_TypeInfoFactoryName(aaPrefix) {                                                 \
        rOOPS_TypeInfoFactoryName(aaPrefix)() {}                                                 \
        static const rOops::rPropTypeInfo* createPropTypeInfo() {                                \
            static aaTypeInfo sTypeInfo{__VA_ARGS__};                                            \
            sTypeInfo.addAlias(rOops::getTypeName<aaTypeName>());                                \
            return &sTypeInfo;                                                                   \
        }                                                                                        \
        static const rOops::rPropTypeInfo* _pTypeInfo;                                           \
    };                                                                                           \
    template<class ClassT>                                                                       \
    const rOops::rPropTypeInfo* rOOPS_TypeInfoFactoryName(aaPrefix)<ClassT>::_pTypeInfo =        \
        rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::createPropTypeInfo();                   \
    inline const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName*) {                     \
        rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::_pTypeInfo =                            \
            rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::createPropTypeInfo();               \
        return rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::_pTypeInfo;                      \
    }                                                                                            \
    inline const rOops::rPropTypeInfo* rPropGetTypeInfo(const aaTypeName* const *) {             \
        rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::_pTypeInfo =                            \
            rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::createPropTypeInfo();               \
        return rOOPS_TypeInfoFactoryName(aaPrefix)<aaTypeName>::_pTypeInfo;                      \
    }

#define rOOPS_EnumTypeInfoTypeName rOOPS_JOIN(EnumTypeInfo_,__LINE__)

#define rOOPS_DECLARE_USER_DEFINED_TYPE_INFO(aaTypeName, aaTypeInfo)                             \
    rOOPS_InlineCreatePropTypeInfo(USER_, aaTypeName, aaTypeInfo, #aaTypeName)

#define rOOPS_DECLARE_ENUM_TYPE_INFO(aTypeName,aConv)                                            \
    using rOOPS_EnumTypeInfoTypeName = rOops::rPropTypeInfoEnum<aTypeName,aConv>;                \
    rOOPS_InlineCreatePropTypeInfo(ENUM_, aTypeName, rOOPS_EnumTypeInfoTypeName, #aTypeName)

#define rOOPS_DECLARE_STL_ARRAY_TYPE_INFO(aTypeName)                                             \
    rOOPS_InlineCreatePropTypeInfo(ARRAY_, aTypeName, rOops::rPropTypeInfoArray<aTypeName>, #aTypeName, rPropGetTypeInfo(static_cast<rOops::rPropRemovePointer<aTypeName::value_type>::type*>(nullptr)))

#define rOOPS_DECLARE_STL_LIST_TYPE_INFO(aTypeName)                                              \
    rOOPS_InlineCreatePropTypeInfo(LIST_, aTypeName, rOops::rPropTypeInfoSTLList<aTypeName>, #aTypeName, rPropGetTypeInfo(static_cast<rOops::rPropRemovePointer<aTypeName::value_type>::type*>(nullptr)))

#define rOOPS_DECLARE_STL_SET_TYPE_INFO(aTypeName)                                               \
    rOOPS_InlineCreatePropTypeInfo(SET_, aTypeName, rOops::rPropTypeInfoSTLSet<aTypeName>, #aTypeName, rPropGetTypeInfo(static_cast<rOops::rPropRemovePointer<aTypeName::value_type>::type*>(nullptr)))

#define rOOPS_DECLARE_ASSOCIATIVE_CONTAINER_TYPE_INFO(aTypeName)                                             \
    rOOPS_InlineCreatePropTypeInfo(                                                                          \
        MAP_, aTypeName, rOops::rPropTypeInfoAssociative<aTypeName>, #aTypeName,                             \
        rPropGetTypeInfo(static_cast<rOops::rPropRemovePointer<aTypeName::key_type>::type*>(nullptr)),       \
        rPropGetTypeInfo(static_cast<rOops::rPropRemovePointer<aTypeName::mapped_type>::type*>(nullptr)))


#define rOOPS_PROP_FLAG_DEFAULT (::rOops::rPropFlag::cDefault)

#define rOOPS_VERSION(...)                        \
    apTypeInfo->setVersionNumber(__VA_ARGS__)

#define rOOPS_VALIDATE(aaValidateFunction)        \
    apTypeInfo->setValidator(aaValidateFunction)

#define rOOPS_INHERIT( aParent )                                    \
    apTypeInfo->inheritBaseClass(                                   \
        ::rPropGetTypeInfo(static_cast<const aParent*>(nullptr)),   \
        rOops::getBaseClassOffset<class_type,aParent>(),            \
        rOOPS_PROP_FLAG_DEFAULT)

#define rOOPS_ADD_BASE_CLASS_PROPERTIES( aParent )                  \
    apTypeInfo->addBaseProperties(                                  \
        ::rPropGetTypeInfo(static_cast<const aParent*>(nullptr)),   \
        rOops::getBaseClassOffset<class_type,aParent>(),            \
        rOOPS_PROP_FLAG_DEFAULT)

#define rOOPS_PROPERTY( aMemberName )                                                                                           \
    apTypeInfo->addPropDesc(                                                                                                    \
        ::rPropGetTypeInfo(static_cast<typename rOops::rPropRemovePointer<decltype(class_type::aMemberName)>::type*>(nullptr)), \
        rOops::createApplyPointer<decltype(class_type::aMemberName)>(),                                                         \
        #aMemberName, offsetof(class_type,aMemberName), rOOPS_PROP_FLAG_DEFAULT)

//#define rOOPS_PROPERTY_SMARTPTR( aMemberName ) rOOPS_PROPERTY( aMemberName )

//#define rOOPS_OPTIONAL( aMemberName ) rOOPS_PROPERTY( aMemberName )

#define rOOPS_PROPERTY_SET_GET_FUNC( aType, aSetFuncName, aGetFuncName )            \
    apTypeInfo->addPropDesc( new rOops::rPropDescSetGetFunc<class_type,aType>(      \
        ::rPropGetTypeInfo((const aType*)0),                                        \
        #aSetFuncName, &class_type::aSetFuncName, &class_type::aGetFuncName,        \
        rOOPS_PROP_FLAG_DEFAULT) )

#define rOOPS_PROPERTY_SET_GET_FUNC2( aType, aSetFuncName, aGetFuncName )           \
    apTypeInfo->addPropDesc( new rOops::rPropDescSetGetFunc2<class_type,aType>(     \
        ::rPropGetTypeInfo((const aType*)0),                                        \
        #aSetFuncName, &class_type::aSetFuncName, &class_type::aGetFuncName,        \
        rOOPS_PROP_FLAG_DEFAULT) )

//#define rOOPS_FUNCTION( aFuncName )

//#define rOOPS_FUNC_SET_GET( aSetFuncName, aGetFuncName )

// Frequently used type descriptors:
rOOPS_DECLARE_STL_LIST_TYPE_INFO(std::vector<std::int32_t>)
rOOPS_DECLARE_STL_LIST_TYPE_INFO(std::vector<double>)
rOOPS_DECLARE_STL_LIST_TYPE_INFO(std::deque<double>)
//rOOPS_DECLARE_STL_LIST_TYPE_INFO(std::list<short>)
