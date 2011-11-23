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



//#ifndef DBG_UTIL
//#error Wer fummelt denn an den makefiles rum?
//#endif

//#include "dbgloop.hxx"
//#include "errhdl.hxx"
namespace binfilter {

//DbgLoopStack DbgLoop::aDbgLoopStack;

/*************************************************************************
 *						class DbgLoopStack
 *************************************************************************/

///*N*/ DbgLoopStack::DbgLoopStack()
///*N*/ {
///*N*/ 	Reset();
///*N*/ }

///*N*/ void DbgLoopStack::Reset()
///*N*/ {
///*N*/ 	nPtr = 0;
///*N*/ 	pDbg = 0;
///*N*/ 	for( USHORT i = 0; i < DBG_MAX_STACK; ++i )
///*N*/ 		aCount[i] = 0;
///*N*/ }

/*************************************************************************
 *						 DbgLoopStack::Push()
 *************************************************************************/

///*N*/ void DbgLoopStack::Push( const void *pThis )
///*N*/ {
///*N*/ 	// Wir muessen irgendwie mitbekommen, wann die erste Stackposition
///*N*/ 	// resettet werden soll, z.B. wenn wir einen Nullpointer uebergeben
///*N*/ 	if( !nPtr && ( pDbg != pThis || !pThis ) )
///*N*/ 	{
///*N*/ 		aCount[1] = 0;
///*N*/ 		pDbg = pThis;
///*N*/ 	}
///*N*/ 
///*N*/ 	++nPtr;
///*N*/ 	if( DBG_MAX_STACK > nPtr )
///*N*/ 	{
///*N*/ 		// Wenn eine loop entdeckt wird, wird der counter wieder zurueckgesetzt.
///*N*/ 		ASSERT( DBG_MAX_LOOP > aCount[nPtr], "DbgLoopStack::Push: loop detected" );
///*N*/ 		if( DBG_MAX_LOOP > aCount[nPtr] )
///*N*/ 			++(aCount[nPtr]);
///*N*/ 		else
///*N*/ 			aCount[nPtr] = 0;
///*N*/ 	}
///*N*/ }

/*************************************************************************
 *						 DbgLoopStack::Pop()
 *************************************************************************/

///*N*/ void DbgLoopStack::Pop()
///*N*/ {
///*N*/ 	if( DBG_MAX_STACK > nPtr )
///*N*/ 	{
///*N*/ 		ASSERT( nPtr, "DbgLoopStack::Pop: can't pop the stack" );
///*N*/ 
///*N*/ 		ASSERT( aCount[nPtr], "DbgLoopStack::Pop: can't dec the count" );
///*N*/ 		if( DBG_MAX_STACK > nPtr + 1 )
///*N*/ 			aCount[nPtr + 1] = 0;
///*N*/ 	}
///*N*/ 	--nPtr;
///*N*/ }

/*************************************************************************
 *						 DbgLoopStack::Print()
 *************************************************************************/


//#ifdef STAND_ALONE
// compile with: cl /AL /DSTAND_ALONE dbgloop.cxx

/*************************************************************************
 *							main()
 *************************************************************************/

//#include <stdlib.h>

///*N*/ void AssertFail( const char *pErr, const char *pFile, USHORT nLine )
///*N*/ {
///*N*/ 	cout << pErr << '\n';
/////*N*/ 	PrintLoopStack( cout );
///*N*/ 	exit(0);
///*N*/ }

//#endif


}
