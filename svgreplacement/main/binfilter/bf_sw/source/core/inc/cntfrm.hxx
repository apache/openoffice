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


#ifndef _CNTFRM_HXX
#define _CNTFRM_HXX

#include <bf_svtools/bf_solar.h>

#include "frame.hxx"
#include "flowfrm.hxx"
#include "cshtyp.hxx"
namespace binfilter {

class SwLayoutFrm;
class SwPageFrm;
class SwCntntNode;
struct SwCrsrMoveState;
class SwBorderAttrs;
class SwAttrSetChg;

//Implementiert in cntfrm.cxx, wird von cntfrm.cxx und crsrsh.cxx angezogen
extern BOOL GetFrmInPage( const SwCntntFrm*, SwWhichPage, SwPosPage, SwPaM* );

class SwCntntFrm: public SwFrm, public SwFlowFrm
{
	friend void MakeNxt( SwFrm *pFrm, SwFrm *pNxt );	//ruft MakePrtArea

	BOOL _WouldFit( SwTwips nSpace, SwLayoutFrm *pNewUpper, BOOL bTstMove );
	virtual void MakeAll();

	void _UpdateAttr( SfxPoolItem*, SfxPoolItem*, BYTE &,
					  SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );

	virtual BOOL ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL, BOOL& );

protected:

	BOOL MakePrtArea( const SwBorderAttrs & );

	virtual void Modify( SfxPoolItem*, SfxPoolItem* );
    virtual SwTwips ShrinkFrm( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm  ( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );

	SwCntntFrm( SwCntntNode * const );

public:
	virtual ~SwCntntFrm();
	TYPEINFO();	//bereits in Basisklassen drin

	virtual void Cut();
	virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 );

	inline 		 SwCntntNode *GetNode();
	inline const SwCntntNode *GetNode() const;
				 USHORT 	  GetSectionLevel();

	inline const SwCntntFrm *GetFollow() const;
	inline		 SwCntntFrm *GetFollow();

		//Layoutabhaengiges Cursortravelling
	inline	BOOL	StartNextPage( SwPaM * ) const;
	inline	BOOL	StartPrevPage( SwPaM * ) const;
	inline	BOOL	StartCurrPage( SwPaM * ) const;
	inline	BOOL	EndCurrPage( SwPaM * ) const;
	inline	BOOL	EndNextPage( SwPaM * ) const;
	inline	BOOL	EndPrevPage( SwPaM * ) const;

	//nMaxHeight liefert die benoetigte Hoehe,
	//bSplit sagt, obj der Absatz gesplittet werden muss.
	virtual BOOL WouldFit( SwTwips &nMaxHeight, BOOL &bSplit );

};

inline SwCntntNode *SwCntntFrm::GetNode()
{
	return (SwCntntNode*)GetDep();
}
inline const SwCntntNode *SwCntntFrm::GetNode() const
{
	return (SwCntntNode*)GetDep();
}

inline const SwCntntFrm *SwCntntFrm::GetFollow() const
{
	return (const SwCntntFrm*)SwFlowFrm::GetFollow();
}
inline SwCntntFrm *SwCntntFrm::GetFollow()
{
	return (SwCntntFrm*)SwFlowFrm::GetFollow();
}

} //namespace binfilter
#endif
