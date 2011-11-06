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


#ifndef _SDTFSITM_HXX
#define _SDTFSITM_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

enum SdrFitToSizeType {SDRTEXTFIT_NONE,         // - kein FitToSize
					   SDRTEXTFIT_PROPORTIONAL, // - Alle Buchstaben proportional umgroessern
					   SDRTEXTFIT_ALLLINES,     // - Zus. jede Zeile separat in der Breite stretchen
					   SDRTEXTFIT_RESIZEATTR};  // - Bei Rahmenumgroesserung (ausser Autogrow) wird
												//   die Schriftgroesse umattributiert (hart)

// Bei SDRTEXTFIT_PROPORTIONAL und SDRTEXTFIT_ALLLINES gibt es kein AutoGrow und
// keine automatischen Umbrueche.
// Ist SDRTEXTFIT_RESIZEATTR gesetzt, so wird beim umgroessern des Textrahmens
// (ausser bei AutoGrow) die Schrift durch harte Attributierung ebenfalls
// umgegroessert.
// Bei AutoGrowingWidth gibt es ebenfalls keine automatischen Umbrueche (erst bei
// TextMaxFrameWidth).

//--------------------------------
// class SdrTextFitToSizeTypeItem
//--------------------------------
class SdrTextFitToSizeTypeItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrTextFitToSizeTypeItem(SdrFitToSizeType eFit=SDRTEXTFIT_NONE): SfxEnumItem(SDRATTR_TEXT_FITTOSIZE,eFit) {}
	SdrTextFitToSizeTypeItem(SvStream& rIn)                        : SfxEnumItem(SDRATTR_TEXT_FITTOSIZE,rIn)  {}
	virtual SfxPoolItem*     Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem*     Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT           GetValueCount() const; // { return 4; }
			SdrFitToSizeType GetValue() const      { return (SdrFitToSizeType)SfxEnumItem::GetValue(); }

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

};

}//end of namespace binfilter
#endif
