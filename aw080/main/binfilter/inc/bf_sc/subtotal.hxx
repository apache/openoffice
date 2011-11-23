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



#ifndef SC_SUBTOTAL_HXX
#define SC_SUBTOTAL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef MSC
	#include <setjmp.h>
#else
	extern "C"
	{
	#define _JBLEN  9  /* bp, di, si, sp, ret addr, ds */
	typedef  int  jmp_buf[_JBLEN];
	#define _JMP_BUF_DEFINED
	#define setjmp  _setjmp
	int  __cdecl _setjmp(jmp_buf);
	void __cdecl longjmp(jmp_buf, int);
	};
#endif

namespace binfilter {

class SubTotal
{
private:
	long	nCount;
	long	nCount2;
	double	nSum;
	double	nSumSqr;
	double	nMax;
	double	nMin;
	double	nProduct;
	BOOL	bSumOk;
	BOOL	bSumSqrOk;
	BOOL	bProductOk;
	USHORT  nProgress;

public:
	USHORT	nIndex;			// Test
	static jmp_buf aGlobalJumpBuf;

public:
			SubTotal();
			~SubTotal();

	void	Update( double nVal );
	static  BOOL SafePlus( double& fVal1, double fVal2);
	static  BOOL SafeMult( double& fVal1, double fVal2);
};


struct ScFunctionData					// zum Berechnen von einzelnen Funktionen
{
	ScSubTotalFunc	eFunc;
	double			nVal;
	long			nCount;
	BOOL			bError;

	ScFunctionData( ScSubTotalFunc eFn ) :
		eFunc(eFn), nVal(0.0), nCount(0), bError(FALSE) {}
};


} //namespace binfilter
#endif


