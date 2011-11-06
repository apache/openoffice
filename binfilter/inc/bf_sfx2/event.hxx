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


#ifndef _SFXEVENT_HXX
#define _SFXEVENT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SFXHINT_HXX //autogen
#include <bf_svtools/hint.hxx>
#endif

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
class PrintDialog;
class Printer;
namespace binfilter {

class SfxObjectShell;

//-------------------------------------------------------------------

class SfxEventHint : public SfxHint
{
	USHORT				nEventId;
	SfxObjectShell* 	pObjShell;
	String				_aArgs;
	BOOL				_bDummy;
	BOOL                _bAddToHistory;

public:
	TYPEINFO();
						SfxEventHint( USHORT nId,
									  const String& rArgs,
									  SfxObjectShell *pObj = 0  )
						:	nEventId(nId),
							pObjShell(pObj),
							_bAddToHistory(FALSE),
							_aArgs( rArgs )
						{}
						SfxEventHint( USHORT nId, SfxObjectShell *pObj = 0 )
						:	nEventId(nId),
							pObjShell(pObj),
							_bAddToHistory(FALSE)
						{}

	const String& GetArgs() const {	return _aArgs;}

	USHORT				GetEventId() const
						{ return nEventId; }

	SfxObjectShell* 	GetObjShell() const
						{ return pObjShell; }
};

//-------------------------------------------------------------------

class SfxNamedHint : public SfxHint
{
	String				_aEventName;
	SfxObjectShell* 	_pObjShell;
	String				_aArgs;
	BOOL				_bDummy;
	BOOL                _bAddToHistory;

public:
						TYPEINFO();

						SfxNamedHint( const String& rName,
									  const String& rArgs,
									  SfxObjectShell *pObj = 0  )
						:	_aEventName( rName ),
							_pObjShell( pObj),
							_bAddToHistory( FALSE ),
							_aArgs( rArgs )
						{}

						SfxNamedHint( const String& rName,
									  SfxObjectShell *pObj = 0 )
						:	_aEventName( rName ),
							_pObjShell( pObj ),
							_bAddToHistory( FALSE )
						{}

	const String&		GetArgs() const { return _aArgs;}
	const String&		GetName() const { return _aEventName; }
    SfxObjectShell*     GetObjShell() const { return _pObjShell; }
};

class SfxPrintingHint : public SfxHint
{
	PrintDialog*		pDialog;
	Printer*			pPrinter;
	sal_Int32			nWhich;
	::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue > aOpts;
public:
						TYPEINFO();
						SfxPrintingHint( sal_Int32 nEvent, PrintDialog* pDlg, Printer* pPrt, const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rOpts )
							: pDialog( pDlg )
							, pPrinter( pPrt )
							, nWhich( nEvent )
							, aOpts( rOpts )
						{}

						SfxPrintingHint( sal_Int32 nEvent, PrintDialog* pDlg, Printer* pPrt )
							: pDialog( pDlg )
							, pPrinter( pPrt )
							, nWhich( nEvent )
						{}

	Printer*  			GetPrinter() const { return pPrinter; }
	PrintDialog*		GetPrintDialog() const { return pDialog; }
	sal_Int32			GetWhich() const { return nWhich; }
	const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& GetAdditionalOptions() { return aOpts; }
};

}//end of namespace binfilter
#endif
