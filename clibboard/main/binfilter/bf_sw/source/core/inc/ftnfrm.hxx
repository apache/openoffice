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


#ifndef _FTNFRM_HXX
#define _FTNFRM_HXX

#include <bf_svtools/bf_solar.h>

#include "layfrm.hxx"
namespace binfilter {

class SwCntntFrm;
class SwTxtFtn;
class SwBorderAttrs;
class SwFtnFrm;

//Fuer Fussnoten gibt es einen Speziellen Bereich auf der Seite. Dieser
//Bereich ist ein SwFtnContFrm.
//Jede Fussnote ist durch einen SwFtnFrm abgegrenzt, dieser nimmt die
//Fussnotenabsaetze auf. SwFtnFrm koennen aufgespalten werden, sie gehen
//dann auf einer anderen Seite weiter.

class SwFtnContFrm: public SwLayoutFrm
{
public:
	SwFtnContFrm( SwFrmFmt* );


    virtual SwTwips ShrinkFrm( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm( SwTwips, SZPTR
							 BOOL bTst = FALSE, BOOL bInfo = FALSE );
	virtual void    Format( const SwBorderAttrs *pAttrs = 0 );
};

class SwFtnFrm: public SwLayoutFrm
{
	//Zeiger auf den FtnFrm in dem die Fussnote weitergefuehrt wird:
	// 0     wenn kein Follow vorhanden,
	// this  beim letzten
	// der Follow sonst.
	SwFtnFrm	 *pFollow;
	SwFtnFrm	 *pMaster;		//Der FtnFrm dessen Follow ich bin.
	SwCntntFrm	 *pRef;			//In diesem CntntFrm steht die Fussnotenref.
	SwTxtFtn	 *pAttr;		//Fussnotenattribut (zum wiedererkennen)

	BOOL bBackMoveLocked;		//Absaetze in dieser Fussnote duerfen derzeit
								//nicht rueckwaerts fliessen.
#ifdef DBG_UTIL
protected:
    virtual SwTwips ShrinkFrm( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm  ( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
#endif


public:
	SwFtnFrm( SwFrmFmt*, SwCntntFrm*, SwTxtFtn* );

	virtual void Cut();
	virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 );


#ifndef DBG_UTIL
	const SwCntntFrm *GetRef() const	{ return pRef; }
		 SwCntntFrm  *GetRef()  		{ return pRef; }
#else
	//JP 15.10.2001: in a non pro version test if the attribute has the same
	//				meaning which his reference is
	const SwCntntFrm *GetRef() const;
		 SwCntntFrm  *GetRef();
#endif
	const SwCntntFrm *GetRefFromAttr()  const;
		  SwCntntFrm *GetRefFromAttr();

	const SwFtnFrm *GetFollow() const 	{ return pFollow; }
		  SwFtnFrm *GetFollow() 		{ return pFollow; }

	const SwFtnFrm *GetMaster() const	{ return pMaster; }
		  SwFtnFrm *GetMaster() 		{ return pMaster; }

	const SwTxtFtn   *GetAttr() const 	{ return pAttr; }
		  SwTxtFtn	 *GetAttr() 	  	{ return pAttr; }

	void SetFollow( SwFtnFrm *pNew ) { pFollow = pNew; }
	void SetMaster( SwFtnFrm *pNew ) { pMaster = pNew; }
	void SetRef   ( SwCntntFrm *pNew ) { pRef = pNew; }

	void InvalidateNxtFtnCnts( SwPageFrm* pPage );

	void LockBackMove()		{ bBackMoveLocked = TRUE; }
	void UnlockBackMove()   { bBackMoveLocked = FALSE;}
	BOOL IsBackMoveLocked() { return bBackMoveLocked; }

	// Verhindert, dass der letzte Inhalt den SwFtnFrm mitloescht (Cut())
	inline void ColLock()		{ bColLocked = TRUE; }
	inline void ColUnlock()   	{ bColLocked = FALSE; }
};

} //namespace binfilter
#endif	//_FTNFRM_HXX
