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
Save and load function templates.
*/

#include <oops/rOopsParserContext.h>

namespace rOops
{

    /**
     * Save an object in the given format.
     *
     * @tparam FormatT - Type of the Oops format classes (text, binary or yaml).
     * @tparam ObjectT - The type of the object to be saved.
     * @param arFormat - The format object to be used. The output stream shall be assigned to this object.
     * @param arObj - Constant reference to the object to be saved.
     * @param arObjName - Name of the object used as the name of the top-level object at the beginning of the stream.
     * @return - the TypeInfo object of ObjectT.
     */
    template <typename FormatT, typename ObjectT>
    typename std::enable_if<!is_UniquePtr<ObjectT>::value && !is_SharedPtr<ObjectT>::value, const rPropTypeInfo*>::type
    save(FormatT& arFormat, const ObjectT& arObj, const std::string& arObjName="")
    {
        const rPropTypeInfo* pTypeInfo = rPropGetTypeInfo(&arObj);
        arFormat.writeProtocolVersion();
        arFormat.setTopLevel(pTypeInfo, arObjName);
        pTypeInfo->save(arFormat, &arObj);
        arFormat.writeValueSeparator(true);
        arFormat.saveAllUnsaved(arFormat);
        arFormat.flush();
        return pTypeInfo;
    }

    /**
     * Load an object from the stream assigned to the parser.
     *
     * @tparam ParserT - type of the parser object
     * @tparam ObjectT - type of the top-level object on the stream
     * @param arParser - the parser object. The input stream shall be already assigned to the parser.
     * @param arObj - reference to the object for storing the content of the stream.
     * @return - the TypeInfo object of ObjectT.
     */
    template <typename ParserT, typename ObjectT>
    typename std::enable_if<!is_UniquePtr<ObjectT>::value && !is_SharedPtr<ObjectT>::value, const rPropTypeInfo*>::type
    load(ParserT& arParser, ObjectT& arObj)
    {
        try {
            const rPropTypeInfo* pTypeInfo = rPropGetTypeInfo(&arObj);
            rOopsParserContext ctx;
            arParser.parse(ctx);
            pTypeInfo->load(arParser, &arObj, ctx);
            arParser.parseTopLevelSeparator(ctx); // parse ';'.
            arParser.createUnloadedObjects(arParser, ctx);
            arParser.skipWhiteSpace();
            return pTypeInfo;
        } //try
        catch (const std::runtime_error& arEx)
        {
            throw std::runtime_error(arParser.where() + arEx.what());
        } //catch
        catch (...)
        {
            throw std::runtime_error(arParser.where() + "Unknown exception.");
        } //catch
    }

    /**
     * Save a polymorph object in the given format.
     *
     * @tparam FormatT - Type of the Oops format classes (text, binary or yaml).
     * @tparam PointerT - The base type of the pointer to be saved.
     * @param arFormat - The format object to be used. The output stream shall be assigned to this object.
     * @param arObjPtr - Constant reference to the shared of unique pointer referencing object to be saved.
     * @param arObjName - Name of the object used as the name of the top-level object at the beginning of the stream.
     * @return the TypeInfo object of the actual type of \c arObj.
     */
    template <typename FormatT, typename PointerT>
    typename std::enable_if<is_UniquePtr<PointerT>::value || is_SharedPtr<PointerT>::value, const rPropTypeInfo*>::type
    save(FormatT& arFormat, const PointerT& arObjPtr, const std::string& arObjName="")
    {
        const rPropTypeInfo* pTypeInfo = rPropGetTypeInfo(arObjPtr.get())->objTypeInfo(arObjPtr.get());
        arFormat.writeProtocolVersion();
        arFormat.setTopLevel(pTypeInfo, arObjName);
        pTypeInfo->save(arFormat, arObjPtr.get());
        arFormat.writeValueSeparator(true);
        arFormat.saveAllUnsaved(arFormat);
        arFormat.flush();
        return pTypeInfo;
    }

    /**
     * Load a polymorph object from the stream assigned to the parser.
     * The type of the actual object is read from the stream.
     *
     * The load() function reads the top-level object's type,
     * finds its TypeInfo object and creates a new object of that type.
     * The address of the new object is stored in the shared or unique pointer.
     *
     * @tparam ParserT - type of the parser object
     * @tparam PointerT - base type of the pointer for the top-level class hierarchy
     * @param arParser - the parser object. The input stream shall be already assigned to the parser.
     * @param arObjPtr - reference to a smart pointer for storing the content of the stream.
     * @return - the TypeInfo object of ObjectT.
     */
    template <typename ParserT, typename PointerT>
    typename std::enable_if<is_UniquePtr<PointerT>::value || is_SharedPtr<PointerT>::value, const rPropTypeInfo*>::type
    load(ParserT& arParser, PointerT& arObjPtr)
    {
        try {
            rOopsParserContext ctx;
            arParser.parse(ctx);
            if (ctx.typeName_.empty()) {
                throw std::runtime_error(rOOPS_BUILD_EXCEPTION_MESSAGE("Top level type name is missing."));
            }
            const rPropTypeInfo* pTypeInfo = rPropGetTypeInfoByName(ctx.typeName_);
            arObjPtr.reset(static_cast<typename PointerT::element_type*>(pTypeInfo->createObj()));
            pTypeInfo->load(arParser, arObjPtr.get(), ctx);
            arParser.parseTopLevelSeparator(ctx); // parse ';'.
            arParser.createUnloadedObjects(arParser, ctx);
            arParser.skipWhiteSpace();
            return pTypeInfo;
        } //try
        catch (const std::runtime_error& arEx)
        {
            throw std::runtime_error(arParser.where() + arEx.what());
        } //catch
        catch (...)
        {
            throw std::runtime_error(arParser.where() + "Unknown exception.");
        } //catch
    }

}
