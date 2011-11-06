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

// INCLUDE ---------------------------------------------------------------

#include "scitems.hxx"
#include <bf_svx/boxitem.hxx>
#include <bf_svx/shaditem.hxx>

#include "document.hxx"
#include "cell.hxx"
#include "attarray.hxx"
#include "markdata.hxx"
#include "patattr.hxx"
#include "poolhelp.hxx"
#include "docpool.hxx"
#include "conditio.hxx"
#include "stlpool.hxx"
namespace binfilter {


// -----------------------------------------------------------------------

enum FillInfoLinePos
	{
		FILP_TOP,
		FILP_BOTTOM,
		FILP_LEFT,
		FILP_RIGHT
	};


/*N*/ inline const SvxBorderLine* GetNullOrLine( const SvxBoxItem* pBox, FillInfoLinePos eWhich )
/*N*/ {
/*N*/  if (pBox)
/*N*/ 	{
/*N*/ 		if (eWhich==FILP_TOP)
/*N*/ 			return pBox->GetTop();
/*N*/ 		else if (eWhich==FILP_BOTTOM)
/*N*/ 			return pBox->GetBottom();
/*N*/ 		else if (eWhich==FILP_LEFT)
/*N*/ 			return pBox->GetLeft();
/*N*/ 		else
/*N*/ 			return pBox->GetRight();
/*N*/ 	}
/*N*/ 	else
/*N*/ 		return NULL;
/*N*/ }

/*N*/ inline BOOL ScDocument::RowHidden( USHORT nRow, USHORT nTab )
/*N*/ {
/*N*/ 	return ( pTab[nTab]->pRowFlags[nRow] & CR_HIDDEN ) != 0;
/*N*/ }

}
