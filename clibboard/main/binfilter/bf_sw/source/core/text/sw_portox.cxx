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




#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "portox.hxx"
namespace binfilter {

/*************************************************************************
 *               virtual SwToxPortion::Paint()
 *************************************************************************/


/*************************************************************************
 *                      class SwIsoToxPortion
 *************************************************************************/

/*N*/ SwLinePortion *SwIsoToxPortion::Compress() { return this; }
/*N*/ 
/*N*/ SwIsoToxPortion::SwIsoToxPortion() : nViewWidth(0)
/*N*/ {
/*N*/ 	SetLen(1);
/*N*/ 	SetWhichPor( POR_ISOTOX );
/*N*/ }

/*************************************************************************
 *				 virtual SwIsoToxPortion::GetViewWidth()
 *************************************************************************/


/*************************************************************************
 *                 virtual SwIsoToxPortion::Format()
 *************************************************************************/

/*N*/ sal_Bool SwIsoToxPortion::Format( SwTxtFormatInfo &rInf )
/*N*/ {
/*N*/ 	const sal_Bool bFull = SwLinePortion::Format( rInf );
/*N*/ 	return bFull;
/*N*/ }

/*************************************************************************
 *               virtual SwIsoToxPortion::Paint()
 *************************************************************************/


/*************************************************************************
 *              virtual SwIsoToxPortion::HandlePortion()
 *************************************************************************/


}
