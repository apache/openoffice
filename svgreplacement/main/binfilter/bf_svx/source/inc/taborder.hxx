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


#ifndef _SVX_TABORDER_HXX
#define _SVX_TABORDER_HXX

#ifndef _SVTREEBOX_HXX
#include <bf_svtools/svtreebx.hxx>
#endif

#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLERMODEL_HPP_
#include <com/sun/star/awt/XTabControllerModel.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLCONTAINER_HPP_
#include <com/sun/star/awt/XControlContainer.hpp>
#endif

#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif


#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif


#ifndef _SVX_FMEXCH_HXX
#include "fmexch.hxx"
#endif

class SdrModel;
class ImageList;

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
FORWARD_DECLARE_INTERFACE(beans,XPropertySet)
namespace binfilter {
//STRIP008 FORWARD_DECLARE_INTERFACE(beans,XPropertySet)

//========================================================================
class FmOrderTreeListBox : public SvTreeListBox
{
protected:
	::binfilter::svxform::OControlExchangeHelper	m_aFieldExchange;//STRIP008 	::svxform::OControlExchangeHelper	m_aFieldExchange;

public:
	FmOrderTreeListBox( Window* pParent );
	FmOrderTreeListBox( Window* pParent, const ResId& rResId  );
	virtual ~FmOrderTreeListBox();


protected:
	virtual sal_Int8	AcceptDrop( const AcceptDropEvent& rEvt );
	virtual sal_Int8    ExecuteDrop( const ExecuteDropEvent& rEvt );
	virtual void		StartDrag( sal_Int8 nAction, const Point& rPosPixel );
};


//========================================================================
class FmFormShell;
class FmTabOrderDlg : public ModalDialog
{
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel > 		xTempModel,	// Model zum Bearbeiten der TabReihenfolge
								xModel;		// Model mit der zu veraendernden TabReihenfolge

	::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > 		xControlContainer;
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >	m_xORB;
	SdrModel*					pDrawModel;

    FixedText                   aFT_Controls;
	FmOrderTreeListBox			aLB_Controls;

	OKButton					aPB_OK;
	CancelButton				aPB_CANCEL;
	HelpButton					aPB_HELP;

    PushButton                  aPB_MoveUp;
	PushButton					aPB_MoveDown;
	PushButton					aPB_AutoOrder;

	ImageList*					pImageList;

	DECL_LINK( MoveUpClickHdl, Button* );
	DECL_LINK( MoveDownClickHdl, Button* );
	DECL_LINK( AutoOrderClickHdl, Button* );
	DECL_LINK( OKClickHdl, Button* );


public:
	FmTabOrderDlg(	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&	_xORB,
					Window* pParent, FmFormShell* pShell );
	virtual ~FmTabOrderDlg();

};

}//end of namespace binfilter
#endif

