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





#include "svdotext.hxx"
#include "svdmodel.hxx"
#include "svdio.hxx"

#ifndef SVX_LIGHT
#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
#endif

#ifndef _SVXLINKMGR_HXX //autogen
#include <linkmgr.hxx>
#endif


#include <bf_svtools/urihelper.hxx>

// #90477#
#ifndef _TOOLS_TENCCVT_HXX
#include <tools/tenccvt.hxx>
#endif
#include "bf_so3/staticbaseurl.hxx"
namespace binfilter {

#ifndef SVX_LIGHT
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@  @@@@@  @@@@@@  @@@@@@ @@@@@@ @@  @@ @@@@@@  @@    @@ @@  @@ @@  @@
// @@  @@ @@  @@     @@    @@   @@      @@@@    @@    @@    @@ @@@ @@ @@ @@
// @@  @@ @@@@@      @@    @@   @@@@@    @@     @@    @@    @@ @@@@@@ @@@@
// @@  @@ @@  @@ @@  @@    @@   @@      @@@@    @@    @@    @@ @@ @@@ @@ @@
//  @@@@  @@@@@   @@@@     @@   @@@@@@ @@  @@   @@    @@@@@ @@ @@  @@ @@  @@
//
// ImpSdrObjTextLink zur Verbindung von SdrTextObj und LinkManager
//
// Einem solchen Link merke ich mir als SdrObjUserData am Objekt. Im Gegensatz
// zum Grafik-Link werden die ObjektDaten jedoch kopiert (fuer Paint, etc.).
// Die Information ob das Objekt ein Link ist besteht genau darin, dass dem
// Objekt ein entsprechender UserData-Record angehaengt ist oder nicht.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/*?*/ class ImpSdrObjTextLink: public ::binfilter::SvBaseLink
/*?*/ {
/*?*/ 	SdrTextObj*					pSdrObj;
/*?*/ 
/*?*/ public:
/*?*/ 	ImpSdrObjTextLink( SdrTextObj* pObj1 )
/*?*/ 		: ::binfilter::SvBaseLink( ::binfilter::LINKUPDATE_ONCALL, FORMAT_FILE ),
/*?*/ 			pSdrObj( pObj1 )
/*?*/ 	{}
/*?*/ 	virtual ~ImpSdrObjTextLink();
/*?*/ 
/*?*/ 	virtual void Closed();
/*?*/ 	virtual void DataChanged( const String& rMimeType,
/*?*/ 								const ::com::sun::star::uno::Any & rValue );
/*?*/ 
/*?*/ 	BOOL Connect() { return 0 != SvBaseLink::GetRealObject(); }
/*?*/ };

/*?*/ ImpSdrObjTextLink::~ImpSdrObjTextLink()
/*?*/ {
/*?*/ }

/*?*/ void ImpSdrObjTextLink::Closed()
/*?*/ {
/*?*/ 	if (pSdrObj )
/*?*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 	}
/*?*/ 	SvBaseLink::Closed();
/*?*/ }


/*?*/ void ImpSdrObjTextLink::DataChanged( const String& rMimeType,
/*?*/ 								const ::com::sun::star::uno::Any & rValue )
/*?*/ {
/*?*/ 	FASTBOOL bForceReload=FALSE;
/*?*/ 	SdrModel* pModel = pSdrObj ? pSdrObj->GetModel() : 0;
/*?*/ 	SvxLinkManager* pLinkManager= pModel ? pModel->GetLinkManager() : 0;
/*?*/ 	if( pLinkManager )
/*?*/ 	{
/*?*/ 		ImpSdrObjTextLinkUserData* pData=pSdrObj->GetLinkUserData();
/*?*/ 		if( pData )
/*?*/ 		{
/*?*/ 			String aFile;
/*?*/ 			String aFilter;
/*?*/ 			pLinkManager->GetDisplayNames( this, 0,&aFile, 0, &aFilter );
/*?*/ 
/*?*/ 			if( !pData->aFileName.Equals( aFile ) ||
/*?*/ 				!pData->aFilterName.Equals( aFilter ))
/*?*/ 			{
/*?*/ 				pData->aFileName = aFile;
/*?*/ 				pData->aFilterName = aFilter;
/*?*/ 				pSdrObj->SetChanged();
/*?*/ 				bForceReload = TRUE;
/*?*/ 			}
/*?*/ 		}
/*?*/ 	}
/*?*/ 	if (pSdrObj )
/*?*/ 	{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 	pSdrObj->ReloadLinkedText( bForceReload );
/*?*/ }
#endif // SVX_LIGHT

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// @@    @@ @@  @@ @@  @@  @@  @@  @@@@@ @@@@@@ @@@@@   @@@@@   @@@@  @@@@@@  @@@@
// @@    @@ @@@ @@ @@ @@   @@  @@ @@     @@     @@  @@  @@  @@ @@  @@   @@   @@  @@
// @@    @@ @@@@@@ @@@@    @@  @@  @@@@  @@@@@  @@@@@   @@  @@ @@@@@@   @@   @@@@@@
// @@    @@ @@ @@@ @@@@@   @@  @@     @@ @@     @@  @@  @@  @@ @@  @@   @@   @@  @@
// @@@@@ @@ @@  @@ @@  @@   @@@@  @@@@@  @@@@@@ @@  @@  @@@@@  @@  @@   @@   @@  @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ TYPEINIT1(ImpSdrObjTextLinkUserData,SdrObjUserData);

/*N*/ ImpSdrObjTextLinkUserData::ImpSdrObjTextLinkUserData(SdrTextObj* pObj1):
/*N*/ 	SdrObjUserData(SdrInventor,SDRUSERDATA_OBJTEXTLINK,0),
/*N*/ 	pObj(pObj1),
/*N*/ 	pLink(NULL),
/*N*/ 	eCharSet(RTL_TEXTENCODING_DONTKNOW)
/*N*/ {
/*N*/ }

/*N*/ ImpSdrObjTextLinkUserData::~ImpSdrObjTextLinkUserData()
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*?*/ DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	delete pLink;
/*N*/ #endif
/*N*/ }

/*N*/ SdrObjUserData* ImpSdrObjTextLinkUserData::Clone(SdrObject* pObj1) const
/*N*/ {
/*N*/ 	ImpSdrObjTextLinkUserData* pData=new ImpSdrObjTextLinkUserData((SdrTextObj*)pObj1);
/*N*/ 	pData->aFileName  =aFileName;
/*N*/ 	pData->aFilterName=aFilterName;
/*N*/ 	pData->aFileDate0 =aFileDate0;
/*N*/ 	pData->eCharSet   =eCharSet;
/*N*/ 	pData->pLink=NULL;
/*N*/ 	return pData;
/*N*/ }

/*N*/ void ImpSdrObjTextLinkUserData::WriteData(SvStream& rOut)
/*N*/ {
/*N*/ 	SdrObjUserData::WriteData(rOut);
/*N*/ 	// Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*N*/ 	SdrDownCompat aCompat(rOut, STREAM_WRITE);
/*N*/ 
/*N*/ #ifdef DBG_UTIL
/*N*/ 	aCompat.SetID("ImpSdrObjTextLinkUserData");
/*N*/ #endif
/*N*/ 
/*N*/ 	String aRelFileName;
/*N*/ 
/*N*/ 	if( aFileName.Len() )
/*N*/ 	{
/*N*/ 		aRelFileName = ::binfilter::StaticBaseUrl::AbsToRel( aFileName,
/*N*/ 												INetURLObject::WAS_ENCODED,
/*N*/ 												INetURLObject::DECODE_UNAMBIGUOUS );
/*N*/ 	}
/*N*/ 
/*N*/ 	rOut.WriteByteString( aRelFileName );
/*N*/ 
/*N*/ 	// UNICODE: rOut << aFilterName;
/*N*/ 	rOut.WriteByteString(aFilterName);
/*N*/ 
/*N*/ 	// #90477# rOut << UINT16(GetStoreCharSet(eCharSet));
/*N*/ 	rOut << UINT16(GetSOStoreTextEncoding(eCharSet, (sal_uInt16)rOut.GetVersion()));
/*N*/ 
/*N*/ 	rOut << UINT32(aFileDate0.GetDate());
/*N*/ 	rOut << UINT32(aFileDate0.GetTime());
/*N*/ }

/*N*/ void ImpSdrObjTextLinkUserData::ReadData(SvStream& rIn)
/*N*/ {
/*N*/ 	SdrObjUserData::ReadData(rIn);
/*N*/ 	// Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*N*/ 	SdrDownCompat aCompat(rIn, STREAM_READ);
/*N*/ 
/*N*/ #ifdef DBG_UTIL
/*N*/ 	aCompat.SetID("ImpSdrObjTextLinkUserData");
/*N*/ #endif
/*N*/ 
/*N*/ 	UINT32 nTmp32;
/*N*/ 	UINT16 nTmp16;
/*N*/ 	String aFileNameRel;
/*N*/ 
/*N*/ 	rIn.ReadByteString(aFileNameRel);
/*N*/ 
/*N*/ 	if( aFileNameRel.Len() )
/*N*/ 	{
/*N*/ 
/*N*/ 		aFileName = ::binfilter::StaticBaseUrl::SmartRelToAbs( aFileNameRel, FALSE,
/*N*/ 												INetURLObject::WAS_ENCODED,
/*N*/ 												INetURLObject::DECODE_UNAMBIGUOUS );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		aFileName.Erase();
/*N*/ 
/*N*/ 	// UNICODE: rIn >> aFilterName;
/*N*/ 	rIn.ReadByteString(aFilterName);
/*N*/ 
/*N*/ 	// #90477# rIn >> nTmp16; eCharSet = rtl_TextEncoding(nTmp16);
/*N*/ 	rIn >> nTmp16; 
/*N*/ 	eCharSet = (rtl_TextEncoding)GetSOLoadTextEncoding((rtl_TextEncoding)nTmp16, (sal_uInt16)rIn.GetVersion());
/*N*/ 
/*N*/ 	rIn >> nTmp32; aFileDate0.SetDate(nTmp32);
/*N*/ 	rIn >> nTmp32; aFileDate0.SetTime(nTmp32);
/*N*/ }

/*N*/ void ImpSdrObjTextLinkUserData::AfterRead()
/*N*/ {
/*N*/ 	if (pObj!=NULL) {
/*N*/ 		pObj->ImpLinkAnmeldung();
/*N*/ 		// lt. Anweisung von MB kein automatisches Reload mehr
/*N*/ 		//pObj->ReloadLinkedText();
/*N*/ 	}
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@@ @@@@@ @@   @@ @@@@@@  @@@@  @@@@@  @@@@@@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@     @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@@@    @@@     @@   @@  @@ @@@@@      @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@ @@  @@
//    @@   @@@@@ @@   @@   @@    @@@@  @@@@@   @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////





/*N*/ ImpSdrObjTextLinkUserData* SdrTextObj::GetLinkUserData() const
/*N*/ {
/*N*/ 	ImpSdrObjTextLinkUserData* pData=NULL;
/*N*/ 	USHORT nAnz=GetUserDataCount();
/*N*/ 	for (USHORT nNum=nAnz; nNum>0 && pData==NULL;) {
/*N*/ 		nNum--;
/*N*/ 		pData=(ImpSdrObjTextLinkUserData*)GetUserData(nNum);
/*N*/ 		if (pData->GetInventor()!=SdrInventor || pData->GetId()!=SDRUSERDATA_OBJTEXTLINK) {
/*N*/ 			pData=NULL;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return pData;
/*N*/ }

/*N*/ void SdrTextObj::ImpLinkAnmeldung()
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	ImpSdrObjTextLinkUserData* pData=GetLinkUserData();
/*N*/ 	SvxLinkManager* pLinkManager=pModel!=NULL ? pModel->GetLinkManager() : NULL;
/*N*/ 	if (pLinkManager!=NULL && pData!=NULL && pData->pLink==NULL) { // Nicht 2x Anmelden
/*N*/ 		pData->pLink=new ImpSdrObjTextLink(this);
/*N*/ #ifdef GCC
/*N*/ 		pLinkManager->InsertFileLink(*pData->pLink,OBJECT_CLIENT_FILE,pData->aFileName,
/*N*/ 									 pData->aFilterName.Len() ?
/*N*/ 									  &pData->aFilterName : (const String *)NULL,
/*N*/ 									 (const String *)NULL);
/*N*/ #else
/*N*/ 		pLinkManager->InsertFileLink(*pData->pLink,OBJECT_CLIENT_FILE,pData->aFileName,
/*N*/ 									 pData->aFilterName.Len() ? &pData->aFilterName : NULL,NULL);
/*N*/ #endif
/*N*/ 		pData->pLink->Connect();
/*N*/ 	}
/*N*/ #endif // SVX_LIGHT
/*N*/ }

/*N*/ void SdrTextObj::ImpLinkAbmeldung()
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	ImpSdrObjTextLinkUserData* pData=GetLinkUserData();
/*N*/ 	SvxLinkManager* pLinkManager=pModel!=NULL ? pModel->GetLinkManager() : NULL;
/*N*/ 	if (pLinkManager!=NULL && pData!=NULL && pData->pLink!=NULL) { // Nicht 2x Abmelden
/*N*/ 		// Bei Remove wird *pLink implizit deleted
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	pLinkManager->Remove( pData->pLink );
/*N*/ 	}
/*N*/ #endif // SVX_LIGHT
/*N*/ }

}
