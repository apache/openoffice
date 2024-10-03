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

#include <cxxabi.h>
#ifndef _GLIBCXX_CDTOR_CALLABI // new in GCC 4.7 cxxabi.h
#define _GLIBCXX_CDTOR_CALLABI
#endif
#include <cstddef>

#ifndef CONFIG_CXXABI_H
#define CONFIG_CXXABI_H

#define HAVE_CXXABI_H_BASE_CLASS_TYPE_INFO 0
#define HAVE_CXXABI_H_CLASS_TYPE_INFO 0
#define HAVE_CXXABI_H_CXA_ALLOCATE_EXCEPTION 0
#define HAVE_CXXABI_H_CXA_EH_GLOBALS 1
#define HAVE_CXXABI_H_CXA_EXCEPTION 1
#define HAVE_CXXABI_H_CXA_GET_GLOBALS 1
#define HAVE_CXXABI_H_CXA_CURRENT_EXCEPTION_TYPE 1
#define HAVE_CXXABI_H_CXA_THROW 0
#define HAVE_CXXABI_H_SI_CLASS_TYPE_INFO 0
#define HAVE_CXXABI_H_VMI_CLASS_TYPE_INFO 0

#endif

#if !HAVE_CXXABI_H_CLASS_TYPE_INFO
// <https://mentorembedded.github.io/cxx-abi/abi.html>,
// libstdc++-v3/libsupc++/cxxabi.h:
namespace __cxxabiv1 {
class __class_type_info: public std::type_info {
public:
    explicit __class_type_info(char const * n): type_info(n) {}
    ~__class_type_info();
};
}
#endif

#if !HAVE_CXXABI_H_SI_CLASS_TYPE_INFO
// <https://mentorembedded.github.io/cxx-abi/abi.html>,
// libstdc++-v3/libsupc++/cxxabi.h:
namespace __cxxabiv1 {
class __si_class_type_info: public __class_type_info {
public:
    __class_type_info const * __base_type;
    explicit __si_class_type_info(
        char const * n, __class_type_info const *base):
        __class_type_info(n), __base_type(base) {}
    ~__si_class_type_info();
};
}
#endif

namespace CPPU_CURRENT_NAMESPACE
{

void dummy_can_throw_anything( char const * );

// ----- following decl from libstdc++-v3/libsupc++/unwind-cxx.h and unwind.h

#ifdef __GLIBCXX__
struct _Unwind_Exception
{
    unsigned exception_class __attribute__((__mode__(__DI__)));
    void * exception_cleanup;
    unsigned private_1 __attribute__((__mode__(__word__)));
    unsigned private_2 __attribute__((__mode__(__word__)));
} __attribute__((__aligned__));

struct __cxa_exception
{ 
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
#endif /* __GLIBCXX__ */

extern "C" void *__cxa_allocate_exception(
    std::size_t thrown_size ) throw();
extern "C" void __cxa_throw (
    void *thrown_exception, std::type_info *tinfo, void (*dest) (void *) ) __attribute__((noreturn));

#ifdef __GLIBCXX__
struct __cxa_eh_globals
{
    __cxa_exception *caughtExceptions;
    unsigned int uncaughtExceptions;
};
extern "C" __cxa_eh_globals *__cxa_get_globals () throw();
#endif /* __GLIBCXX__ */

// -----

//==================================================================================================
void raiseException(
    uno_Any * pUnoExc, uno_Mapping * pUno2Cpp );
//==================================================================================================
#ifndef __GLIBCXX__
using __cxxabiv1:: __cxa_exception;
#endif /* __GLIBCXX__ */

void fillUnoException(
    __cxa_exception * header, uno_Any *, uno_Mapping * pCpp2Uno );
}

namespace ppc64
{
     enum ppclimits { MAX_GPR_REGS = 8, MAX_SSE_REGS = 13 };
    bool return_in_hidden_param( typelib_TypeDescriptionReference *pTypeRef );
}
