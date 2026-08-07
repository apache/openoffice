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

// Donor types for RTTI synthesis.  Their type_info objects are emitted by the
// compiler, so they carry the real libc++abi vtables and the platform's own
// uniqueness convention.  They must stay ordinary namespace-scope classes with
// no virtual functions and a single public non-virtual base -- exactly the
// shape of a generated UNO exception -- so that typeid(RttiDonorDerived) is a
// __si_class_type_info and typeid(RttiDonorBase) a __class_type_info.
// Do not move them into an anonymous namespace.
struct RttiDonorBase { sal_Int32 dummy; };
struct RttiDonorDerived : public RttiDonorBase { sal_Int32 dummy2; };

// Itanium ABI object layouts (http://itanium-cxx-abi.github.io/cxx-abi/abi.html#rtti).
// libc++abi does not publish __cxxabiv1::__class_type_info, and declaring a
// look-alike class is not an option: it would get its own vtable, and
// __class_type_info::can_catch() dynamic_casts the thrown type to the real
// libc++abi class, so no typed handler would ever match.  We therefore build
// raw storage in the ABI layout and install a borrowed, genuine vtable.
struct RttiClassLayout   { void const * pVtable; sal_uIntPtr nName; };
struct RttiSiClassLayout { void const * pVtable; sal_uIntPtr nName; void const * pBase; };

extern "C" void *__cxa_allocate_exception(
    std::size_t thrown_size ) throw();
extern "C" void __cxa_free_exception( void *thrown_exception ) throw();
extern "C" void __cxa_throw (
    void *thrown_exception, std::type_info *tinfo, void (*dest) (void *) ) __attribute__((noreturn));
extern "C" std::type_info *__cxa_current_exception_type();

//==================================================================================================
void raiseException(
    uno_Any * pUnoExc, uno_Mapping * pUno2Cpp );
//==================================================================================================
void fillUnoException(
    std::type_info const & type, void * exception, uno_Any *,
    uno_Mapping * pCpp2Uno );
}
