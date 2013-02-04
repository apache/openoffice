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


#ifndef _SVX_DLGUTIL_HXX
#define _SVX_DLGUTIL_HXX

// include ---------------------------------------------------------------
#ifndef _FIELD_HXX
#include <vcl/field.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif
namespace binfilter {

// macro -----------------------------------------------------------------

// typedef ---------------------------------------------------------------


// Functions -------------------------------------------------------------

// HM- und LanguageStrings aus der Resource laden

// FieldUnit im MetricField oder -Box umsetzen

FieldUnit GetModuleFieldUnit( const SfxItemSet* pSet = NULL );

// Metriken umrechnen
long 		CalcToUnit( float nIn, SfxMapUnit eUnit );
long 		CalcToPoint( long nIn, SfxMapUnit eUnit, USHORT nFaktor );

long		ItemToControl( long nIn, SfxMapUnit eItem, SfxFieldUnit eCtrl );




// to Twips

// to CM

// to MM

// to Inch

// to Point

// To Pica

// generische Wandlung

// replacement of the SfxIniManager

#define OUTPUT_DRAWMODE_COLOR		(DRAWMODE_DEFAULT)
#define OUTPUT_DRAWMODE_CONTRAST	(DRAWMODE_SETTINGSLINE | DRAWMODE_SETTINGSFILL | DRAWMODE_SETTINGSTEXT | DRAWMODE_SETTINGSGRADIENT)

}//end of namespace binfilter
#endif

