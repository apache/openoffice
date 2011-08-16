/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _BF_SOLAR_H
#define _BF_SOLAR_H

#include <tools/solar.h>

/*** common solar defines ***********************************/

#ifndef TRUE
#define TRUE		1
#endif
#ifndef FALSE
#define FALSE		0
#endif

/*** common solar types ********************************************/
/* NOTE: these types are deprecated, or soon will be.  They should */
/* not be used in new code, and should be replaced with their      */
/* corresponding types from sal/types.h in old code when possible. */
/*   Previous typedefs from before sal convergence are in comments */
/*   to the right of the new types.                                */

typedef sal_Bool		BOOL;	/* typedef unsigned char   BOOL; */
typedef sal_uInt8		BYTE;	/* typedef unsigned char   BYTE; */
typedef sal_uInt16		USHORT;	/* typedef unsigned short  USHORT; */
typedef sal_uIntPtr		ULONG;	/* typedef unsigned long   ULONG; */
typedef int 			FASTBOOL;

#if !defined(VCL_NEED_BASETSD) || defined(__MINGW32__)
#ifndef _SOLAR_NOUINT
typedef sal_Int16			INT16;	/* typedef short           INT16; */
typedef sal_uInt16			UINT16;	/* typedef unsigned short  UINT16; */
#if defined(SAL_W32)
typedef sal_sChar			INT8;
#else
typedef sal_Char			INT8;	/* typedef char            INT8; */
#endif
typedef sal_uInt8			UINT8;	/* typedef unsigned char   UINT8; */
#endif /* _SOLAR_NOUINT */
#endif

#ifndef VCL_NEED_BASETSD
#ifndef _SOLAR_NOUINT
/* types with exact defined size (not just the minimum size) */
typedef sal_Int64			INT64;	/* typedef long            INT64;  previously "void" on Windows */
typedef sal_uInt64			UINT64;	/* typedef unsigned long   UINT64; previously "void" on Windows */
typedef sal_Int32			INT32;	/* typedef int             INT32;  previously "long" on Windows */
typedef sal_uInt32			UINT32;	/* typedef unsigned int    UINT32; previously "unsigned long" on Windows */
#endif /* _SOLAR_NOUINT */
#endif

#endif	/* _BF_SOLAR_H */
