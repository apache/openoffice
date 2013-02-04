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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif

#ifndef _SFXREQUEST_HXX
#include <bf_sfx2/request.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <bf_svtools/useroptions.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_XSELECTIONSUPPLIER_HPP_
#include <com/sun/star/view/XSelectionSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>	// helper for implementations
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#include <bf_svx/dataaccessdescriptor.hxx>
#endif

#ifndef _OFF_APP_HXX //autogen
#include <bf_offmgr/app.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <bf_svx/wghtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX //autogen
#include <bf_svx/postitem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <bf_svx/udlnitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX //autogen
#include <bf_svx/crsditem.hxx>
#endif
#ifndef _SVX_CMAPITEM_HXX //autogen
#include <bf_svx/cmapitem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <bf_svx/colritem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <bf_svx/brshitem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif

#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _USRPREF_HXX
#include <usrpref.hxx>
#endif
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _UINUMS_HXX
#include <uinums.hxx>
#endif
#ifndef _PRTOPT_HXX
#include <prtopt.hxx>		// fuer PrintOptions
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _APP_HRC
#include <app.hrc>
#endif
#include "helpid.h"
namespace binfilter {

using namespace ::rtl;
using namespace ::binfilter ::svx;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::view;
using namespace ::com::sun::star::lang;
#define C2U(char) ::rtl::OUString::createFromAscii(char)

/*------------------------------------------------------------------------
 Beschreibung:	Neuer Master fuer die Einstellungen wird gesetzt;
				dieser wirkt sich auf die aktuelle Sicht und alle
				folgenden aus.
------------------------------------------------------------------------*/

/*M*/ void SwModule::ApplyUsrPref(const SwViewOption &rUsrPref, SwView* pActView,
/*M*/ 							sal_uInt16 nDest )
/*M*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001
/*M*/ }
/*-----------------05.02.97 08.03-------------------

--------------------------------------------------*/

/*N*/ SwPrintOptions* 	SwModule::GetPrtOptions(sal_Bool bWeb)
/*N*/ {
/*N*/ 	if(bWeb && !pWebPrtOpt)
/*N*/ 	{
/*N*/ 		pWebPrtOpt = new SwPrintOptions(TRUE);
/*N*/ 	}
/*N*/ 	else if(!bWeb && !pPrtOpt)
/*N*/ 	{
/*N*/ 		pPrtOpt = new SwPrintOptions(FALSE);
/*N*/ 	}
/*N*/
/*N*/ 	return bWeb ? pWebPrtOpt : pPrtOpt;
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Redlining
 --------------------------------------------------------------------*/

/*N*/ sal_uInt16 SwModule::GetRedlineAuthor()
/*N*/ {
DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 	if (!bAuthorInitialised)
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*N*/ const String& SwModule::GetRedlineAuthor(sal_uInt16 nPos)
/*N*/ {
/*N*/ 	return *pAuthorNames->GetObject(nPos);
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*N*/ sal_uInt16 SwModule::InsertRedlineAuthor(const String& rAuthor) //SW50.SDW
/*N*/ {
/*N*/ 	sal_uInt16 nPos = 0;
/*N*/
/*N*/ 	while (nPos < pAuthorNames->Count() && *pAuthorNames->GetObject(nPos) != rAuthor)
/*N*/ 		nPos++;
/*N*/
/*N*/ 	if (nPos == pAuthorNames->Count())
/*N*/ 		pAuthorNames->Insert(new String(rAuthor), nPos);
/*N*/
/*N*/ 	return nPos;
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*N*/ sal_uInt16 SwModule::GetRedlineMarkPos()
/*N*/ {
/*N*/ 	return pModuleConfig->GetMarkAlignMode();
/*N*/ }


/*-----------------03.03.98 16:47-------------------

--------------------------------------------------*/
/*N*/ const SwViewOption*	SwModule::GetViewOption(sal_Bool bWeb)
/*N*/ {
/*N*/ 	return GetUsrPref( bWeb );
/*N*/ }

// returne den definierten DocStat - WordDelimiter
/*N*/ const String& SwModule::GetDocStatWordDelim() const
/*N*/ {
/*N*/ 	return pModuleConfig->GetWordDelimiter();
/*N*/ }
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// Durchreichen der Metric von der ModuleConfig (fuer HTML-Export)
/*N*/ sal_uInt16 SwModule::GetMetric( sal_Bool bWeb ) const
/*N*/ {
/*N*/ 	SwMasterUsrPref* pPref;
/*N*/ 	if(bWeb)
/*N*/ 	{
/*?*/ 		if(!pWebUsrPref)
/*?*/ 			GetUsrPref(sal_True);
/*?*/ 		pPref = pWebUsrPref;
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		if(!pUsrPref)
/*?*/ 			GetUsrPref(sal_False);
/*N*/ 		pPref = pUsrPref;
/*N*/ 	}
/*N*/ 	return pPref->GetMetric();
/*N*/ }
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// Update-Stati durchreichen
/*N*/ sal_uInt16 SwModule::GetLinkUpdMode( sal_Bool ) const
/*N*/ {
/*N*/ 	if(!pUsrPref)
/*N*/ 		GetUsrPref(sal_False);
/*N*/     return (sal_uInt16)pUsrPref->GetUpdateLinkMode();
/*N*/ }
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ sal_uInt16 SwModule::GetFldUpdateFlags( sal_Bool ) const
/*N*/ {
/*N*/ 	if(!pUsrPref)
/*N*/ 		GetUsrPref(sal_False);
/*N*/ 	return (sal_uInt16)pUsrPref->GetFldUpdateFlags();
/*N*/ }
}
