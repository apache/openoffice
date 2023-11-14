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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

// This is an implementation of the x86-64 ABI as described in 'System V
// Application Binary Interface, AMD64 Architecture Processor Supplement'
// (http://www.x86-64.org/documentation/abi-0.95.pdf)
//
// The code in this file is a modification of src/x86/ffi64.c from libffi
// (http://sources.redhat.com/libffi/) which is under the following license:

/* -----------------------------------------------------------------------
   ffi.c - Copyright (c) 2002  Bo Thorsen <bo@suse.de>
   
   x86-64 Foreign Function Interface 

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL CYGNUS SOLUTIONS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------- */

#include "abi.hxx"

#include <rtl/ustring.hxx>

using namespace x86_64;

typedef struct
{
    /* Registers for argument passing.  */
    long gpr[MAX_GPR_REGS];
    __int128_t sse[MAX_SSE_REGS];

    /* Stack space for arguments.  */
    char argspace[0];
} stackLayout;

/* Register class used for passing given 64bit part of the argument.
   These represent classes as documented by the PS ABI, with the exception
   of SSESF, SSEDF classes, that are basically SSE class, just gcc will
   use SF or DFmode move instead of DImode to avoid reformatting penalties.

   Similary we play games with INTEGERSI_CLASS to use cheaper SImode moves
   whenever possible (upper half does contain padding).
 */
enum x86_64_reg_class
{
    X86_64_NO_CLASS,
    X86_64_INTEGER_CLASS,
    X86_64_INTEGERSI_CLASS,
    X86_64_SSE_CLASS,
    X86_64_SSESF_CLASS,
    X86_64_SSEDF_CLASS,
    X86_64_SSEUP_CLASS,
    X86_64_X87_CLASS,
    X86_64_X87UP_CLASS,
    X86_64_MEMORY_CLASS
};

#define MAX_CLASSES 4

/* x86-64 register passing implementation.  See x86-64 ABI for details.  Goal
   of this code is to classify each 8bytes of incoming argument by the register
   class and assign registers accordingly.  */

/* Return the union class of CLASS1 and CLASS2.
   See the x86-64 PS ABI for details.  */

static enum x86_64_reg_class
merge_classes (enum x86_64_reg_class class1, enum x86_64_reg_class class2)
{
    /* Rule #1: If both classes are equal, this is the resulting class.  */
    if (class1 == class2)
        return class1;

    /* Rule #2: If one of the classes is NO_CLASS, the resulting class is
       the other class.  */
    if (class1 == X86_64_NO_CLASS)
        return class2;
    if (class2 == X86_64_NO_CLASS)
        return class1;

    /* Rule #3: If one of the classes is MEMORY, the result is MEMORY.  */
    if (class1 == X86_64_MEMORY_CLASS || class2 == X86_64_MEMORY_CLASS)
        return X86_64_MEMORY_CLASS;

    /* Rule #4: If one of the classes is INTEGER, the result is INTEGER.  */
    if ((class1 == X86_64_INTEGERSI_CLASS && class2 == X86_64_SSESF_CLASS)
            || (class2 == X86_64_INTEGERSI_CLASS && class1 == X86_64_SSESF_CLASS))
        return X86_64_INTEGERSI_CLASS;
    if (class1 == X86_64_INTEGER_CLASS || class1 == X86_64_INTEGERSI_CLASS
            || class2 == X86_64_INTEGER_CLASS || class2 == X86_64_INTEGERSI_CLASS)
        return X86_64_INTEGER_CLASS;

    /* Rule #5: If one of the classes is X87 or X87UP class, MEMORY is used.  */
    if (class1 == X86_64_X87_CLASS || class1 == X86_64_X87UP_CLASS
            || class2 == X86_64_X87_CLASS || class2 == X86_64_X87UP_CLASS)
        return X86_64_MEMORY_CLASS;

    /* Rule #6: Otherwise class SSE is used.  */
    return X86_64_SSE_CLASS;
}

/* Classify the argument of type TYPE and mode MODE.
   CLASSES will be filled by the register class used to pass each word
   of the operand.  The number of words is returned.  In case the parameter
   should be passed in memory, 0 is returned. As a special case for zero
   sized containers, classes[0] will be NO_CLASS and 1 is returned.

   See the x86-64 PS ABI for details.
*/
static int
classify_argument( typelib_TypeDescriptionReference *pTypeRef, enum x86_64_reg_class classes[], int byteOffset )
{
    switch ( pTypeRef->eTypeClass )
    {
        case typelib_TypeClass_VOID:
            classes[0] = X86_64_NO_CLASS;
            return 1;
        case typelib_TypeClass_CHAR:
        case typelib_TypeClass_BOOLEAN:
        case typelib_TypeClass_BYTE:
        case typelib_TypeClass_SHORT:
        case typelib_TypeClass_UNSIGNED_SHORT:
        case typelib_TypeClass_LONG:
        case typelib_TypeClass_UNSIGNED_LONG:
        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:
        case typelib_TypeClass_ENUM:
            if ( ( byteOffset % 8 + pTypeRef->pType->nSize ) <= 4 )
                classes[0] = X86_64_INTEGERSI_CLASS;
            else
                classes[0] = X86_64_INTEGER_CLASS;
            return 1;
        case typelib_TypeClass_FLOAT:
            if ( ( byteOffset % 8 ) == 0 )
                classes[0] = X86_64_SSESF_CLASS;
            else
                classes[0] = X86_64_SSE_CLASS;
            return 1;
        case typelib_TypeClass_DOUBLE:
            classes[0] = X86_64_SSEDF_CLASS;
            return 1;
        /*case LONGDOUBLE:
            classes[0] = X86_64_X87_CLASS;
            classes[1] = X86_64_X87UP_CLASS;
            return 2;*/
        case typelib_TypeClass_STRING:
        case typelib_TypeClass_TYPE:
        case typelib_TypeClass_ANY:
        case typelib_TypeClass_TYPEDEF:
        case typelib_TypeClass_UNION:
        case typelib_TypeClass_SEQUENCE:
        case typelib_TypeClass_ARRAY:
        case typelib_TypeClass_INTERFACE:
            return 0;
        case typelib_TypeClass_STRUCT:
        case typelib_TypeClass_EXCEPTION:
            {
                typelib_TypeDescription * pTypeDescr = 0;
                TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );

                const int UNITS_PER_WORD = 8;
                int words = ( pTypeDescr->nSize + UNITS_PER_WORD - 1 ) / UNITS_PER_WORD;
                enum x86_64_reg_class subclasses[MAX_CLASSES];

                /* If the struct is larger than 16 bytes, pass it on the stack.  */
                if ( pTypeDescr->nSize > 16 )
                {
                    TYPELIB_DANGER_RELEASE( pTypeDescr );
                    return 0;
                }

                for ( int i = 0; i < words; i++ )
                    classes[i] = X86_64_NO_CLASS;

                const typelib_CompoundTypeDescription *pStruct = reinterpret_cast<const typelib_CompoundTypeDescription*>( pTypeDescr );

                /* Merge the fields of structure.  */
                for ( sal_Int32 nMember = 0; nMember < pStruct->nMembers; ++nMember )
                {
                    typelib_TypeDescriptionReference *pTypeInStruct = pStruct->ppTypeRefs[ nMember ];
                    int offset = byteOffset + pStruct->pMemberOffsets[ nMember ];

                    int num = classify_argument( pTypeInStruct, subclasses, offset );

                    if ( num == 0 )
                    {
                        TYPELIB_DANGER_RELEASE( pTypeDescr );
                        return 0;
                    }

                    for ( int i = 0; i < num; i++ )
                    {
                        int pos = offset / 8;
                        classes[i + pos] = merge_classes( subclasses[i], classes[i + pos] );
                    }
                }

                TYPELIB_DANGER_RELEASE( pTypeDescr );

                /* Final merger cleanup.  */
                for ( int i = 0; i < words; i++ )
                {
                    /* If one class is MEMORY, everything should be passed in
                       memory.  */
                    if ( classes[i] == X86_64_MEMORY_CLASS )
                        return 0;

                    /* The X86_64_SSEUP_CLASS should be always preceded by
                       X86_64_SSE_CLASS.  */
                    if ( classes[i] == X86_64_SSEUP_CLASS
                            && ( i == 0 || classes[i - 1] != X86_64_SSE_CLASS ) )
                        classes[i] = X86_64_SSE_CLASS;

                    /*  X86_64_X87UP_CLASS should be preceded by X86_64_X87_CLASS.  */
                    if ( classes[i] == X86_64_X87UP_CLASS
                            && ( i == 0 || classes[i - 1] != X86_64_X87_CLASS ) )
                        classes[i] = X86_64_SSE_CLASS;
                }
                return words;
            }

        default:
#if OSL_DEBUG_LEVEL > 1
            OSL_TRACE( "Unhandled case: pType->eTypeClass == %d\n", pTypeRef->eTypeClass );
#endif
            OSL_ASSERT(0);
    }
    return 0; /* Never reached.  */
}

/* Examine the argument and return set number of register required in each
   class.  Return 0 iff parameter should be passed in memory.  */
bool x86_64::examine_argument( typelib_TypeDescriptionReference *pTypeRef, bool bInReturn, int &nUsedGPR, int &nUsedSSE )
{
    enum x86_64_reg_class classes[MAX_CLASSES];
    int n;

    n = classify_argument( pTypeRef, classes, 0 );

    if ( n == 0 )
        return false;

    nUsedGPR = 0;
    nUsedSSE = 0;
    for ( n--; n >= 0; n-- )
        switch ( classes[n] )
        {
            case X86_64_INTEGER_CLASS:
            case X86_64_INTEGERSI_CLASS:
                nUsedGPR++;
                break;
            case X86_64_SSE_CLASS:
            case X86_64_SSESF_CLASS:
            case X86_64_SSEDF_CLASS:
                nUsedSSE++;
                break;
            case X86_64_NO_CLASS:
            case X86_64_SSEUP_CLASS:
                break;
            case X86_64_X87_CLASS:
            case X86_64_X87UP_CLASS:
                if ( !bInReturn )
                    return false;
                break;
            default:
#if OSL_DEBUG_LEVEL > 1
            OSL_TRACE( "Unhandled case: classes[n] == %d\n", classes[n] );
#endif
            OSL_ASSERT(0);
        }
    return true;
}

bool x86_64::return_in_hidden_param( typelib_TypeDescriptionReference *pTypeRef )
{
    int g, s;

    return examine_argument( pTypeRef, true, g, s ) == 0;
}

void x86_64::fill_struct( typelib_TypeDescriptionReference *pTypeRef, const sal_uInt64 *pGPR, const double *pSSE, void *pStruct )
{
    enum x86_64_reg_class classes[MAX_CLASSES];
    int n;

    n = classify_argument( pTypeRef, classes, 0 );

    sal_uInt64 *pStructAlign = reinterpret_cast<sal_uInt64 *>( pStruct );
    for ( n--; n >= 0; n-- )
        switch ( classes[n] )
        {
            case X86_64_INTEGER_CLASS:
            case X86_64_INTEGERSI_CLASS:
                *pStructAlign++ = *pGPR++;
                break;
            case X86_64_SSE_CLASS:
            case X86_64_SSESF_CLASS:
            case X86_64_SSEDF_CLASS:
                *pStructAlign++ = *reinterpret_cast<const sal_uInt64 *>( pSSE++ );
                break;
            default:
                break;
        }
}

#if 0

/* Functions to load floats and double to an SSE register placeholder.  */
extern void float2sse (float, __int128_t *);
extern void double2sse (double, __int128_t *);
extern void floatfloat2sse (void *, __int128_t *);

/* Functions to put the floats and doubles back.  */
extern float sse2float (__int128_t *);
extern double sse2double (__int128_t *);
extern void sse2floatfloat(__int128_t *, void *);

/*@-exportheader@*/
void
ffi_prep_args (stackLayout *stack, extended_cif *ecif)
/*@=exportheader@*/
{
  int gprcount, ssecount, i, g, s;
  void **p_argv;
  void *argp = &stack->argspace;
  ffi_type **p_arg;

  /* First check if the return value should be passed in memory. If so,
     pass the pointer as the first argument.  */
  gprcount = ssecount = 0;
  if (ecif->cif->rtype->type != FFI_TYPE_VOID 
      && examine_argument (ecif->cif->rtype, 1, &g, &s) == 0)
    (void *)stack->gpr[gprcount++] = ecif->rvalue;

  for (i=ecif->cif->nargs, p_arg=ecif->cif->arg_types, p_argv = ecif->avalue;
       i!=0; i--, p_arg++, p_argv++)
    {
      int in_register = 0;

      switch ((*p_arg)->type)
	{
	case FFI_TYPE_SINT8:
	case FFI_TYPE_SINT16:
	case FFI_TYPE_SINT32:
	case FFI_TYPE_SINT64:
	case FFI_TYPE_UINT8:
	case FFI_TYPE_UINT16:
	case FFI_TYPE_UINT32:
	case FFI_TYPE_UINT64:
	case FFI_TYPE_POINTER:
	  if (gprcount < MAX_GPR_REGS)
	    {
	      stack->gpr[gprcount] = 0;
	      stack->gpr[gprcount++] = *(long long *)(*p_argv);
	      in_register = 1;
	    }
	  break;

	case FFI_TYPE_FLOAT:
	  if (ssecount < MAX_SSE_REGS)
	    {
	      float2sse (*(float *)(*p_argv), &stack->sse[ssecount++]);
	      in_register = 1;
	    }
	  break;

	case FFI_TYPE_DOUBLE:
	  if (ssecount < MAX_SSE_REGS)
	    {
	      double2sse (*(double *)(*p_argv), &stack->sse[ssecount++]);
	      in_register = 1;
	    }
	  break;
	}

      if (in_register)
	continue;

      /* Either all places in registers where filled, or this is a
	 type that potentially goes into a memory slot.  */
      if (examine_argument (*p_arg, 0, &g, &s) == 0
	  || gprcount + g > MAX_GPR_REGS || ssecount + s > MAX_SSE_REGS)
	{
	  /* Pass this argument in memory.  */
	  argp = (void *)ALIGN(argp, (*p_arg)->alignment);
	  memcpy (argp, *p_argv, (*p_arg)->size);
	  argp += (*p_arg)->size;
	}
      else
	{
	  /* All easy cases are eliminated. Now fire the big guns.  */

	  enum x86_64_reg_class classes[MAX_CLASSES];
	  int j, num;
	  void *a;

	  num = classify_argument (*p_arg, classes, 0);
	  for (j=0, a=*p_argv; j<num; j++, a+=8)
	    {
	      switch (classes[j])
		{
		case X86_64_INTEGER_CLASS:
		case X86_64_INTEGERSI_CLASS:
		  stack->gpr[gprcount++] = *(long long *)a;
		  break;
		case X86_64_SSE_CLASS:
		  floatfloat2sse (a, &stack->sse[ssecount++]);
		  break;
		case X86_64_SSESF_CLASS:
		  float2sse (*(float *)a, &stack->sse[ssecount++]);
		  break;
		case X86_64_SSEDF_CLASS:
		  double2sse (*(double *)a, &stack->sse[ssecount++]);
		  break;
		default:
		  abort();
		}
	    }
	}
    }
}

/* Perform machine dependent cif processing.  */
ffi_status
ffi_prep_cif_machdep (ffi_cif *cif)
{
  int gprcount, ssecount, i, g, s;

  gprcount = ssecount = 0;

  /* Reset the byte count. We handle this size estimation here.  */
  cif->bytes = 0;

  /* If the return value should be passed in memory, pass the pointer
     as the first argument. The actual memory isn't allocated here.  */
  if (cif->rtype->type != FFI_TYPE_VOID 
      && examine_argument (cif->rtype, 1, &g, &s) == 0)
    gprcount = 1;

  /* Go over all arguments and determine the way they should be passed.
     If it's in a register and there is space for it, let that be so. If
     not, add it's size to the stack byte count.  */
  for (i=0; i<cif->nargs; i++)
    {
      if (examine_argument (cif->arg_types[i], 0, &g, &s) == 0
	  || gprcount + g > MAX_GPR_REGS || ssecount + s > MAX_SSE_REGS)
	{
	  /* This is passed in memory. First align to the basic type.  */
	  cif->bytes = ALIGN(cif->bytes, cif->arg_types[i]->alignment);

	  /* Stack arguments are *always* at least 8 byte aligned.  */
	  cif->bytes = ALIGN(cif->bytes, 8);

	  /* Now add the size of this argument.  */
	  cif->bytes += cif->arg_types[i]->size;
	}
      else
	{
	  gprcount += g;
	  ssecount += s;
	}
    }

  /* Set the flag for the closures return.  */
    switch (cif->rtype->type)
    {
    case FFI_TYPE_VOID:
    case FFI_TYPE_STRUCT:
    case FFI_TYPE_SINT64:
    case FFI_TYPE_FLOAT:
    case FFI_TYPE_DOUBLE:
    case FFI_TYPE_LONGDOUBLE:
      cif->flags = (unsigned) cif->rtype->type;
      break;

    case FFI_TYPE_UINT64:
      cif->flags = FFI_TYPE_SINT64;
      break;

    default:
      cif->flags = FFI_TYPE_INT;
      break;
    }

  return FFI_OK;
}

typedef struct
{
  long gpr[2];
  __int128_t sse[2];
  long double st0;
} return_value;

//#endif

void
ffi_fill_return_value (return_value *rv, extended_cif *ecif)
{
    enum x86_64_reg_class classes[MAX_CLASSES];
    int i = 0, num;
    long *gpr = rv->gpr;
    __int128_t *sse = rv->sse;
    signed char sc;
    signed short ss;

    /* This is needed because of the way x86-64 handles signed short
       integers.  */
    switch (ecif->cif->rtype->type)
    {
        case FFI_TYPE_SINT8:
            sc = *(signed char *)gpr;
            *(long long *)ecif->rvalue = (long long)sc;
            return;
        case FFI_TYPE_SINT16:
            ss = *(signed short *)gpr;
            *(long long *)ecif->rvalue = (long long)ss;
            return;
        default:
            /* Just continue.  */
            ;
    }

    num = classify_argument (ecif->cif->rtype, classes, 0);

    if (num == 0)
        /* Return in memory.  */
        ecif->rvalue = (void *) rv->gpr[0];
    else if (num == 2 && classes[0] == X86_64_X87_CLASS &&
            classes[1] == X86_64_X87UP_CLASS)
        /* This is a long double (this is easiest to handle this way instead
           of an eightbyte at a time as in the loop below.  */
        *((long double *)ecif->rvalue) = rv->st0;
    else
    {
        void *a;

        for (i=0, a=ecif->rvalue; i<num; i++, a+=8)
        {
            switch (classes[i])
            {
                case X86_64_INTEGER_CLASS:
                case X86_64_INTEGERSI_CLASS:
                    *(long long *)a = *gpr;
                    gpr++;
                    break;
                case X86_64_SSE_CLASS:
                    sse2floatfloat (sse++, a);
                    break;
                case X86_64_SSESF_CLASS:
                    *(float *)a = sse2float (sse++);
                    break;
                case X86_64_SSEDF_CLASS:
                    *(double *)a = sse2double (sse++);
                    break;
                default:
                    abort();
            }
        }
    }
}

//#if 0

/*@-declundef@*/
/*@-exportheader@*/
extern void ffi_call_UNIX64(void (*)(stackLayout *, extended_cif *),
			    void (*) (return_value *, extended_cif *),
			    /*@out@*/ extended_cif *, 
			    unsigned, /*@out@*/ unsigned *, void (*fn)());
/*@=declundef@*/
/*@=exportheader@*/

void ffi_call(/*@dependent@*/ ffi_cif *cif, 
	      void (*fn)(), 
	      /*@out@*/ void *rvalue, 
	      /*@dependent@*/ void **avalue)
{
  extended_cif ecif;
  int dummy;

  ecif.cif = cif;
  ecif.avalue = avalue;
  
  /* If the return value is a struct and we don't have a return	*/
  /* value address then we need to make one		        */

  if ((rvalue == NULL) && 
      (examine_argument (cif->rtype, 1, &dummy, &dummy) == 0))
    {
      /*@-sysunrecog@*/
      ecif.rvalue = alloca(cif->rtype->size);
      /*@=sysunrecog@*/
    }
  else
    ecif.rvalue = rvalue;
    
  /* Stack must always be 16byte aligned. Make it so.  */
  cif->bytes = ALIGN(cif->bytes, 16);
  
  switch (cif->abi) 
    {
    case FFI_SYSV:
      /* Calling 32bit code from 64bit is not possible  */
      FFI_ASSERT(0);
      break;

    case FFI_UNIX64:
      /*@-usedef@*/
      ffi_call_UNIX64 (ffi_prep_args, ffi_fill_return_value, &ecif,
		       cif->bytes, ecif.rvalue, fn);
      /*@=usedef@*/
      break;

    default:
      FFI_ASSERT(0);
      break;
    }
}

extern void ffi_closure_UNIX64(void);

ffi_status
ffi_prep_closure (ffi_closure* closure,
		  ffi_cif* cif,
		  void (*fun)(ffi_cif*, void*, void**, void*),
		  void *user_data)
{
  volatile unsigned short *tramp;

  /* FFI_ASSERT (cif->abi == FFI_OSF);  */

  tramp = (volatile unsigned short *) &closure->tramp[0];
  tramp[0] = 0xbb49;		/* mov <code>, %r11	*/
  tramp[5] = 0xba49;		/* mov <data>, %r10	*/
  tramp[10] = 0xff49;		/* jmp *%r11	*/
  tramp[11] = 0x00e3;
  *(void * volatile *) &tramp[1] = ffi_closure_UNIX64;
  *(void * volatile *) &tramp[6] = closure;

  closure->cif = cif;
  closure->fun = fun;
  closure->user_data = user_data;

  return FFI_OK;
}

int
ffi_closure_UNIX64_inner(ffi_closure *closure, va_list l, void *rp)
{
  ffi_cif *cif;
  void **avalue;
  ffi_type **arg_types;
  long i, avn, argn;

  cif = closure->cif;
  avalue = alloca(cif->nargs * sizeof(void *));

  argn = 0;

  i = 0;
  avn = cif->nargs;
  arg_types = cif->arg_types;
  
  /* Grab the addresses of the arguments from the stack frame.  */
  while (i < avn)
    {
      switch (arg_types[i]->type)
	{
	case FFI_TYPE_SINT8:
	case FFI_TYPE_UINT8:
	case FFI_TYPE_SINT16:
	case FFI_TYPE_UINT16:
	case FFI_TYPE_SINT32:
	case FFI_TYPE_UINT32:
	case FFI_TYPE_SINT64:
	case FFI_TYPE_UINT64:
	case FFI_TYPE_POINTER:
	  {
	    if (l->gp_offset > 48-8)
	      {
		avalue[i] = l->overflow_arg_area;
		l->overflow_arg_area = (char *)l->overflow_arg_area + 8;
	      }
	    else
	      {
		avalue[i] = (char *)l->reg_save_area + l->gp_offset;
		l->gp_offset += 8;
	      }
	  }
	  break;

	case FFI_TYPE_STRUCT:
	  /* FIXME  */
	  FFI_ASSERT(0);
	  break;

	case FFI_TYPE_DOUBLE:
	  {
	    if (l->fp_offset > 176-16)
	      {
		avalue[i] = l->overflow_arg_area;
		l->overflow_arg_area = (char *)l->overflow_arg_area + 8;
	      }
	    else
	      {
		avalue[i] = (char *)l->reg_save_area + l->fp_offset;
		l->fp_offset += 16;
	      }
	  }
#if DEBUG_FFI
	  fprintf (stderr, "double arg %d = %g\n", i, *(double *)avalue[i]);
#endif
	  break;
	  
	case FFI_TYPE_FLOAT:
	  {
	    if (l->fp_offset > 176-16)
	      {
		avalue[i] = l->overflow_arg_area;
		l->overflow_arg_area = (char *)l->overflow_arg_area + 8;
	      }
	    else
	      {
		avalue[i] = (char *)l->reg_save_area + l->fp_offset;
		l->fp_offset += 16;
	      }
	  }
#if DEBUG_FFI
	  fprintf (stderr, "float arg %d = %g\n", i, *(float *)avalue[i]);
#endif
	  break;
	  
	default:
	  FFI_ASSERT(0);
	}

      argn += ALIGN(arg_types[i]->size, SIZEOF_ARG) / SIZEOF_ARG;
      i++;
    }

  /* Invoke the closure.  */
  (closure->fun) (cif, rp, avalue, closure->user_data);

  /* FIXME: Structs not supported.  */
  FFI_ASSERT(cif->rtype->type != FFI_TYPE_STRUCT);

  /* Tell ffi_closure_UNIX64 how to perform return type promotions.  */

  return cif->rtype->type;
}

#endif
