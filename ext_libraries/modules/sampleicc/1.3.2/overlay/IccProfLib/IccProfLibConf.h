/** @file
    File:       IccProfLibConf.h

    Contains:   Platform Specific Configuration

    Version:    V1

    Copyright:  © see ICC Software License
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

/* Header file guard bands */
#ifndef ICCCONFIG_h
#define ICCCONFIG_h

//Define the following to use namespace
//#define USESAMPLEICCNAMESPACE

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

//PC, visual C++
#if defined(_MSC_VER) && !defined(__MWERKS__) && (defined(_M_IX86) || defined(_M_AMD64))

  //Define how 64 bit integers are represented
  #define ICCUINT64 unsigned __int64
  #define ICCINT64 __int64
  #define ICUINT64TYPE unsigned __int64
  #define ICINT64TYPE __int64

  #define USE_WINDOWS_MB_SUPPORT
  #define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
  //#include <windows.h> //For Multibyte Translation Support

  #define ICC_BYTE_ORDER_LITTLE_ENDIAN

  #if defined(ICCPROFLIBDLL_EXPORTS)
    #define ICCPROFLIB_API _declspec(dllexport)
    #define ICCPROFLIB_EXTERN
  #elif defined(ICCPROFLIBDLL_IMPORTS)
    #define ICCPROFLIB_API _declspec(dllimport)
    #define ICCPROFLIB_EXTERN extern
  #else //just a regular lib
    #define ICCPROFLIB_API
    #define ICCPROFLIB_EXTERN
  #endif

#else // non-PC, perhaps Mac or Linux

#include <sal/types.h>
#include <osl/endian.h>

  #define ICCUINT64 sal_uInt64
  #define ICCINT64 sal_Int64
  #define ICUINT64TYPE sal_uInt64
  #define ICINT64TYPE sal_Int64
  #define ICINT32TYPE sal_Int32
  #define ICUINT32TYPE sal_uInt32

#if defined(_LITTLE_ENDIAN)
#    define ICC_BYTE_ORDER_LITTLE_ENDIAN
#elif defined(_BIG_ENDIAN)
#    define ICC_BYTE_ORDER_BIG_ENDIAN
#else
#    error "ENDIAN unknown"
#endif

  #define ICCPROFLIB_API
  #define ICCPROFLIB_EXTERN
  #define stricmp strcasecmp

#endif

// remove comment below if you want LAB to XYZ conversions to not clip negative XYZ values
//#define SAMPLEICC_NOCLIPLABTOXYZ

#ifdef SAMPLEICCCMM_EXPORTS
#define MAKE_A_DLL
#endif

#ifdef MAKE_A_DLL
#define SAMPLEICCEXPORT __declspec( dllexport)
#else
#define SAMPLEICCEXPORT __declspec( dllimport)
#endif

#ifdef USESAMPLEICCNAMESPACE
}
#endif

#endif //ICCCOFIG_h
