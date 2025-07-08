#pragma once

/*****************************************************************************
 The MIT License (MIT)

 Object Oriented Property Stream Library
 Copyright (C) 1998-2019:
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

#include <map>
#include <unordered_map>
#include <string>

namespace rOops
{


template < typename EnumT >
class rPropEnumStringTool
{
public:
    std::string operator()(EnumT aEnum ) const
        {
            auto it = _enum2str.find(aEnum);
            if (_enum2str.end() == it) {
                return std::to_string(static_cast<typename std::underlying_type<EnumT>::type>(aEnum));
            }
            return it->second;
        }
    EnumT operator()( const std::string& arStr ) const
        {
            auto it = _str2enum.find(arStr);
            if (_str2enum.end() == it) throw std::runtime_error("Cannot convert '" + arStr + "' to enum.");
            return it->second;
        }
protected:
    void insert( EnumT aID, const std::string& arName )
    {
        _enum2str.emplace(aID, arName);
        _str2enum.emplace(arName, aID);
    }
    void alias( EnumT aID, const std::string& arName )
    {
        if (_enum2str.end() != _enum2str.find(aID)) {
            // add alias
            _str2enum.emplace(arName, aID);
        }
        else {
            // Not inserted yet.
            insert(aID, arName);
        }
    }
protected:
    std::map<EnumT, std::string> _enum2str;
    std::unordered_map<std::string, EnumT> _str2enum;
}; //rPropEnumStringTool


template < typename EnumT >
class rPropEnumStringIntConv
{
public:
    std::string operator()(EnumT aEnum ) const
        {
            int temp = aEnum;
            return std::to_string(temp);
        }
    EnumT operator()( const std::string& arStr ) const
        {
            int temp = std::stoi(arStr);
            return static_cast<EnumT>(temp);
        }
}; //class rPropEnumStringIntConv


} //namespace rOops
