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

#include <oops/rPropTypeInfo.h>
#include <oops/rPropLog.h>
#include <oops/rPropFind.h>
#include <oops/rPropIter_impl.h>
#include <oops/rPropTypeInfoBase.h>
#include <unordered_map>

namespace rOops
{

    void replaceSpaces(std::string& arTypeName)
    {
        // Type name should not contain any whitespace character.
        // e.g. const ObjType*
        // TODO: any other character to replace? * &
        for (std::size_t idx=0; idx<arTypeName.size(); ++idx) {
            if (' ' == arTypeName[idx]) {
                arTypeName[idx] = '_';
            }
        }
    }

    // static members:
    rPropTypeInfo::NameMap_t* rPropTypeInfo::gpNameMap = nullptr;
    rPropTypeInfo::IdMap_t* rPropTypeInfo::gpIdMap = nullptr;

    rPropTypeInfo::~rPropTypeInfo()
    {
        _unregisterType();
        rDbgTrace("Destroy type info of " << typeId_ << ":" << typeName_)
    }

    rPropTypeInfo::rPropTypeInfo() noexcept = default;

    rPropTypeInfo::rPropTypeInfo( const std::string& arTypeName ) noexcept
        : typeId_(generateTypeId(arTypeName))
        , typeName_(arTypeName)
    {
        replaceSpaces(typeName_);
        rDbgTrace( "Type info of " << typeId_ << ":" << typeName_ << " created.")
        if(typeName_ != "RunTimeType")
            _registerType();
    }

    void rPropTypeInfo::addAlias(std::string aliasTypeName) const
    {
        replaceSpaces(aliasTypeName);
        if(nullptr==gpNameMap ) gpNameMap = new rPropTypeInfo::NameMap_t();
        auto it = gpNameMap->find(aliasTypeName);
        if( gpNameMap->end() != it ) {
            if (this==it->second) {
                rDbgTrace( "Type info with the same name is already added for " << aliasTypeName << " type.");
            }
            else {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Alias type name '" << aliasTypeName << "' is not a unique name."));
            }
        }
        else {
            (*gpNameMap)[aliasTypeName] = this;
            rDbgTrace( "Type info alias " << type_name << " is added to " << typeName_ << ".");
        }
    }

    void rPropTypeInfo::_registerType()
    {
        if (typeName_.empty()) return;
        if(nullptr==gpNameMap ) gpNameMap = new rPropTypeInfo::NameMap_t();
        if(nullptr==gpIdMap ) gpIdMap = new rPropTypeInfo::IdMap_t();
        // Check if the name and id is unique.
        if( gpNameMap->end() != gpNameMap->find(typeName_) ) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type name '" << typeName_ << "' is not a unique name."));
        }
        if( gpIdMap->end() != gpIdMap->find(typeId_) ) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Type id '" << typeId_ << "' of '" << typeName_ << "' is not a unique type id."));
        }
        (*gpNameMap)[typeName_] = this;
        (*gpIdMap)[typeId_] = this;
    }

    void rPropTypeInfo::_unregisterType()
    {
        if(nullptr != gpNameMap ) {
            gpNameMap->erase( typeName_ );
            if( gpNameMap->empty() ) {
                delete gpNameMap;
                gpNameMap = nullptr;
            }
        }
        if(nullptr != gpIdMap ) {
            gpIdMap->erase( typeId_ );
            if( gpIdMap->empty() ) {
                delete gpIdMap;
                gpIdMap = nullptr;
            }
        }
    }

    rPropIterator rPropTypeInfo::getIter( void* apVal, const std::string& arName ) const
    {
        return { std::make_unique<rPropIteratorValue>(this,apVal,arName) };
    }

    rPropConstIterator rPropTypeInfo::getIter( const void* apVal, const std::string& arName) const
    {
        return { std::make_unique<rPropConstIterValue>(this,apVal,arName) };
    }

    rPropIterator rPropTypeInfo::find1st( void* apVal, const std::string& arPropName, rPropIterator aiProp ) const
    {
        if( !aiProp.isValid() ) aiProp = begin( apVal, "" );
        return rOops::find1stImpl( aiProp, arPropName );
    }

    rPropConstIterator rPropTypeInfo::find( const void* apVal, const std::string& arPropName, rPropConstIterator aiProp ) const
    {
        if( !aiProp.isValid() ) aiProp = begin( apVal, "" );
        while( !aiProp.isEnd() ) {
            if( arPropName == aiProp.propName() ) break;
            ++aiProp;
        } //while
        if (aiProp.isEnd() && rOops::globalConfig().errorIfPropNameNotFound) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Property name not found: " << arPropName));
        }
        else {
            rDbgWarning("Property name not found: " << arPropName);
        }
        return aiProp;
    }

    rPropConstIterator rPropTypeInfo::find1st( const void* apVal, const std::string& arPropName, rPropConstIterator aiProp ) const
    {
        if( !aiProp.isValid() ) aiProp = begin( apVal, "" );
        return rOops::find1stImpl( aiProp, arPropName );
    }

    bool rPropTypeInfo::isClass( const rOops::rPropTypeInfo* apTIClass ) const
    {
        if( apTIClass == this ) return true;
        if( isCompound() ) {
            rOops::size_type idx = 0;
            for( ; ; ++idx ) {
                const rPropDescriptor* pPD = getPropDesc(idx);
                if(nullptr == pPD ) return false;
                if( !pPD->isAncestor() ) return false; // must be an ancestor
                if( pPD->propTypeInfo()->isClass(apTIClass) ) return true;
            } //while
        }
        return false;
    }

    bool rPropTypeInfo::isClass( const std::string& aClass ) const
    {
        return isClass(rPropGetTypeInfoByName(aClass));
    }

    bool rPropTypeInfo::operator==( const rPropTypeInfo &a2 )
    {
        if( this == &a2 ) return true;
        if( 0!=typeId_ && typeId_== a2.typeId_) return true;
        return typeName_==a2.typeName_;
    } //operator==()


    bool rPropTypeInfo::operator!=( const rPropTypeInfo &a2 )
    {
        return !this->operator==(a2);
    } //operator!=()


    bool rPropTypeInfo::operator==(const std::string& arTypeName)
    {
        return typeName_==arTypeName;
    } //operator==()


    const rPropTypeInfo* rPropGetTypeInfoByName(const std::string& arName)
    {
        if (!arName.empty()) {
            std::string type_name = arName;
            replaceSpaces(type_name);
            auto iter = rPropTypeInfo::gpNameMap->find(type_name);
            if( iter != rPropTypeInfo::gpNameMap->end() ) {
                return iter->second;
            }
        }
        return nullptr;
    }


    const rPropTypeInfo* rPropGetTypeInfoById(const rPropTypeInfo::id_type& arId)
    {
        auto iter = rPropTypeInfo::gpIdMap->find(arId);
        if( iter != rPropTypeInfo::gpIdMap->end() ) {
            return iter->second;
        }
        return nullptr;
    }

    void* rPropCreateObj(const std::string& arName)
    {
        const rPropTypeInfo* pTI = rPropGetTypeInfoByName(arName);
        if (nullptr == pTI) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Unknown type: " << arName));
        }
        return pTI->createObj();
    }

} //namespace rOops

rOOPS_CreateNamedTypeInfoObject(bool, rOops::rPropTypeInfoBool, Bool)

// char, signed char (int8_t), and unsigned char (uint8_t) are different types.
rOOPS_CreateNamedTypeInfoObject(char, rOops::rPropTypeInfoChar<char>, Char)

rOOPS_CreateNamedTypeInfoObject(int8_t, rOops::rPropTypeInfoCharAsInt<int8_t>, Int8)
rOOPS_CreateNamedTypeInfoObject(int16_t, rOops::rPropTypeInfoBaseType<int16_t>, Int16)
rOOPS_CreateNamedTypeInfoObject(int32_t, rOops::rPropTypeInfoBaseType<int32_t>, Int32)
rOOPS_CreateNamedTypeInfoObject(int64_t, rOops::rPropTypeInfoBaseType<int64_t>, Int64)

rOOPS_CreateNamedTypeInfoObject(uint8_t, rOops::rPropTypeInfoCharAsInt<uint8_t>, UInt8)
rOOPS_CreateNamedTypeInfoObject(uint16_t, rOops::rPropTypeInfoBaseType<uint16_t>, UInt16)
rOOPS_CreateNamedTypeInfoObject(uint32_t, rOops::rPropTypeInfoBaseType<uint32_t>, UInt32)
rOOPS_CreateNamedTypeInfoObject(uint64_t, rOops::rPropTypeInfoBaseType<uint64_t>, UInt64)

// The int*_t typedefs are compiler dependent.
// Some additional definitions are required to cover all integer types,
// but depending on the compiler they are different.
#if defined(__clang__)
rOOPS_CreateNamedTypeInfoObject(long long, rOops::rPropTypeInfoBaseType<long long>, LongLong)
rOOPS_CreateNamedTypeInfoObject(unsigned long long, rOops::rPropTypeInfoBaseType<unsigned long long>, UnsignedLongLong )
#elif defined(__GNUC__)
rOOPS_CreateNamedTypeInfoObject(long long, rOops::rPropTypeInfoBaseType<long long>, LongLong)
rOOPS_CreateNamedTypeInfoObject(unsigned long long, rOops::rPropTypeInfoBaseType<unsigned long long>, UnsignedLongLong )
#elif defined(_MSC_VER)
rOOPS_CreateNamedTypeInfoObject(long, rOops::rPropTypeInfoBaseType<long>, Long)
rOOPS_CreateNamedTypeInfoObject(unsigned long, rOops::rPropTypeInfoBaseType<unsigned long>, UnsignedLong)
#endif

rOOPS_CreateNamedTypeInfoObject(float, rOops::rPropTypeInfoFloat, Float)
rOOPS_CreateNamedTypeInfoObject(double, rOops::rPropTypeInfoDouble, Double)
rOOPS_CreateNamedTypeInfoObject(long double, rOops::rPropTypeInfoLongDouble, LongDouble)

rOOPS_CreateNamedTypeInfoObject(std::string, rOops::rPropTypeInfoStdString, StdString)

// Chrono duration types:
rOOPS_CreateNamedTypeInfoObject(std::chrono::nanoseconds, rOops::rPropTypeInfoStdChronoDuration<std::chrono::nanoseconds>, StdChronoNanoSeconds)
rOOPS_CreateNamedTypeInfoObject(std::chrono::microseconds, rOops::rPropTypeInfoStdChronoDuration<std::chrono::microseconds>, StdChronoMicroseconds)
rOOPS_CreateNamedTypeInfoObject(std::chrono::milliseconds, rOops::rPropTypeInfoStdChronoDuration<std::chrono::milliseconds>, StdChronoMilliseconds)
rOOPS_CreateNamedTypeInfoObject(std::chrono::seconds, rOops::rPropTypeInfoStdChronoDuration<std::chrono::seconds>, StdChronoSeconds)
rOOPS_CreateNamedTypeInfoObject(std::chrono::minutes, rOops::rPropTypeInfoStdChronoDuration<std::chrono::minutes>, StdChronoMinutes)
rOOPS_CreateNamedTypeInfoObject(std::chrono::hours, rOops::rPropTypeInfoStdChronoDuration<std::chrono::hours>, StdChronoHours)
// Chrono timepoints:
rOOPS_CreateNamedTypeInfoObject(std::chrono::system_clock::time_point, rOops::rPropTypeInfoStdChronoSystemClock<std::chrono::system_clock::time_point>, StdChronoSystemClockTimePoint)
rOOPS_CreateNamedTypeInfoObject(std::chrono::steady_clock::time_point, rOops::rPropTypeInfoStdChronoTimePoint<std::chrono::steady_clock::time_point>, StdChronoSteadyClockTimePoint)

// Add alias names for built-in types.
namespace {
    int initPropTypeInfo() noexcept
    {
        rPropGetTypeInfo(static_cast<short*>(nullptr))->addAlias("short");
        rPropGetTypeInfo(static_cast<int*>(nullptr))->addAlias("int");
        #if ! defined(_MSC_VER)
        rPropGetTypeInfo(static_cast<long*>(nullptr))->addAlias("long");
        #endif
        #if ! defined(__GNUC__)
        rPropGetTypeInfo(static_cast<long long*>(nullptr))->addAlias("long long");
        #endif
        rPropGetTypeInfo(static_cast<unsigned short*>(nullptr))->addAlias("unsigned short");
        rPropGetTypeInfo(static_cast<unsigned*>(nullptr))->addAlias("unsigned");
        rPropGetTypeInfo(static_cast<unsigned int*>(nullptr))->addAlias("unsigned int");
        #if ! defined(_MSC_VER)
        rPropGetTypeInfo(static_cast<unsigned long*>(nullptr))->addAlias("unsigned long");
        #endif
        #if ! defined(__GNUC__)
        rPropGetTypeInfo(static_cast<unsigned long long*>(nullptr))->addAlias("unsigned long long");
        #endif
        return 1;
    }
    int rPropTypeInfoInitialized = initPropTypeInfo();
}
