/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "uno/mapping.h"

#include <typeinfo>
#include <exception>
#include <cstddef>

namespace CPPU_CURRENT_NAMESPACE
{

void dummy_can_throw_anything( char const * );

typedef unsigned _Unwind_Ptr __attribute__((__mode__(__pointer__)));

// ----- the following structure is compatible with the one declared in libunwind's unwind.h
// (use forced types)

struct _Unwind_Exception
{
    uint64_t exception_class;
    void * exception_cleanup;
    uintptr_t private_1;
    uintptr_t private_2;
};

struct __cxa_exception
{
    /* From LLVM 10 - Added reserved member at top of struct. Who the hell does that?
       https://reviews.llvm.org/rG674ec1eb16678b8addc02a4b0534ab383d22fa77
       NOTE: Apple clang version != real LLVM version. Don't be fooled!
    */
#if 0
    void *reserved;
#endif
    size_t referenceCount;
    ::std::type_info *exceptionType;
    void (*exceptionDestructor)(void *); 
    ::std::unexpected_handler unexpectedHandler;
    ::std::terminate_handler terminateHandler;
    __cxa_exception *nextException;
    int handlerCount;
    int handlerSwitchValue;
    const unsigned char *actionRecord;
    const unsigned char *languageSpecificData;
    void *catchTemp;
    void *adjustedPtr;
    _Unwind_Exception unwindHeader;
};

extern "C" void *__cxa_allocate_exception(
    std::size_t thrown_size ) throw();
extern "C" void __cxa_throw (
    void *thrown_exception, std::type_info *tinfo, void (*dest) (void *) ) __attribute__((noreturn));

struct __cxa_eh_globals
{
    __cxa_exception *caughtExceptions;
    unsigned int uncaughtExceptions;
};
extern "C" __cxa_eh_globals *__cxa_get_globals () throw();

// -----

// on OSX 64bit the class_type_info classes are specified
// in http://refspecs.linuxbase.org/cxxabi-1.86.html#rtti but
// these details are not generally available in a public header
// of most development environments. So we define them here.
class __class_type_info : public std::type_info
{
public:
        explicit __class_type_info( const char* pRttiName)
        : std::type_info( pRttiName)
        {}
};

class __si_class_type_info : public __class_type_info
{
        const __class_type_info* mpBaseType;
public:
        explicit __si_class_type_info( const char* pRttiName, __class_type_info* pBaseType)
        : __class_type_info( pRttiName), mpBaseType( pBaseType)
        {}
};

//==================================================================================================
void raiseException(
    uno_Any * pUnoExc, uno_Mapping * pUno2Cpp );
//==================================================================================================
void fillUnoException(
    __cxa_exception * header, uno_Any *, uno_Mapping * pCpp2Uno );
}

