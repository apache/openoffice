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



#ifndef SC_DPOUTPUT_HXX
#define SC_DPOUTPUT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_SHEET_XDIMENSIONSSUPPLIER_HPP_
#include <com/sun/star/sheet/XDimensionsSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_SHEET_DATARESULT_HPP_
#include <com/sun/star/sheet/DataResult.hpp>
#endif


#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
class Rectangle;
class SvStream;
namespace binfilter {

class ScDocument;

struct ScDPOutLevelData;




struct ScDPPositionData
{
	long	nDimension;
	long	nHierarchy;
	long	nLevel;
	long	nFlags;
	String	aMemberName;

	ScDPPositionData() { nDimension = nHierarchy = nLevel = -1; nFlags = 0; }	// invalid
};



class ScDPOutput			//! name???
{
private:
	//!	use impl-object?
	ScDocument*				pDoc;
	::com::sun::star::uno::Reference<
		::com::sun::star::sheet::XDimensionsSupplier> xSource;
	ScAddress				aStartPos;
	BOOL					bDoFilter;
	ScDPOutLevelData*		pColFields;
	ScDPOutLevelData*		pRowFields;
	ScDPOutLevelData*		pPageFields;
	long					nColFieldCount;
	long					nRowFieldCount;
	long					nPageFieldCount;
	::com::sun::star::uno::Sequence<
		::com::sun::star::uno::Sequence<
			::com::sun::star::sheet::DataResult> > aData;
	BOOL					bResultsError;
	String					aDataDescription;
	UINT32*					pColNumFmt;
	UINT32*					pRowNumFmt;
	long					nColFmtCount;
	long					nRowFmtCount;

	BOOL					bSizesValid;
	BOOL					bSizeOverflow;
	long					nColCount;
	long					nRowCount;
	long					nHeaderSize;
	USHORT					nTabStartCol;
	USHORT					nTabStartRow;
	USHORT					nMemberStartCol;
	USHORT					nMemberStartRow;
	USHORT					nDataStartCol;
	USHORT					nDataStartRow;
	USHORT					nTabEndCol;
	USHORT					nTabEndRow;


public:
 					~ScDPOutput();

	void			SetPosition( const ScAddress& rPos );


};


} //namespace binfilter
#endif

