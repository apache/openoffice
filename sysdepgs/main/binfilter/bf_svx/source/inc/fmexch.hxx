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


#ifndef _SVX_FMEXCH_HXX
#define _SVX_FMEXCH_HXX

#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#ifndef _TRANSFER_HXX
#include <bf_svtools/transfer.hxx>
#endif

class SvTreeListBox;
namespace binfilter {

class FmFormShell;
class FmFormPage;
class SvLBoxEntry;

//========================================================================
// Exchange types
#define SVX_FM_FIELD_EXCH			String("SvxFormFieldExch",				sizeof("SvxFormFieldExch"))
#define SVX_FM_CONTROL_EXCH			String("SvxFormExplCtrlExch",			sizeof("SvxFormExplCtrlExch"))
#define SVX_FM_CONTROLS_AS_PATH		String("SvxFormControlsAsPathExchange",	sizeof("SvxFormControlsAsPathExchange"))
#define SVX_FM_HIDDEN_CONTROLS		String("SvxFormHiddenControlsExchange",	sizeof("SvxFormHiddenControlsExchange"))
#define SVX_FM_FILTER_FIELDS		String("SvxFilterFieldExchange",		sizeof("SvxFilterFieldExchange"))

//========================================================================

//........................................................................
namespace svxform
{
//........................................................................

	//====================================================================

	DECLARE_STL_VECTOR( SvLBoxEntry*, ListBoxEntryArray );

	//====================================================================
	//= OLocalExchange
	//====================================================================

	//====================================================================
	//= OLocalExchangeHelper
	//====================================================================
	/// a helper for navigator windows (SvTreeListBox'es) which allow DnD within themself
	class OLocalExchangeHelper
	{
	protected:
		Window*				m_pDragSource;

	public:
		OLocalExchangeHelper( Window* _pDragSource );
		~OLocalExchangeHelper();

		void		prepareDrag( );

		void		startDrag( sal_Int8 nDragSourceActions );
		void		copyToClipboard( ) const;



	protected:

	protected:
		void implReset();
	};

	//====================================================================
	//= OControlTransferData
	//====================================================================

	//====================================================================

	//====================================================================
	//= OControlExchange
	//====================================================================

	//====================================================================
	//= OControlExchangeHelper
	//====================================================================
/*N*/ 	class OControlExchangeHelper : public OLocalExchangeHelper
/*N*/ 	{
/*N*/ 	public:
/*N*/ 		OControlExchangeHelper(Window* _pDragSource) : OLocalExchangeHelper(_pDragSource) { }
/*N*/ 
/*N*/ 
/*N*/ 	protected:
/*N*/ 	};

	//====================================================================
	//====================================================================

//........................................................................
}	// namespace svxform
//........................................................................

}//end of namespace binfilter
#endif

