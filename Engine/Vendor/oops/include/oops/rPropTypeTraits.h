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
Type traits and helper templates.
*/

#include <oops/rPropTypeInfo.h>
#include <memory>
#if __cplusplus > 201402L
#include <optional>
#endif
#include <string>
#include <type_traits>
#include <cstring>

namespace rOops
{
    template < typename BaseT >
    BaseT string_to(const std::string& /*arStr*/, size_t* apIdx=nullptr, int /*aBase*/=10)
    {
        if (apIdx) *apIdx = 0;
        return 0;
    }
    // int types
    template <> inline short string_to(const std::string& arStr, size_t *apIdx, int aBase) { return static_cast<short>(std::stoi(arStr, apIdx, aBase)); }
    template <> inline int string_to(const std::string& arStr, size_t *apIdx, int aBase) { return std::stoi(arStr, apIdx, aBase); }
    template <> inline long string_to(const std::string& arStr, size_t *apIdx, int aBase) { return std::stol(arStr, apIdx, aBase); }
    template <> inline long long string_to(const std::string& arStr, size_t *apIdx, int aBase) { return std::stoll(arStr, apIdx, aBase); }
    // unsigned types
    template <> inline unsigned short string_to(const std::string& arStr, size_t *apIdx, int aBase) { return static_cast<unsigned short>(std::stoi(arStr, apIdx, aBase)); }
    template <> inline unsigned int string_to(const std::string& arStr, size_t *apIdx, int aBase) { return static_cast<unsigned int>(std::stoul(arStr, apIdx, aBase)); }
    template <> inline unsigned long string_to(const std::string& arStr, size_t *apIdx, int aBase) { return std::stoul(arStr, apIdx, aBase); }
    template <> inline unsigned long long string_to(const std::string& arStr, size_t *apIdx, int aBase) { return std::stoull(arStr, apIdx, aBase); }
    // float types
    template <> inline float string_to(const std::string& arStr, size_t *apIdx, int) { return std::stof(arStr, apIdx); }
    template <> inline double string_to(const std::string& arStr, size_t *apIdx, int) { return std::stod(arStr, apIdx); }
    template <> inline long double string_to(const std::string& arStr, size_t *apIdx, int) { return std::stold(arStr, apIdx); }


    template <typename T> struct is_UniquePtr : std::false_type {};
    template <typename T> struct is_UniquePtr<std::unique_ptr<T>> : std::true_type {};
    template <typename T> struct is_UniquePtr<std::unique_ptr<T const>> : std::true_type {};

    template <typename T> struct is_SharedPtr : std::false_type {};
    template <typename T> struct is_SharedPtr<std::shared_ptr<T>> : std::true_type {};
    template <typename T> struct is_SharedPtr<std::shared_ptr<T const>> : std::true_type {};

    template <typename T> struct is_Optional : std::false_type {};
    #if __cplusplus > 201402L
    template <typename T> struct is_Optional<std::optional<T>> : std::true_type {};
    #endif

    template <typename T>
    constexpr bool isPointerType()
    {
        return (std::is_pointer<T>::value || is_UniquePtr<T>::value || is_SharedPtr<T>::value);
    }

    template <typename T> struct rPropRemovePointer{ typedef T type; };
    template <typename T> struct rPropRemovePointer<T*> { typedef T type; };
    template <typename T> struct rPropRemovePointer<const T*> { typedef T type; };
    template <typename T> struct rPropRemovePointer<std::unique_ptr<T>> { typedef T type; };
    template <typename T> struct rPropRemovePointer<std::shared_ptr<T>> { typedef T type; };
    template <typename T> struct rPropRemovePointer<std::unique_ptr<T const>> { typedef T type; };
    template <typename T> struct rPropRemovePointer<std::shared_ptr<T const>> { typedef T type; };
    #if __cplusplus > 201402L
    template <typename T> struct rPropRemovePointer<std::optional<T>> { typedef T type; };
    template <typename T> struct rPropRemovePointer<std::optional<T const>> { typedef T type; };
    #endif

    template <typename ContainerT>
    typename std::enable_if<std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    pushBackHelper(ContainerT& arCntnr, typename ContainerT::value_type& arValue)
    {
        arCntnr.push_back(std::move(arValue));
    }

    template <typename ContainerT>
    typename std::enable_if<!std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    pushBackHelper(ContainerT& arCntnr, typename ContainerT::value_type& arValue)
    {
        arCntnr.push_back(arValue);
    }


    template <typename ContainerT>
    typename std::enable_if<std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    insertHelper(ContainerT& arCntnr, typename ContainerT::value_type& arValue)
    {
        arCntnr.insert(std::move(arValue));
    }

    template <typename ContainerT>
    typename std::enable_if<!std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    insertHelper(ContainerT& arCntnr, typename ContainerT::value_type& arValue)
    {
        arCntnr.insert(arValue);
    }


    template <typename ContainerT>
    typename std::enable_if<std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    assignAtHelper(ContainerT& arCntnr, std::size_t aIdx, typename ContainerT::value_type& arValue)
    {
        arCntnr.at(aIdx) = std::move(arValue);
    }

    template <typename ContainerT>
    typename std::enable_if<!std::is_move_assignable<typename ContainerT::value_type>::value, void>::type
    assignAtHelper(ContainerT& arCntnr, std::size_t aIdx, typename ContainerT::value_type& arValue)
    {
        arCntnr.at(aIdx) = arValue;
    }


    template < typename T >
    typename std::enable_if<std::is_pointer<T>::value, void>::type
    assignHelper( T& arVal, void* apVal )
    {
        arVal = static_cast<T>(apVal);
    }

    template < typename T >
    typename std::enable_if<is_UniquePtr<T>::value, void>::type
    assignHelper( T& arVal, void* apVal )
    {
        arVal.reset( static_cast<typename rPropRemovePointer<T>::type*>(apVal) );
    }

    template < typename T >
    typename std::enable_if<is_SharedPtr<T>::value, void>::type
    assignHelper( T& arVal, void* apVal )
    {
        arVal.reset( static_cast<typename rPropRemovePointer<T>::type*>(apVal) );
    }

    template < typename T >
    typename std::enable_if<!std::is_pointer<T>::value && !is_UniquePtr<T>::value && !is_SharedPtr<T>::value && !std::is_copy_assignable<T>::value, void>::type
    assignHelper( T& arVal, void* apVal )
    {
        arVal = std::move(*static_cast<T*>(apVal));
    }

    template < typename T >
    typename std::enable_if<!std::is_pointer<T>::value && !is_UniquePtr<T>::value && !is_SharedPtr<T>::value && std::is_copy_assignable<T>::value, void>::type
    assignHelper( T& arVal, void* apVal )
    {
        arVal = *static_cast<T*>(apVal);
    }

    /// Set and MultiSet insert() functions are different.
    /// Get the iterator from the MultiSet result, which is the iterator itself.
    template < typename IterT, typename ResultT >
    typename std::enable_if<std::is_same<IterT, ResultT>::value, IterT>::type
    getIteratorFromResult(ResultT& arResult)
    {
        return arResult;
    }

    /// Set and MultiSet insert() functions are different.
    /// Get the iterator from the Set result, which is a pair, and the iterator is stored in first.
    template < typename IterT, typename ResultT >
    typename std::enable_if<!std::is_same<IterT, ResultT>::value, IterT>::type
    getIteratorFromResult(ResultT& arResult)
    {
        return arResult.first;
    }


    template<class T>
    inline auto getTypeName()
    {
        #if defined(__clang__)
            static constexpr char   prefix[] = "auto rOops::getTypeName() [T = ";
            static constexpr char  postfix[] = "]";
            #define OOPS_DECORATED_NAME __PRETTY_FUNCTION__
        #elif defined(__GNUC__)
            static constexpr char   prefix[] = "auto rOops::getTypeName() [with T = ";
            static constexpr char  postfix[] = "]";
            #define OOPS_DECORATED_NAME __PRETTY_FUNCTION__
        #elif defined(_MSC_VER)
            static constexpr char   prefix[] = "auto __cdecl rOops::getTypeName<";
            static constexpr char  postfix[] = ">(void)";
            #define OOPS_DECORATED_NAME __FUNCSIG__
        #else
            #error Unknown compiler.
        #endif

        static_assert(sizeof(OOPS_DECORATED_NAME) >= (sizeof(prefix) + sizeof(postfix) - 1), "wrong prefix provided by the compiler");
        if (0 != memcmp(OOPS_DECORATED_NAME, prefix, sizeof(prefix) - 1)) {
            throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE(OOPS_DECORATED_NAME << " != " << prefix << ": wrong prefix provided by the compiler"));
        }
        std::string type_name(OOPS_DECORATED_NAME + sizeof(prefix) - 1, sizeof(OOPS_DECORATED_NAME) - sizeof(prefix) - sizeof(postfix) + 1);
        #undef OOPS_DECORATED_NAME
        // MSC uses a 'struct', 'class', or 'enum' prefix.
        auto remove_substr = [&type_name](const char* apSubStr) {
            std::size_t pos = 0;
            auto nrOfCharsToErase = strlen(apSubStr);
            while (std::string::npos != (pos = type_name.find(apSubStr))) {
                type_name.erase(pos, nrOfCharsToErase);
            }
        };
        remove_substr("struct ");
        remove_substr("class ");
        remove_substr("enum ");
        return type_name;
    }

}

