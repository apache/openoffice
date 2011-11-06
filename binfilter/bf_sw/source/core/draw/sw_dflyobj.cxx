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

#include "hintids.hxx"

#ifndef _SVX_PROTITEM_HXX //autogen
#include <bf_svx/protitem.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <bf_svx/svdpage.hxx>
#endif



#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "frmatr.hxx"
#include "dflyobj.hxx"
#include "frmfmt.hxx"
#include "flyfrms.hxx"
namespace binfilter {

static FASTBOOL bInResize = FALSE;

/*N*/ TYPEINIT1( SwFlyDrawObj, SdrObject )
/*N*/ TYPEINIT1( SwVirtFlyDrawObj, SdrVirtObj )

/*************************************************************************
|*
|*	SwFlyDrawObj::Ctor
|*
|*	Ersterstellung		MA 18. Apr. 95
|*	Letzte Aenderung	MA 28. May. 96
|*
*************************************************************************/
/*N*/ SwFlyDrawObj::SwFlyDrawObj()
/*N*/ {
/*N*/ 	bNotPersistent = TRUE;
/*N*/ 	mpLocalItemSet = NULL;
/*N*/ }

/*N*/ SwFlyDrawObj::~SwFlyDrawObj()
/*N*/ {
/*N*/ 	if(mpLocalItemSet)
/*?*/ 		delete mpLocalItemSet;
/*N*/ }



/*************************************************************************
|*
|*	SwFlyDrawObj::Paint()
|*
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	MA 20. May. 95
|*
*************************************************************************/


/*************************************************************************
|*
|*	SwFlyDrawObj::Factory-Methoden
|*
|*	Ersterstellung		MA 23. Feb. 95
|*	Letzte Aenderung	MA 23. Feb. 95
|*
*************************************************************************/

/*N*/ UINT32 __EXPORT SwFlyDrawObj::GetObjInventor() const
/*N*/ {
/*N*/ 	return SWGInventor;
/*N*/ }


/*N*/ UINT16 __EXPORT SwFlyDrawObj::GetObjIdentifier()	const
/*N*/ {
/*N*/ 	return SwFlyDrawObjIdentifier;
/*N*/ }



/*************************************************************************
|*
|*	SwVirtFlyDrawObj::CToren, Dtor
|*
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	MA 28. May. 96
|*
*************************************************************************/

/*N*/ SwVirtFlyDrawObj::SwVirtFlyDrawObj(SdrObject& rNew, SwFlyFrm* pFly) :
/*N*/ 	SdrVirtObj( rNew ),
/*N*/ 	pFlyFrm( pFly )
/*N*/ {
/*N*/ 	bNotPersistent = bNeedColorRestore = bWriterFlyFrame = TRUE;
/*N*/ 	const SvxProtectItem &rP = pFlyFrm->GetFmt()->GetProtect();
/*N*/ 	bMovProt = rP.IsPosProtected();
/*N*/ 	bSizProt = rP.IsSizeProtected();
/*N*/ }


/*N*/ __EXPORT SwVirtFlyDrawObj::~SwVirtFlyDrawObj()
/*N*/ {
/*N*/ 	if ( GetPage() )	//Der SdrPage die Verantwortung entziehen.
/*N*/ 		GetPage()->RemoveObject( GetOrdNum() );
/*N*/ }

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::GetFmt()
|*
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	MA 08. Dec. 94
|*
*************************************************************************/



/*N*/ SwFrmFmt *SwVirtFlyDrawObj::GetFmt()
/*N*/ {
/*N*/ 	return GetFlyFrm()->GetFmt();
/*N*/ }

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Paint()
|*
|*	Ersterstellung		MA 20. Dec. 94
|*	Letzte Aenderung	MA 18. Dec. 95
|*
*************************************************************************/


/*************************************************************************
|*
|*	SwVirtFlyDrawObj::CheckHit()
|*	Beschreibung		Das Teil ist genau dann getroffen wenn
|*						1. der Point im Rand des Frm liegt.
|*						2. der Point im heissen Bereich liegt.
|* 						3. der Point in der Flaeche liegt und es sich um
|* 						   einen Rahmen mit NoTxtFrm handelt und dieser
|*						   keine URL traegt.
|*						3a nicht aber wenn ueber dem Fly noch ein Fly liegt,
|* 						   und der Point in dessen Flaeche nicht steht.
|* 						4. der Point in der Flaeche liegt und der Rahmen
|* 						   selektiert ist.
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	JP 25.03.96
|*
*************************************************************************/


/*************************************************************************
|*
|*	SwVirtFlyDrawObj::TakeObjInfo()
|*
|*	Ersterstellung		MA 03. May. 95
|*	Letzte Aenderung	MA 03. May. 95
|*
*************************************************************************/



/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Groessenermittlung
|*
|*	Ersterstellung		MA 12. Jan. 95
|*	Letzte Aenderung	MA 10. Nov. 95
|*
*************************************************************************/

/*N*/ void SwVirtFlyDrawObj::SetRect() const
/*N*/ {
/*N*/ 	if ( GetFlyFrm()->Frm().HasArea() )
/*N*/ 		((SwVirtFlyDrawObj*)this)->aOutRect = GetFlyFrm()->Frm().SVRect();
/*N*/ 	else
/*?*/ 		((SwVirtFlyDrawObj*)this)->aOutRect = Rectangle();
/*N*/ }


/*N*/ const Rectangle& __EXPORT SwVirtFlyDrawObj::GetBoundRect() const
/*N*/ {
/*N*/ 	SetRect();
/*N*/ 	return aOutRect;
/*N*/ }






/*N*/ const Rectangle& __EXPORT SwVirtFlyDrawObj::GetSnapRect()  const
/*N*/ {
/*N*/ 	SetRect();
/*N*/ 	return aOutRect;
/*N*/ }













/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Move() und Resize()
|*
|*	Ersterstellung		MA 12. Jan. 95
|*	Letzte Aenderung	MA 26. Jul. 96
|*
*************************************************************************/















}
