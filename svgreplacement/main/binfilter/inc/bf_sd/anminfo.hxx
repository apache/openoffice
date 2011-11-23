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



#ifndef _SD_ANMINFO_HXX
#define _SD_ANMINFO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONEFFECT_HPP_
#include <com/sun/star/presentation/AnimationEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONSPEED_HPP_
#include <com/sun/star/presentation/AnimationSpeed.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_CLICKACTION_HPP_
#include <com/sun/star/presentation/ClickAction.hpp>
#endif

#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _COLOR_HXX //autogen
#include <tools/color.hxx>
#endif
class Polygon;
class Point;
class SvStream;
namespace binfilter {

class SdrObjSurrogate;
class SdrObject;
class SdrPathObj;
class SdDrawDocument;

class SdAnimationInfo : public SdrObjUserData, public SfxListener
{
private:

	SdDrawDocument*			pDoc;

public:

	Polygon*				pPolygon; 		// fuer nichtlinearen Pfad (unbenutzt)
	Point					aStart;			// Startpunkt eines linearen Pfades (unbenutzt)
	Point					aEnd; 			// Endpunkt eines linearen Pfades (unbenutzt)
	::com::sun::star::presentation::AnimationEffect			eEffect;		// Animationseffekt
	::com::sun::star::presentation::AnimationEffect			eTextEffect;	// Animationseffekt fuer Textinhalt
	::com::sun::star::presentation::AnimationSpeed			eSpeed;			// Geschwindigkeit der Animation
	BOOL					bActive;		// eingeschaltet ?
	BOOL					bDimPrevious;	// Objekt abblenden
	BOOL					bIsMovie; 		// wenn Gruppenobjekt, dann Sequenz aus den
	BOOL					bDimHide; 		// verstecken statt abblenden
	Color					aBlueScreen;	// identifiziert "Hintergrundpixel"
	Color					aDimColor;		// zum Abblenden des Objekts
	String					aSoundFile;		// Pfad zum Soundfile in MSDOS-Notation
	BOOL					bSoundOn; 		// Sound ein/aus
	BOOL					bPlayFull;		// Sound ganz abspielen
	SdrObjSurrogate*		pPathSuro;		// Surrogat fuer pPathObj
	SdrPathObj* 			pPathObj; 		// das Pfadobjekt
	::com::sun::star::presentation::ClickAction 			eClickAction; 	// Aktion bei Mausklick
	::com::sun::star::presentation::AnimationEffect			eSecondEffect;	// fuer Objekt ausblenden
	::com::sun::star::presentation::AnimationSpeed			eSecondSpeed; 	// fuer Objekt ausblenden
	String					aSecondSoundFile; // fuer Objekt ausblenden
	BOOL					bSecondSoundOn;	// fuer Objekt ausblenden
	BOOL					bSecondPlayFull;// fuer Objekt ausblenden
	String					aBookmark;		// Sprung zu Objekt/Seite
	USHORT					nVerb;			// fuer OLE-Objekt
	BOOL					bInvisibleInPresentation;
	BOOL					bIsShown; 		// in der Show gerade sichtbar, NICHT PERSISTENT!
	BOOL					bShow;			// Befehl: mit 1. Effekt zeigen (TRUE)
											// oder mit 2. Effekt entfernen (FALSE)
											// NICHT PERSISTENT!
	BOOL					bDimmed;		// in der Show abgeblendet (TRUE) oder
											// nicht (TRUE)
											// NICHT PERSISTENT!
	ULONG					nPresOrder;

							SdAnimationInfo(SdDrawDocument* pTheDoc);
							SdAnimationInfo(const SdAnimationInfo& rAnmInfo);
	virtual					~SdAnimationInfo();

	virtual SdrObjUserData* Clone(SdrObject* pObj) const {DBG_BF_ASSERT(0, "STRIP"); return NULL;} //STRIP001 	virtual SdrObjUserData*	Clone(SdrObject* pObj) const;

	virtual void			WriteData(SvStream& rOut);
	virtual void			ReadData(SvStream& rIn);

			// NULL loest die Verbindung zum Pfadobjekt
	virtual void			AfterRead();
};

} //namespace binfilter
#endif // _SD_ANMINFO_HXX

