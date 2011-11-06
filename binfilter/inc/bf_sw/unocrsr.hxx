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


#ifndef _UNOCRSR_HXX
#define _UNOCRSR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SWCRSR_HXX
#include <swcrsr.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
namespace binfilter {


class SwUnoCrsr : public virtual SwCursor, public SwModify
{
	BOOL bRemainInSection : 1;
	BOOL bSkipOverHiddenSections : 1;
	BOOL bSkipOverProtectSections : 1;

public:
	SwUnoCrsr( const SwPosition &rPos, SwPaM* pRing = 0 );
	virtual ~SwUnoCrsr();

	virtual operator SwUnoCrsr* ();

//	virtual SwCursor* Create( SwPaM* pRing = 0 ) const;

	// gibt es eine Selection vom Content in die Tabelle
	// Return Wert gibt an, ob der Crsr auf der alten Position verbleibt
	virtual FASTBOOL IsSelOvr( int eFlags =
								( SELOVER_CHECKNODESSECTION |
								  SELOVER_TOGGLE | SELOVER_CHANGEPOS ));

	BOOL IsRemainInSection() const 			{ return bRemainInSection; }
	void SetRemainInSection( BOOL bFlag )	{ bRemainInSection = bFlag; }

	BOOL IsSkipOverProtectSections() const
									{ return bSkipOverProtectSections; }
	void SetSkipOverProtectSections( BOOL bFlag )
									{ bSkipOverProtectSections = bFlag; }

	BOOL IsSkipOverHiddenSections() const
									{ return bSkipOverHiddenSections; }
	void SetSkipOverHiddenSections( BOOL bFlag )
									{ bSkipOverHiddenSections = bFlag; }

	DECL_FIXEDMEMPOOL_NEWDEL( SwUnoCrsr )
};



class SwUnoTableCrsr : public virtual SwUnoCrsr, public virtual SwTableCursor
{
	// die Selection hat die gleiche Reihenfolge wie die
	// TabellenBoxen. D.h., wird aus dem einen Array an einer Position
	// etwas geloescht, dann muss es auch im anderen erfolgen!!
	SwCursor aTblSel;

public:
	SwUnoTableCrsr( const SwPosition& rPos );
	virtual ~SwUnoTableCrsr();

//	virtual SwCursor* Create( SwPaM* pRing = 0 ) const;

	virtual operator SwUnoCrsr* ();
	virtual operator SwUnoTableCrsr* ();
	virtual operator SwTableCursor* ();

	// gibt es eine Selection vom Content in die Tabelle
	// Return Wert gibt an, ob der Crsr auf der alten Position verbleibt
	virtual FASTBOOL IsSelOvr( int eFlags =
								( SELOVER_CHECKNODESSECTION |
								  SELOVER_TOGGLE | SELOVER_CHANGEPOS ));

	void MakeBoxSels();
 		  SwCursor& GetSelRing() 			{ return aTblSel; }
	const SwCursor& GetSelRing() const 		{ return aTblSel; }
};



} //namespace binfilter
#endif
