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

#define ITEMID_BOXINFO SID_ATTR_BORDER_INNER
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _FESH_HXX
#include <fesh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _CELLFRM_HXX
#include <cellfrm.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _TABCOL_HXX
#include <tabcol.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif

#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
namespace binfilter {

//siehe auch swtable.cxx
#define COLFUZZY 20L


/*N*/ SwTabCols *pLastCols   = 0;



/*N*/ inline const SwCursor& GetShellCursor( const SwCrsrShell& rShell )
/*N*/ {
/*N*/ 	const SwShellCrsr *pCrsr = rShell.GetTableCrsr();
/*N*/ 	if( !pCrsr )
/*N*/ 		pCrsr = (SwShellCrsr*)*rShell.GetSwCrsr( FALSE );
/*N*/ 	return *pCrsr;
/*N*/ }


/*N*/ void ClearFEShellTabCols()
/*N*/ {
/*N*/ 	DELETEZ( pLastCols );
/*N*/ }

}
