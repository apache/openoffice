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


// INCLUDE ---------------------------------------------------------------

#ifdef RS6000

#include <fptrap.h>
#include <fpxcp.h>

#elif defined ( MAC )

#include <MAC_START.h>
#include <fp.h>
#include <MAC_END.h>

#endif

#ifdef PCH
#endif


#include <float.h>
//#include <math.h>
#ifdef SOLARIS
#include <ieeefp.h>
#endif
#include <signal.h>

#include "subtotal.hxx"
#include "interpre.hxx"
namespace binfilter {

// STATIC DATA -----------------------------------------------------------

jmp_buf SubTotal::aGlobalJumpBuf;

// -----------------------------------------------------------------------

/*N*/ SubTotal::SubTotal()
/*N*/ {
/*N*/	nIndex = 0;				//! test fuer Pivot
/*N*/ 
/*N*/ 	nCount	   = 0;
/*N*/ 	nCount2	   = 0;
/*N*/ 	nSum	   = 0.0;
/*N*/ 	nSumSqr	   = 0.0;
/*N*/ 	nMax	   = -MAXDOUBLE;
/*N*/ 	nMin	   = MAXDOUBLE;
/*N*/ 	nProduct   = 1.0;
/*N*/ 	bSumOk     = TRUE;
/*N*/ 	bSumSqrOk  = TRUE;
/*N*/ 	bProductOk = TRUE;
/*N*/ }


/*N*/ SubTotal::~SubTotal()
/*N*/ {
/*N*/ }




/*N*/ void SubTotal::Update( double nVal )
/*N*/ {
/*N*/ 	SAL_MATH_FPEXCEPTIONS_OFF();
/*N*/     nCount++;
/*N*/     nCount2++;
/*N*/     if (nVal > nMax) nMax = nVal;
/*N*/     if (nVal < nMin) nMin = nVal;
/*N*/     nProgress = 0;
/*N*/     if (bSumOk) nSum += nVal;
/*N*/     nProgress = 1;
/*N*/     if (bProductOk) nProduct *= nVal;
/*N*/     nProgress = 2;
/*N*/     if (bSumSqrOk) nSumSqr += nVal*nVal;
/*N*/ 	if (!::rtl::math::isFinite(nSum))
/*N*/ 		bSumOk = FALSE;
/*N*/ 	if (!::rtl::math::isFinite(nProduct))
/*N*/ 		bProductOk = FALSE;
/*N*/ 	if (!::rtl::math::isFinite(nSumSqr))
/*N*/ 		bSumSqrOk = FALSE;
/*N*/ }








/*N*/ BOOL SubTotal::SafePlus(double& fVal1, double fVal2)
/*N*/ {
/*N*/ 	BOOL bOk = TRUE;
/*N*/ 	SAL_MATH_FPEXCEPTIONS_OFF();
/*N*/     fVal1 += fVal2;
/*N*/ 	if (!::rtl::math::isFinite(fVal1))
/*N*/ 	{
/*N*/ 		bOk = FALSE;
/*N*/ 		if (fVal2 > 0.0)
/*N*/ 			fVal1 = DBL_MAX;
/*N*/ 		else
/*N*/ 			fVal1 = -DBL_MAX;
/*N*/ 	}
/*N*/ 	return bOk;
/*N*/ }


/*N*/ BOOL SubTotal::SafeMult(double& fVal1, double fVal2)
/*N*/ {
/*N*/ 	BOOL bOk = TRUE;
/*N*/ 	SAL_MATH_FPEXCEPTIONS_OFF();
/*N*/     fVal1 *= fVal2;
/*N*/ 	if (!::rtl::math::isFinite(fVal1))
/*N*/ 	{
/*N*/ 		bOk = FALSE;
/*N*/ 		fVal1 = DBL_MAX;
/*N*/ 	}
/*N*/ 	return bOk;
/*N*/ }


}
