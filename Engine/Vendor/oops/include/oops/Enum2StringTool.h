#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 2021:
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
Helper class for enum to string and string to enum conversion.
*/

#include <oops/Enum2StringMacros.h>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <string>

template < typename EnumT, typename EnumToolT >
class Enum2StringTool
{
public:
    using underlying_type = typename std::underlying_type<EnumT>::type;
    using iterator_type = typename std::map<EnumT, std::string>::const_iterator;
    using reverse_iterator_type = typename std::map<EnumT, std::string>::const_reverse_iterator;
    /// Convert enum value to string.
    static const std::string& to_string(EnumT aEnum) { return instance().toString(aEnum);  }
    std::string operator()(EnumT aEnum ) const { return instance().toString(aEnum); }
    /// Convert string to the enum value.
    static EnumT from_string(const std::string& arStr) { return instance().fromString(arStr); }
    EnumT operator()( const std::string& arStr ) const  { return instance().fromString(arStr); }
    /// Get the integral value of the given enum.
    static underlying_type to_integral(EnumT aEnum) { return static_cast<underlying_type>(aEnum);  }
    /// Convert an integral value to appropriate enum value.
    /// Throw std::runtime_error if there is no appropriate enum value.
    static EnumT from_integral(underlying_type  aEnumValue)
    {
        auto enum_val = static_cast<EnumT>(aEnumValue);
        if (instance()._enum2str.end() == instance()._enum2str.find(enum_val)) {
            throw std::runtime_error(std::string ("Invalid integral value for enum: ")+std::to_string(aEnumValue));
        }
        return enum_val;
    }
    /// Returns the number of enum values. E.g. for enum {A=20, B=40} size() returns 2.
    static std::size_t size() { return instance()._enum2str.size(); }
    /// Returns the smallest enum value, generally the first one.
    /// But for enum {A=10, B=2} first and smallest are different.
    static EnumT first() { return instance()._enum2str.begin()->first; }
    /// Returns the largest enum value, generally the last one.
    /// But for enum {A=10, B=2} last and largest are different.
    static EnumT last() { return instance()._enum2str.rbegin()->first; }
    /// Return an iterator pointing to a pair of enum value and its string representation.
    /// std::pair<EnumT,st::string>
    static iterator_type begin() { return instance()._enum2str.begin(); }
    static iterator_type end() { return instance()._enum2str.end(); }
    /// Return a reverse iterator pointing to a pair of enum value and its string representation.
    /// std::pair<EnumT,st::string>
    static reverse_iterator_type rbegin() { return instance()._enum2str.rbegin(); }
    static reverse_iterator_type rend() { return instance()._enum2str.rend(); }
protected:
    /// Insert a new enum value and its string representation.
    /// It is designed to call from the constructor of the descendant class.
    void insert( EnumT aID, const std::string& arName )
    {
        auto equal_sign_position = arName.find_first_of(" =");
        if (std::string::npos == equal_sign_position) {
            _enum2str.emplace(aID, arName);
            _str2enum.emplace(arName, aID);
        }
        else {
            auto name = arName.substr(0, equal_sign_position);
            _enum2str.emplace(aID, name);
            _str2enum.emplace(name, aID);
        }
    }
    /// Add an alias name to an enum value already inserted to the map.
    /// It is designed to call from the constructor of the descendant class.
    void alias( EnumT aID, const std::string& arName )
    {
        if (_enum2str.end() != _enum2str.find(aID)) {
            // add alias only to the string to enum conversion.
            _str2enum.emplace(arName, aID);
        }
        else {
            // Not inserted yet.
            insert(aID, arName);
        }
    }
    /// Internal implementation. Called from the static to_string() function.
    inline const std::string& toString(EnumT aEnum ) const
        {
            auto it = _enum2str.find(aEnum);
            if (_enum2str.end() == it) {
                throw std::runtime_error(std::string ("Unknown enum value: ")+std::to_string(static_cast<typename std::underlying_type<EnumT>::type>(aEnum)));
            }
            return it->second;
        }
    /// Internal implementation. Called from the static from_string() function.
    inline EnumT fromString( const std::string& arStr ) const
        {
            auto it = _str2enum.find(arStr);
            if (_str2enum.end() == it) {
                throw std::runtime_error("Cannot convert '" + arStr + "' to enum.");
            }
            return it->second;
        }
protected:
    std::map<EnumT, std::string> _enum2str; /// Map for enum to string conversion and for iterating over the enum values.
    std::unordered_map<std::string, EnumT> _str2enum; /// Map for string to enum conversion.
private:
    /// Singleton implementation for the Enum2StringTool object.
    /// Only used inside the public member functions.
    static const EnumToolT& instance()
    {
        // This is thread safe since C++11.
        static EnumToolT enum2str;
        return enum2str;
    }
}; //Enum2StringTool


/**
 * Declare enum class and generate the Enum2String class for the enum.
 */
#define DECLARE_ENUM_CLASS(EnumName, UnderlyingType, ...)                                \
    enum class EnumName : UnderlyingType { __VA_ARGS__ };                                \
    class EnumName##2String : public Enum2StringTool<EnumName,EnumName##2String>         \
    {                                                                                    \
    public:                                                                              \
        EnumName##2String() : Enum2StringTool<EnumName, EnumName##2String>() {           \
            constexpr const char*_the_raw_names[] =                                      \
                { BETTER_ENUMS_ID(BETTER_ENUMS_STRINGIZE(__VA_ARGS__)) };                \
            constexpr const EnumName _value_array[] =                                    \
                { BETTER_ENUMS_ID(BETTER_ENUMS_EAT_ASSIGN(EnumName, __VA_ARGS__)) };     \
            for (std::size_t idx=0; idx<sizeof(_value_array)/sizeof(EnumName); ++idx) {  \
                insert(_value_array[idx], _the_raw_names[idx]);                          \
            }                                                                            \
        }                                                                                \
    }
