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

/*
The following macros are taken from BETTER_ENUM:
    http://aantron.github.io/better-enums
    https://github.com/aantron/better-enums

Copyright (c) 2012-2020, Anton Bachin
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define BETTER_ENUMS_PP_MAP(macro, data, ...)   \
    BETTER_ENUMS_ID(                            \
        BETTER_ENUMS_APPLY(                     \
            BETTER_ENUMS_PP_MAP_VAR_COUNT,      \
            BETTER_ENUMS_PP_COUNT(__VA_ARGS__)) \
        (macro, data, __VA_ARGS__))

#define BETTER_ENUMS_PP_MAP_VAR_COUNT(count) BETTER_ENUMS_M ## count

#define BETTER_ENUMS_APPLY(macro, ...) BETTER_ENUMS_ID(macro(__VA_ARGS__))

#define BETTER_ENUMS_ID(x) x

#define BETTER_ENUMS_M1(m, d, x) m(d,0,x)
#define BETTER_ENUMS_M2(m,d,x,...) m(d,1,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M1(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M3(m,d,x,...) m(d,2,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M2(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M4(m,d,x,...) m(d,3,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M3(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M5(m,d,x,...) m(d,4,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M4(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M6(m,d,x,...) m(d,5,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M5(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M7(m,d,x,...) m(d,6,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M6(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M8(m,d,x,...) m(d,7,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M7(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M9(m,d,x,...) m(d,8,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M8(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M10(m,d,x,...) m(d,9,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M9(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M11(m,d,x,...) m(d,10,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M10(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M12(m,d,x,...) m(d,11,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M11(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M13(m,d,x,...) m(d,12,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M12(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M14(m,d,x,...) m(d,13,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M13(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M15(m,d,x,...) m(d,14,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M14(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M16(m,d,x,...) m(d,15,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M15(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M17(m,d,x,...) m(d,16,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M16(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M18(m,d,x,...) m(d,17,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M17(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M19(m,d,x,...) m(d,18,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M18(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M20(m,d,x,...) m(d,19,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M19(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M21(m,d,x,...) m(d,20,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M20(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M22(m,d,x,...) m(d,21,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M21(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M23(m,d,x,...) m(d,22,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M22(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M24(m,d,x,...) m(d,23,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M23(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M25(m,d,x,...) m(d,24,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M24(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M26(m,d,x,...) m(d,25,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M25(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M27(m,d,x,...) m(d,26,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M26(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M28(m,d,x,...) m(d,27,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M27(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M29(m,d,x,...) m(d,28,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M28(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M30(m,d,x,...) m(d,29,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M29(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M31(m,d,x,...) m(d,30,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M30(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M32(m,d,x,...) m(d,31,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M31(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M33(m,d,x,...) m(d,32,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M32(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M34(m,d,x,...) m(d,33,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M33(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M35(m,d,x,...) m(d,34,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M34(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M36(m,d,x,...) m(d,35,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M35(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M37(m,d,x,...) m(d,36,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M36(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M38(m,d,x,...) m(d,37,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M37(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M39(m,d,x,...) m(d,38,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M38(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M40(m,d,x,...) m(d,39,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M39(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M41(m,d,x,...) m(d,40,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M40(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M42(m,d,x,...) m(d,41,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M41(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M43(m,d,x,...) m(d,42,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M42(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M44(m,d,x,...) m(d,43,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M43(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M45(m,d,x,...) m(d,44,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M44(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M46(m,d,x,...) m(d,45,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M45(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M47(m,d,x,...) m(d,46,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M46(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M48(m,d,x,...) m(d,47,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M47(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M49(m,d,x,...) m(d,48,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M48(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M50(m,d,x,...) m(d,49,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M49(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M51(m,d,x,...) m(d,50,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M50(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M52(m,d,x,...) m(d,51,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M51(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M53(m,d,x,...) m(d,52,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M52(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M54(m,d,x,...) m(d,53,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M53(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M55(m,d,x,...) m(d,54,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M54(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M56(m,d,x,...) m(d,55,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M55(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M57(m,d,x,...) m(d,56,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M56(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M58(m,d,x,...) m(d,57,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M57(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M59(m,d,x,...) m(d,58,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M58(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M60(m,d,x,...) m(d,59,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M59(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M61(m,d,x,...) m(d,60,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M60(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M62(m,d,x,...) m(d,61,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M61(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M63(m,d,x,...) m(d,62,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M62(m,d,__VA_ARGS__))
#define BETTER_ENUMS_M64(m,d,x,...) m(d,63,x) \
    BETTER_ENUMS_ID(BETTER_ENUMS_M63(m,d,__VA_ARGS__))

#define BETTER_ENUMS_PP_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, \
    _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, \
    _56, _57, _58, _59, _60, _61, _62, _63, _64, count, ...) count

#define BETTER_ENUMS_PP_COUNT(...) \
    BETTER_ENUMS_ID(BETTER_ENUMS_PP_COUNT_IMPL(__VA_ARGS__, 64, 63, 62, 61, 60,\
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,\
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,\
        23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, \
        4, 3, 2, 1))

#define BETTER_ENUMS_STRINGIZE_SINGLE(ignored, index, expression)  #expression,

#define BETTER_ENUMS_STRINGIZE(...)                                            \
    BETTER_ENUMS_ID(                                                           \
        BETTER_ENUMS_PP_MAP(                                                   \
            BETTER_ENUMS_STRINGIZE_SINGLE, ignored, __VA_ARGS__))

//! Get intrinsic value of an (Enum::value) by taking advantage of
// C-conversion's parentheses priority
template <typename EnumType>
struct Enum2StrEeatAssign {
    explicit constexpr Enum2StrEeatAssign(EnumType value) : _value(value)
    { }

    template <typename Any>
    constexpr const Enum2StrEeatAssign&
    operator =(Any) const { return *this; }

    constexpr operator EnumType () const { return _value; }

private:
    EnumType    _value;
};

#define BETTER_ENUMS_EAT_ASSIGN_SINGLE(EnumType, index, expression)            \
    (EnumType)((::Enum2StrEeatAssign<EnumType>)EnumType::expression),

#define BETTER_ENUMS_EAT_ASSIGN(EnumType, ...)                                 \
    BETTER_ENUMS_ID(                                                           \
        BETTER_ENUMS_PP_MAP(                                                   \
            BETTER_ENUMS_EAT_ASSIGN_SINGLE, EnumType, __VA_ARGS__))
