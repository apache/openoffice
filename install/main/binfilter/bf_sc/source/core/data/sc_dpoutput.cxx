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

#include "scitems.hxx"
#include <bf_svx/algitem.hxx>
#include <bf_svx/boxitem.hxx>
#include <bf_svx/brshitem.hxx>
#include <bf_svx/wghtitem.hxx>

#include "dpoutput.hxx"
#include "document.hxx"
#include "patattr.hxx"
#include "docpool.hxx"
#include "markdata.hxx"
#include "attrib.hxx"
#include "compiler.hxx"		// errNoValue
#include "miscuno.hxx"
#include "globstr.hrc"
#include "stlpool.hxx"
#include "stlsheet.hxx"

#include <com/sun/star/sheet/XLevelsSupplier.hpp>
#include <com/sun/star/sheet/XHierarchiesSupplier.hpp>
#include <com/sun/star/sheet/XDataPilotResults.hpp>
#include <com/sun/star/sheet/XDataPilotMemberResults.hpp>
#include <com/sun/star/sheet/DataResultFlags.hpp>
#include <com/sun/star/sheet/MemberResultFlags.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/container/XNamed.hpp>
namespace binfilter {

using namespace ::com::sun::star;

// -----------------------------------------------------------------------

//!	move to a header file
#define DP_PROP_ORIENTATION			"Orientation"
#define DP_PROP_POSITION			"Position"
#define DP_PROP_USEDHIERARCHY		"UsedHierarchy"
#define DP_PROP_DATADESCR			"DataDescription"
#define DP_PROP_ISDATALAYOUT		"IsDataLayoutDimension"
#define DP_PROP_NUMBERFORMAT		"NumberFormat"

// -----------------------------------------------------------------------

//!	dynamic!!!
#define SC_DPOUT_MAXLEVELS	256


struct ScDPOutLevelData
{
	long								nDim;
	long								nHier;
	long								nLevel;
	long								nDimPos;
	uno::Sequence<sheet::MemberResult>	aResult;
	String								aCaption;

	ScDPOutLevelData() { nDim = nHier = nLevel = nDimPos = -1; }

	BOOL operator<(const ScDPOutLevelData& r) const
		{ return nDimPos<r.nDimPos || ( nDimPos==r.nDimPos && nHier<r.nHier ) ||
			( nDimPos==r.nDimPos && nHier==r.nHier && nLevel<r.nLevel ); }

	void Swap(ScDPOutLevelData& r)
//!		{ ScDPOutLevelData aTemp = r; r = *this; *this = aTemp; }
		{ ScDPOutLevelData aTemp; aTemp = r; r = *this; *this = aTemp; }

	//!	bug (73840) in uno::Sequence - copy and then assign doesn't work!
};

ScDPOutput::~ScDPOutput()
{
	delete[] pColFields;
	delete[] pRowFields;
	delete[] pPageFields;

	delete[] pColNumFmt;
	delete[] pRowNumFmt;
}

/*N*/ void ScDPOutput::SetPosition( const ScAddress& rPos )
/*N*/ {
/*N*/ 	aStartPos = rPos;
/*N*/  	bSizesValid = bSizeOverflow = FALSE;
/*N*/ }

}
