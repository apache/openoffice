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



#ifndef _BRIDGES_CPP_UNO_AARCH64_ABI_HXX_
#define _BRIDGES_CPP_UNO_AARCH64_ABI_HXX_

// This is an implementation of the AArch64 procedure call standard, as
// described in "Procedure Call Standard for the Arm 64-bit Architecture"
// (ARM IHI 0055), with the deviations documented in Apple's "Writing ARM64
// Code for Apple Platforms".  It is a clean-room implementation written from
// those public specifications; see ../../../../AAPCS64_BRIDGE_SPEC.md.

#include <typelib/typedescription.hxx>

namespace aarch64
{

/* 8 general purpose registers (x0..x7) are used for parameter passing.
   Note: the indirect-result-location register x8 is *separate* and is NOT
   part of this count. */
const sal_uInt32 MAX_GPR_REGS = 8;

/* 8 SIMD/FP registers (v0..v7) are used for parameter passing. */
const sal_uInt32 MAX_FPR_REGS = 8;

/* The largest number of registers a single aggregate can occupy: an HFA/HVA
   may use up to 4 FP registers; a non-HFA aggregate passed in GPRs uses at
   most 2 (16 bytes / 8). */
const sal_uInt32 MAX_AGGREGATE_REGS = 4;

enum ReturnKind
{
    RETURN_KIND_HFA_FLOAT = 0x100,
    RETURN_KIND_HFA_DOUBLE = 0x101
};

/* Count the number of registers required to pass the given type.

   Examines the argument and sets the number of GPR (x) and FPR (v) registers
   it would consume.  For a Homogeneous Floating-point Aggregate the FPR count
   is the number of members (<= 4); for a non-HFA aggregate <= 16 bytes the GPR
   count is 1 or 2; scalars use exactly one register of the appropriate bank.

   Returns false iff the parameter must be passed indirectly (in memory): a
   non-HFA aggregate larger than 16 bytes.  When bInReturn is true the same
   classification answers "can this be returned in registers?" (false => the
   caller must allocate a buffer and pass it in x8).
*/
bool examine_argument( typelib_TypeDescriptionReference *pTypeRef, bool bInReturn, int &nUsedGPR, int &nUsedFPR );

/** Does a function returning this type use the hidden indirect-result pointer
    (passed by the caller in x8), or can it return in registers?

    A scalar returns in x0 or v0; an HFA returns in v0..v3; a non-HFA aggregate
    of <= 16 bytes returns in x0,x1.  Anything larger (non-HFA aggregate
    > 16 bytes) is returned via the caller-allocated buffer addressed by x8 -
    that is the "hidden param" case, for which this returns true.
*/
bool return_in_hidden_param( typelib_TypeDescriptionReference *pTypeRef );

/** Return the assembly return kind for an HFA, or the type class otherwise. */
sal_uInt32 get_return_kind( typelib_TypeDescriptionReference *pTypeRef );

/** Scatter a register-resident return value (an HFA returned in v0..v3, or a
    non-HFA aggregate <= 16 bytes returned in x0,x1) into the caller's struct.

    pGPR points at the saved x0,x1,... ; pFPR at the saved v0,v1,... (each
    element the low 8 bytes of a v register, i.e. a double slot).  Only valid
    when return_in_hidden_param() is false.
*/
void fill_struct( typelib_TypeDescriptionReference *pTypeRef, const sal_uInt64* pGPR, const double* pFPR, void *pStruct );

sal_uInt32 align_stack_offset(
    sal_uInt32 offset, typelib_TypeDescriptionReference *pTypeRef );

sal_uInt32 stack_size( typelib_TypeDescriptionReference *pTypeRef );

} // namespace aarch64

#endif // _BRIDGES_CPP_UNO_AARCH64_ABI_HXX_
