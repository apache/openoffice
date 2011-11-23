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



#ifndef _SVX_SHOWCOLS_HXX
#define _SVX_SHOWCOLS_HXX

#ifndef _SV_DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
namespace binfilter {

//==========================================================================
//	FmShowColsDialog
//==========================================================================

class FmShowColsDialog : public ModalDialog
{
	ListBox			m_aList;
	FixedText		m_aLabel;
	OKButton		m_aOK;
	CancelButton	m_aCancel;

	::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > 	m_xColumns;

public:
	FmShowColsDialog( Window* pParent );
	virtual ~FmShowColsDialog();

	void SetColumns(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer>& xCols);

protected:
	DECL_LINK( OnClickedOk, Button* );
};

}//end of namespace binfilter
#endif // _SVX_SHOWCOLS_HXX

