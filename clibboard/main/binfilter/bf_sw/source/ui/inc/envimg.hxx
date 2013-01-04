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


#ifndef _ENVIMG_HXX
#define _ENVIMG_HXX

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

// enum -----------------------------------------------------------------------

enum SwEnvAlign
{
	ENV_HOR_LEFT = 0,
	ENV_HOR_CNTR,
	ENV_HOR_RGHT,
	ENV_VER_LEFT,
	ENV_VER_CNTR,
	ENV_VER_RGHT
};

// class SwEnvItem ------------------------------------------------------------

class SwEnvItem : public SfxPoolItem
{
public:

	::rtl::OUString   aAddrText;       // Text fuer Empfaenger
	sal_Bool      	bSend;           // Absender?
	::rtl::OUString  	aSendText;       // Text fuer Absender
	sal_Int32       lAddrFromLeft;   // Linker Abstand fuer Empfaenger (twips)
	sal_Int32       lAddrFromTop;    // Oberer Abstand fuer Empfaenger (twips)
	sal_Int32       lSendFromLeft;   // Linker Abstand fuer Absender   (twips)
	sal_Int32       lSendFromTop;    // Oberer Abstand fuer Absender   (twips)
	sal_Int32       lWidth;          // Breite des Umschlags           (twips)
	sal_Int32       lHeight;         // Hoehe  des Umschlags           (twips)
	SwEnvAlign 		eAlign;          // Ausrichtung beim Einzug
	sal_Bool  		bPrintFromAbove; // Von oben drucken?
	sal_Int32       lShiftRight;     // Verschiebung nach rechts       (twips)
	sal_Int32       lShiftDown;      // Verschiebung nach unten        (twips)

	SwEnvItem();
	SwEnvItem(const SwEnvItem& rItem);

    TYPEINFO();


	virtual int operator ==(const SfxPoolItem& rItem) const;

	virtual SfxPoolItem*     Clone(SfxItemPool* = 0) const;
};

// class SwEnvCfgItem -------------------------------------------------------



} //namespace binfilter
#endif

