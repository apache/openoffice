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
#pragma optimize("",off)
#endif


#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <bf_sfx2/docfilt.hxx>
#endif

#ifndef _SFXAPP_HXX
#include <bf_sfx2/app.hxx>
#endif

#ifndef _SO_CLSIDS_HXX
#include <comphelper/classids.hxx>
#endif

#include <sot/formats.hxx>
#include "schmod.hxx"
#include "docshell.hxx"
#include "schlib.hxx"

class ValueSet;

#include "res_bmp.hrc"
namespace binfilter {

#define SCH_FORMAT_STR_50_FORMAT String( RTL_CONSTASCII_USTRINGPARAM( "StarChart 5.0" ))

/*?*/ TYPEINIT1( SchModuleDummy, SfxModule );

extern "C" { static void SAL_CALL thisModule() {} }

/*?*/ SFX_IMPL_OBJECTFACTORY_LIB( SchChartDocShell, SFXOBJECTSHELL_STD_NORMAL, schart,
							SvGlobalName(BF_SO3_SCH_CLASSID), Sch, String( RTL_CONSTASCII_USTRINGPARAM( DLL_NAME )) )
// from macro: void __EXPORT SchChartDocShell::InitFactory()
/*?*/ {
/*?*/ 	ULONG nFormat50 = SOT_FORMATSTR_ID_STARCHART_50;
/*?*/ 	SfxObjectFactory& rFactory = (SfxObjectFactory&)Factory();
/*?*/ 	rFactory.SetDocumentServiceName(String::CreateFromAscii("com.sun.star.chart.ChartDocument"));
/*?*/ }

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

/*?*/ SchDLL::SchDLL()
/*?*/ {
/*?*/ }

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

/*?*/ SchDLL::~SchDLL()
/*?*/ {
/*?*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/


/*?*/ void SchDLL::LibInit()
/*?*/ {
/*?*/ 	// Das Anlegen des ResMgr ist nur fuer die obige Filter-Registierung
/*?*/ 	// notwendig. Sobald dort kein SdResId mehr notwendig ist, kann das
/*?*/ 	// Anlegen entfallen
/*?*/ 	SchChartDocShell::RegisterFactory(SDT_SCH_DOCFACTPRIO);
/*?*/ 
/*?*/ 	SCH_MOD() = (SchModule *) new SchModuleDummy(NULL, TRUE, &SchChartDocShell::Factory() );
/*?*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/


/*?*/ void SchDLL::LibExit()
/*?*/ {
/*?*/ 	FreeLibSch();
/*?*/ 
/*?*/ 	// destroy the dummy-module with Object-Factory-Pointer
/*?*/ 	DELETEZ( SCH_MOD() );
/*?*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/


/*N*/ void __EXPORT SchDLL::Update( SvInPlaceObjectRef aIPObj, SchMemChart* pData, OutputDevice* pOut )
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI*fp)(SvInPlaceObjectRef, SchMemChart*, OutputDevice*);
/*N*/ 
/*N*/ 	fp = (void (__LOADONCALLAPI*)(SvInPlaceObjectRef, SchMemChart*, OutputDevice*))GetFuncSch("SchUpdate");
/*N*/ 
/*N*/ 	if (fp)
/*N*/ 	{
/*N*/ 	fp(aIPObj, pData, pOut);
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/


/*N*/ SchMemChart* __EXPORT SchDLL::GetChartData (SvInPlaceObjectRef aIPObj)
/*N*/ {
/*N*/ 	SchMemChart* (__LOADONCALLAPI*fp)(SvInPlaceObjectRef);
/*N*/ 
/*N*/ 	fp = (SchMemChart* (__LOADONCALLAPI*)(SvInPlaceObjectRef aIPObj))GetFuncSch("SchGetChartData");
/*N*/ 
/*N*/ 	return fp
/*N*/ 			   ? fp (aIPObj)
/*N*/ 			   : 0;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ SchMemChart* __EXPORT SchDLL::NewMemChart (short nCols, short nRows)
/*N*/ {
/*N*/ 	SchMemChart* (__LOADONCALLAPI*fp)(short, short);
/*N*/ 
/*N*/ 	fp = (SchMemChart* (__LOADONCALLAPI*)(short, short))GetFuncSch("SchNewMemChartXY");
/*N*/ 
/*N*/ 	if (fp)
/*N*/ 	{
/*N*/ 		return fp(nCols, nRows);
/*N*/ 	}
/*N*/ 	else return 0;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ void __EXPORT SchDLL::GetDefaultForColumnText( const SchMemChart& rMemChart, sal_Int32 nCol, String& aResult )
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI*fp)( const SchMemChart&, sal_Int32, String& );
/*N*/ 
/*N*/ 	fp = (void (__LOADONCALLAPI*)( const SchMemChart&, sal_Int32, String& ))GetFuncSch("SchGetDefaultForColumnText");
/*N*/ 
/*N*/ 	if( fp )
/*N*/ 	{
/*N*/ 		fp( rMemChart, nCol, aResult );
/*N*/ 	}
/*N*/ }

/*N*/ void __EXPORT SchDLL::GetDefaultForRowText( const SchMemChart& rMemChart, sal_Int32 nRow, String& aResult )
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI*fp)( const SchMemChart&, sal_Int32, String& );
/*N*/ 
/*N*/ 	fp = (void (__LOADONCALLAPI*)( const SchMemChart&, sal_Int32, String& ))GetFuncSch("SchGetDefaultForRowText");
/*N*/ 
/*N*/ 	if( fp )
/*N*/ 	{
/*N*/ 		fp( rMemChart, nRow, aResult );
/*N*/ 	}
/*N*/ }

/*N*/ void __EXPORT SchDLL::ConvertChartRangeForWriter( SchMemChart& rMemChart,
/*N*/ 													BOOL bOldToNew )
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI* fp )( SchMemChart&, BOOL )
/*N*/ 		= (void (__LOADONCALLAPI*)( SchMemChart&, BOOL ))
/*N*/ 								GetFuncSch("SchConvertChartRangeForWriter");
/*N*/ 	if( fp )
/*N*/ 		fp( rMemChart, bOldToNew );
/*N*/ }

/*N*/ void __EXPORT SchDLL::MemChartInsertCols( SchMemChart& rMemChart,
/*N*/ 											short nAtCol, short nCount)
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI* fp )( SchMemChart&, short, short )
/*N*/ 		= (void (__LOADONCALLAPI*)( SchMemChart&, short, short ))
/*N*/ 								GetFuncSch("SchMemChartInsertCols");
/*N*/ 	if( fp )
/*N*/ 		fp( rMemChart, nAtCol, nCount );
/*N*/ }
/*?*/ void __EXPORT SchDLL::MemChartRemoveCols( SchMemChart& rMemChart,
/*?*/ 											short nAtCol, short nCount)
/*?*/ {
/*?*/ 	void (__LOADONCALLAPI* fp )( SchMemChart&, short, short )
/*?*/ 		= (void (__LOADONCALLAPI*)( SchMemChart&, short, short ))
/*?*/ 								GetFuncSch("SchMemChartRemoveCols");
/*?*/ 	if( fp )
/*?*/ 		fp( rMemChart, nAtCol, nCount );
/*?*/ }

/*N*/ void __EXPORT SchDLL::MemChartInsertRows( SchMemChart& rMemChart,
/*N*/ 											short nAtRow, short nCount )
/*N*/ {
/*N*/ 	void (__LOADONCALLAPI* fp )( SchMemChart&, short, short )
/*N*/ 		= (void (__LOADONCALLAPI*)( SchMemChart&, short, short ))
/*N*/ 								GetFuncSch("SchMemChartInsertRows");
/*N*/ 	if( fp )
/*N*/ 		fp( rMemChart, nAtRow, nCount );
/*N*/ }
/*?*/ void __EXPORT SchDLL::MemChartRemoveRows( SchMemChart& rMemChart,
/*?*/ 											short nAtRow, short nCount )
/*?*/ {
/*?*/ 	void (__LOADONCALLAPI* fp )( SchMemChart&, short, short )
/*?*/ 		= (void (__LOADONCALLAPI*)( SchMemChart&, short, short ))
/*?*/ 								GetFuncSch("SchMemChartRemoveRows");
/*?*/ 	if( fp )
/*?*/ 		fp( rMemChart, nAtRow, nCount );
/*?*/ }

/*************************************************************************
|*
|* Lade Module von aussen
|*
\************************************************************************/

/*?*/ SfxModule *SchModuleDummy::Load()
/*?*/ {
/*?*/ 	return (LoadLibSch() ? SCH_MOD() : NULL);
/*?*/ }

/*************************************************************************
|*
|* Bestimme die GUID abh. von der Version
|*
\************************************************************************/

/*?*/ const SvGlobalName SchModuleDummy::GetID (USHORT nFileFormat)
/*?*/ {
/*?*/ 	SvGlobalName aName;
/*?*/ 
/*?*/ 	switch (nFileFormat)
/*?*/ 	{
/*?*/ 		case SOFFICE_FILEFORMAT_60:
/*?*/ 		{
/*?*/ 			aName = SvGlobalName(BF_SO3_SCH_CLASSID_60);
/*?*/ 		}
/*?*/ 		break;
/*?*/ 
/*?*/ 		case SOFFICE_FILEFORMAT_50:
/*?*/ 		{
/*?*/ 			aName = SvGlobalName(BF_SO3_SCH_CLASSID_50);
/*?*/ 		}
/*?*/ 		break;
/*?*/ 
/*?*/ 		case SOFFICE_FILEFORMAT_40:
/*?*/ 		{
/*?*/ 			aName = SvGlobalName(BF_SO3_SCH_CLASSID_40);
/*?*/ 		}
/*?*/ 		break;
/*?*/ 
/*?*/ 		case SOFFICE_FILEFORMAT_31:
/*?*/ 		{
/*?*/ 			aName = SvGlobalName(BF_SO3_SCH_CLASSID_30);
/*?*/ 		}
/*?*/ 		break;
/*?*/ 
/*?*/ 		default:
/*?*/ 		{
/*?*/ 			DBG_ERROR( "Unknown file format" );
/*?*/ 		}
/*?*/ 		break;
/*?*/ 	}
/*?*/ 
/*?*/ 	return aName;
/*?*/ }

/*************************************************************************
|*
|* Bestimme die Version aus der GUID
|*
\************************************************************************/

/*?*/ USHORT SchModuleDummy::HasID (const SvGlobalName &rName)
/*?*/ {
/*?*/ 	USHORT nRet = 0;
/*?*/ 
/*?*/ 	if (GetID(SOFFICE_FILEFORMAT_31) == rName)
/*?*/ 	{
/*?*/ 		nRet = SOFFICE_FILEFORMAT_31;
/*?*/ 	}
/*?*/ 	else if (GetID(SOFFICE_FILEFORMAT_40) == rName)
/*?*/ 	{
/*?*/ 		nRet = SOFFICE_FILEFORMAT_40;
/*?*/ 	}
/*?*/ 	else if (GetID(SOFFICE_FILEFORMAT_50) == rName)
/*?*/ 	{
/*?*/ 		nRet = SOFFICE_FILEFORMAT_50;
/*?*/ 	}
/*?*/ 	else if (GetID(SOFFICE_FILEFORMAT_60) == rName)
/*?*/ 	{
/*?*/ 		nRet = SOFFICE_FILEFORMAT_60;
/*?*/ 	}
/*?*/ 
/*?*/ 	return(nRet);
/*?*/ }

}
