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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif


#ifndef _WRTSWTBL_HXX
#include <wrtswtbl.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

namespace binfilter {



//-----------------------------------------------------------------------


//-----------------------------------------------------------------------




//-----------------------------------------------------------------------

/*N*/ SwWriteTableCol::SwWriteTableCol( USHORT nPosition )
/*N*/ 	: nPos(nPosition),
/*N*/ 	bLeftBorder(TRUE), bRightBorder(TRUE),
/*N*/ 	nWidthOpt( 0 ), bRelWidthOpt( FALSE ),
/*N*/ 	bOutWidth( TRUE )
/*N*/ {
/*N*/ }

//-----------------------------------------------------------------------

/*N*/ long SwWriteTable::GetBoxWidth( const SwTableBox *pBox )
/*N*/ {
/*N*/ 	const SwFrmFmt *pFmt = pBox->GetFrmFmt();
/*N*/ 	const SwFmtFrmSize& aFrmSize=
/*N*/ 		(const SwFmtFrmSize&)pFmt->GetAttr( RES_FRM_SIZE );
/*N*/ 
/*N*/ 	return aFrmSize.GetSize().Width();
/*N*/ }


























}
