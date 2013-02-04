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


#ifndef SDTAKITM_HXX
#define SDTAKITM_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

//------------------------------
// class SdrTextAniKindItem
//------------------------------

enum SdrTextAniKind {SDRTEXTANI_NONE,
					 SDRTEXTANI_BLINK,
					 SDRTEXTANI_SCROLL,
					 SDRTEXTANI_ALTERNATE,
					 SDRTEXTANI_SLIDE};

// - SDRTEXTANI_BLINK:
//   Einfach nur Blinken. Direction und Amount ohne Wirkung.
//   Frequenz siehe Delay, 0=0.5Hz (Delay=250).
//   Count=Anzahl der blinker. 0=Endlos.
//   StartInside: FALSE=beginnend mit Pause, TRUE=Beginnend mit Puls
//   StopInside:  FALSE=Nach Count blinkern nicht sichtbar, TRUE=sichtbar
//                (nur wenn Count!=0)
// - SDRTEXTANI_SCROLL:
//   Schrift laeuft rein, laeuft vollstaendig durch. Wenn ganz verschwunden
//   geht's wieder von vorne los.
//   Delay in ms, Sonderfall Delay=0 wird auf 50ms defaulted (20Hz)
//   Count=Durchlaufanzahl (0=Endlos)
//   Direction: ist eben die Richtung in die gescrollt wird.
//   StartInside: FALSE=Schrift wird auch beim ersten Durchlauf reingescrollt
//                TRUE=Der linke Teil der Schrift steht beim ersten Durchlauf
//                     bereits da (im sichtbaren "Scrollbereich")
//   StopInside:  FALSE=Schrift wird auch beim letzten Durchlauf vollstaendig
//                      rausgescrollt
//                TRUE=Schrift wird beim letzten Durchlauf nicht rausgescrollt
//                (nur wenn Count!=0)
//   Amount: Schrittweite in logischen Einheiten. Negative Werte bedeuten
//           Pixel statt log. Einheiten. Wenn Amount=0, dann wird auf
//           1 Pixel defaulted.
// - SDRTEXTANI_ALTERNATE:
//   Wie SDRTEXTANI_SCROLL, nur wird nicht gescrollt bis Schrift ganz
//   verschwunden, sondern bis der letzte Teil vollstaendig sichtbar ist. Dann
//   wird die Richtung umgekehrt und zuruekgescrollt.
//   Count=Anzahl der Durchlaeufe=Anzahl der Richtungswechsel-1
//   Bei Count=1 ist SDRTEXTANI_ALTERNATE also identisch mit SDRTEXTANI_SCROLL
//   Direction ist die Startrichtung
//   Alle andern Parameter wie SDRTEXTANI_SCROLL
// - SDRTEXTANI_SLIDE:
//   Schrift wird nur reingeschoben bis zur Originalposition.
//   -> also wie SCROLL mit StartInside=FALSE, StopInside=TRUE
//   und Count=1. Count=0 wird als Count=1 interpretiert.
//   Fuer jeden Count>1 wird der Text jedoch ruekwaerts wieder vollstaendig
//   rausgeschoben (aehnlich wie ALTERNATE) und dann wieder reingeschoben.
//   StopInside wird nicht ausgewertet, weil immer Inside gestoppt wird.
//   StartInside wird nicht ausgewertet, weil immer Outside gestartet wird.
//   Alle andern Parameter wie SDRTEXTANI_SCROLL
// StartInside,StopInside: Bei TRUE ist die anfaengliche/entgueltige
// Textposition abhaengig von der Textverankerung am Zeichenobjekt. Sie
// entspricht der Textposition bei normalem Paint (ohne Laufschrift).

class SdrTextAniKindItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrTextAniKindItem(SdrTextAniKind eKind=SDRTEXTANI_NONE): SfxEnumItem(SDRATTR_TEXT_ANIKIND,eKind) {}
	SdrTextAniKindItem(SvStream& rIn)                       : SfxEnumItem(SDRATTR_TEXT_ANIKIND,rIn)  {}
	virtual SfxPoolItem*      Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem*      Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT            GetValueCount() const; // { return 5; }
			SdrTextAniKind GetValue() const      { return (SdrTextAniKind)SfxEnumItem::GetValue(); }

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

};

}//end of namespace binfilter
#endif
