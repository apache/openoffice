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

#define ENABLE_BYTESTRING_STREAM_OPERATORS

#ifndef _SVX_FMPAGE_HXX
#include "fmpage.hxx"
#endif



#ifndef _SVX_FMMODEL_HXX
#include "fmmodel.hxx"
#endif

#ifndef SVX_LIGHT
#endif

#ifndef SVX_LIGHT
#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif
#endif



#ifndef SVX_LIGHT
#ifndef _SVX_FMPGEIMP_HXX
#include "fmpgeimp.hxx"
#endif
#endif

#ifndef _SVDIO_HXX //autogen
#include "svdio.hxx"
#endif

#ifndef SVX_LIGHT
#endif






#ifndef SVX_LIGHT
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
using namespace ::binfilter::svxform;//STRIP008 using namespace ::svxform;
#endif

namespace binfilter {


/*N*/ TYPEINIT1(FmFormPage, SdrPage);

//------------------------------------------------------------------
/*N*/ FmFormPage::FmFormPage(FmFormModel& rModel, StarBASIC* _pBasic, FASTBOOL bMasterPage)
/*N*/ 		   :SdrPage(rModel, bMasterPage)
/*N*/ 		   ,pBasic(_pBasic)
/*N*/ #ifndef SVX_LIGHT
/*N*/ 		   ,pImpl(new FmFormPageImpl(this))
/*N*/ #else
/*N*/ 		   ,pImpl(NULL)
/*N*/ #endif
/*N*/ {
/*N*/ }

//------------------------------------------------------------------

//------------------------------------------------------------------
/*N*/ FmFormPage::~FmFormPage()
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	delete pImpl;
/*N*/ #endif
/*N*/ }

//------------------------------------------------------------------
/*N*/ void FmFormPage::WriteData(SvStream& rOut) const
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	{
/*N*/ 		{
/*N*/ 			SdrDownCompat aVCCompat1( rOut, STREAM_WRITE );
/*N*/ 			sal_uInt16 n = 0;
/*N*/ 			rOut << n;
/*N*/ 		}
/*N*/ 		SdrPage::WriteData( rOut );
/*N*/ 		SdrDownCompat aVCCompat2( rOut, STREAM_WRITE );
/*N*/ 
/*N*/ 		rOut << ByteString(aPageName, gsl_getSystemTextEncoding());
/*N*/ 		rOut << (sal_uInt32)0x11051967;
/*N*/ 		rOut << (sal_uInt32)0x19670511;
/*N*/ 		sal_uInt16 nVer = 1;
/*N*/ 		rOut << nVer;
/*N*/ 		{
/*N*/ 			SdrDownCompat aVCCompat3( rOut, STREAM_WRITE);
/*N*/ 			sal_uInt32 nFormCount = 0;
/*N*/ 			rOut << nFormCount;
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	// dont use the flag in that way: if (rOut.GetVersion() >= SOFFICE_FILEFORMAT_40)
/*N*/ 	if (rOut.GetVersion() >= 3830)
/*N*/ 	{
/*N*/ 		SdrDownCompat aCompat(rOut, STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*N*/ 		pImpl->WriteData(rOut);
/*N*/ 	}
/*N*/ #else
/*N*/ 	DBG_ERROR( "FmFormPage::WriteData: not to be called in SVX_LIGHT version!" );
/*N*/ #endif
/*N*/ }

//------------------------------------------------------------------
/*N*/ void FmFormPage::ReadData(const SdrIOHeader& rHead, SvStream& rIn)
/*N*/ {
/*N*/ 	{
/*N*/ 		{
/*N*/ 			SdrDownCompat aVCCompat1( rIn, STREAM_READ );
/*N*/ 		}
/*N*/ 		SdrPage::ReadData( rHead, rIn );
/*N*/ 		{
/*N*/ 			SdrDownCompat aVCCompat2( rIn, STREAM_READ );
/*N*/ 			ByteString aByteStringName;
/*N*/ 			rIn >> aByteStringName;
/*N*/ 			aPageName = String(aByteStringName, gsl_getSystemTextEncoding());
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	// dont use the flag in that way: if (rIn.GetVersion() >= SOFFICE_FILEFORMAT_40)
/*N*/ 	if (rIn.GetVersion() >= 3830 && rHead.GetVersion() >=14)
/*N*/ 	{
/*N*/ 		SdrDownCompat aCompat(rIn, STREAM_READ);	// Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*N*/ #ifndef SVX_LIGHT
/*N*/ 		DBG_ASSERT( aCompat.GetBytesLeft(), "FmFormPage::ReadData: invalid file format!" );
/*N*/ 		if ( aCompat.GetBytesLeft() )
/*N*/ 			pImpl->ReadData(rHead, rIn);
/*N*/ 		// some old (corrupted) versions between 511 and 554 wrote an empty block here - and some of these documents
/*N*/ 		// are still out there
/*N*/ 		// So we allow for such an empty block ...
/*N*/ #endif
/*N*/ 	}
/*N*/ }

//------------------------------------------------------------------
/*N*/ void FmFormPage::SetModel(SdrModel* pNewModel)
/*N*/ {
/*N*/ 	SdrPage::SetModel( pNewModel );
/*N*/ }

//------------------------------------------------------------------
/*N*/ SdrPage* FmFormPage::Clone() const
/*N*/ {
/*N*/ 	return new FmFormPage(*this);
/*N*/ 	// hier fehlt noch ein kopieren der Objekte
/*N*/ }

//------------------------------------------------------------------
/*N*/ void FmFormPage::InsertObject(SdrObject* pObj, ULONG nPos,
/*N*/ 							  const SdrInsertReason* pReason)
/*N*/ {
/*N*/ 	SdrPage::InsertObject( pObj, nPos, pReason );
/*N*/ }

//------------------------------------------------------------------
/*N*/ const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer > & FmFormPage::GetForms() const
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	return pImpl->getForms();
/*N*/ #else
/*N*/ 	static ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >  aRef;
/*N*/ 	return aRef;
/*N*/ #endif
/*N*/ }

//------------------------------------------------------------------
/*N*/ SdrObject* FmFormPage::RemoveObject(ULONG nObjNum)
/*N*/ {
/*N*/ 	SdrObject* pObj = SdrPage::RemoveObject(nObjNum);
/*N*/ 	return pObj;
/*N*/ }

}
