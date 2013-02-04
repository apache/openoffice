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


#ifndef _IDXMRK_HXX
#define _IDXMRK_HXX

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _BASEDLGS_HXX
#include <bf_sfx2/basedlgs.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif
#ifndef _TOXE_HXX
#include "toxe.hxx"
#endif
#ifndef _STDCTRL_HXX
#include <bf_svtools/stdctrl.hxx>
#endif
#ifndef  _COM_SUN_STAR_I18N_XEXTENDEDINDEXENTRYSUPPLIER_HPP_
#include <com/sun/star/i18n/XExtendedIndexEntrySupplier.hpp>
#endif
namespace binfilter {
class SwWrtShell;
class SwTOXMgr;
class SwTOXMark;
/*--------------------------------------------------------------------
	 Beschreibung:	Markierung fuer Verzeichniseintrag einfuegen
 --------------------------------------------------------------------*/
class SwIndexMarkFloatDlg;
class SwIndexMarkModalDlg;
/* -----------------07.09.99 08:02-------------------

 --------------------------------------------------*/
class SwInsertIdxMarkWrapper : public SfxChildWindow
{
protected:
	SwInsertIdxMarkWrapper(	Window *pParentWindow,
							sal_uInt16 nId,
							SfxBindings* pBindings,
							SfxChildWinInfo* pInfo );

	SFX_DECL_CHILDWINDOW(SwInsertIdxMarkWrapper);

public:

};

/* -----------------15.09.99 08:39-------------------

 --------------------------------------------------*/
class SwAuthMarkModalDlg;
/* -----------------07.09.99 08:02-------------------

 --------------------------------------------------*/
class SwInsertAuthMarkWrapper : public SfxChildWindow
{
protected:
	SwInsertAuthMarkWrapper(	Window *pParentWindow,
							sal_uInt16 nId,
							SfxBindings* pBindings,
							SfxChildWinInfo* pInfo );

	SFX_DECL_CHILDWINDOW(SwInsertAuthMarkWrapper);

public:
};
} //namespace binfilter
#endif // _IDXMRK_HXX

