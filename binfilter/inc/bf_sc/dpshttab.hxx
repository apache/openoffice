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



#ifndef SC_DPSHTTAB_HXX
#define SC_DPSHTTAB_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_DPTABDAT_HXX
#include "dptabdat.hxx"
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {


// --------------------------------------------------------------------
//
//	implementation of ScDPTableData with sheet data
//

struct ScSheetSourceDesc
{
	ScRange			aSourceRange;
	ScQueryParam	aQueryParam;

	BOOL operator==	( const ScSheetSourceDesc& rOther ) const
		{ return aSourceRange == rOther.aSourceRange &&
				 aQueryParam  == rOther.aQueryParam; }
};

 class ScSheetDPData_Impl;

class ScSheetDPData : public ScDPTableData
{
private:
	ScSheetDPData_Impl* pImpl;
    BOOL* pSpecial;

public:
					ScSheetDPData( ScDocument* pD, const ScSheetSourceDesc& rDesc );
	virtual			~ScSheetDPData();

	virtual long					GetColumnCount();
	virtual const TypedStrCollection&	GetColumnEntries(long nColumn);
	virtual String					getDimensionName(long nColumn);
	virtual BOOL					getIsDataLayoutDimension(long nColumn);
	virtual BOOL					IsDateDimension(long nDim);
	virtual UINT32					GetNumberFormat(long nDim);
	virtual void					DisposeData();
	virtual void					SetEmptyFlags( BOOL bIgnoreEmptyRows, BOOL bRepeatIfEmpty );

	virtual void					ResetIterator();
	virtual BOOL					GetNextRow( const ScDPTableIteratorParam& rParam );
};



} //namespace binfilter
#endif

