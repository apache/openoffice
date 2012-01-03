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



#ifndef SC_DETDATA_HXX
#define SC_DETDATA_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {


//------------------------------------------------------------------------

#define SC_DETOP_GROW	4

//------------------------------------------------------------------------
enum ScDetOpType
{
	SCDETOP_ADDSUCC,
	SCDETOP_DELSUCC,
	SCDETOP_ADDPRED,
	SCDETOP_DELPRED,
	SCDETOP_ADDERROR
};

//------------------------------------------------------------------------

class ScDetOpData
{
	ScAddress		aPos;
	ScDetOpType		eOperation;

public:
						ScDetOpData( const ScAddress& rP, ScDetOpType eOp ) :
							aPos(rP), eOperation(eOp) {}

						ScDetOpData( const ScDetOpData& rData ) :
							aPos(rData.aPos), eOperation(rData.eOperation) {}

	const ScAddress&	GetPos() const			{ return aPos; }
	ScDetOpType			GetOperation() const	{ return eOperation; }

	// fuer UpdateRef:
	void				SetPos(const ScAddress& rNew)	{ aPos=rNew; }

	int operator==		( const ScDetOpData& r ) const
							{ return eOperation == r.eOperation && aPos == r.aPos; }
};

//------------------------------------------------------------------------

//
//	Liste der Operationen
//

typedef ScDetOpData* ScDetOpDataPtr;

SV_DECL_PTRARR_DEL(ScDetOpArr_Impl, ScDetOpDataPtr, SC_DETOP_GROW, SC_DETOP_GROW)//STRIP008 ;

class ScDetOpList : public ScDetOpArr_Impl
{
	BOOL	bHasAddError;		// updated in Append

public:
		ScDetOpList() : bHasAddError(FALSE) {}
		ScDetOpList(const ScDetOpList& rList);
		~ScDetOpList() {}

/*N*/ 	void	UpdateReference( ScDocument* pDoc, UpdateRefMode eUpdateRefMode,
/*N*/ 								const ScRange& rRange, short nDx, short nDy, short nDz );


	void	Append( ScDetOpData* pData );

	void	Load( SvStream& rStream );
	void	Store( SvStream& rStream ) const;

	BOOL	HasAddError() const		{ return bHasAddError; }
};



} //namespace binfilter
#endif
