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


#ifndef _SVX_OPTGRID_HXX
#define _SVX_OPTGRID_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
namespace binfilter {

class SvxGridTabPage;

// class SvxOptionsGrid --------------------------------------------------

class SvxOptionsGrid
{
protected:
	UINT32	nFldDrawX;
	UINT32 	nFldDivisionX;
	UINT32	nFldDrawY;
	UINT32 	nFldDivisionY;
	UINT32	nFldSnapX;
	UINT32	nFldSnapY;
	BOOL	bUseGridsnap:1;
	BOOL	bSynchronize:1;
	BOOL	bGridVisible:1;
	BOOL	bEqualGrid:	 1;

public:
	SvxOptionsGrid();
	~SvxOptionsGrid();

	void 	SetFldDrawX(	UINT32 nSet){nFldDrawX 		= nSet;}
	void 	SetFldDivisionX(UINT32 nSet){nFldDivisionX  = nSet;}
	void 	SetFldDrawY   (	UINT32 nSet){nFldDrawY      = nSet;}
	void 	SetFldDivisionY(UINT32 nSet){nFldDivisionY  = nSet;}
	void 	SetFldSnapX(	UINT32 nSet){nFldSnapX 		= nSet;}
	void 	SetFldSnapY   (	UINT32 nSet){nFldSnapY      = nSet;}
	void 	SetUseGridSnap( BOOL bSet ) {bUseGridsnap	= bSet;}
	void 	SetSynchronize( BOOL bSet ) {bSynchronize	= bSet;}
	void 	SetGridVisible( BOOL bSet ) {bGridVisible	= bSet;}
	void 	SetEqualGrid( BOOL bSet )	{bEqualGrid		= bSet;}

	UINT32	GetFldDrawX(	) const {  return nFldDrawX;    }
	UINT32 	GetFldDivisionX() const {  return nFldDivisionX;}
	UINT32	GetFldDrawY   (	) const {  return nFldDrawY;    }
	UINT32 	GetFldDivisionY() const {  return nFldDivisionY;}
	UINT32	GetFldSnapX(	) const {  return nFldSnapX;    }
	UINT32	GetFldSnapY   (	) const {  return nFldSnapY;    }
	BOOL	GetUseGridSnap( ) const {  return bUseGridsnap; }
	BOOL	GetSynchronize( ) const {  return bSynchronize; }
	BOOL	GetGridVisible( ) const {  return bGridVisible; }
	BOOL	GetEqualGrid()	  const {  return bEqualGrid;   }
};

// class SvxGridItem -----------------------------------------------------


// class SvxGridTabPage --------------------------------------------------


}//end of namespace binfilter
#endif

