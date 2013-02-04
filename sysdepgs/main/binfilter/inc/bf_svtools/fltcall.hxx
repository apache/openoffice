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



#ifndef _FLTCALL_HXX
#define _FLTCALL_HXX
#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX
#include <tools/gen.hxx>
#endif
#include <vcl/field.hxx>

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _FILTER_CONFIG_ITEM_HXX_
#include <bf_svtools/FilterConfigItem.hxx>
#endif

class FilterConfigItem;
class SvStream;
class Graphic;
class Window;

namespace binfilter
{

struct FltCallDialogParameter
{

	Window*		pWindow;
	ResMgr*		pResMgr;
	FieldUnit	eFieldUnit;
	String		aFilterExt;

	// In and Out PropertySequence for all filter dialogs
	::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aFilterData;

	FltCallDialogParameter( Window* pW, ResMgr* pRsMgr, FieldUnit eFiUni ) :
		pWindow			( pW ),
		pResMgr			( pRsMgr ),
		eFieldUnit		( eFiUni ) {};
};

typedef BOOL (*PFilterCall)(SvStream & rStream, Graphic & rGraphic,
								FilterConfigItem* pConfigItem, sal_Bool bPrefDialog);
	// Von diesem Typ sind sowohl Export-Filter-Funktionen als auch Import-Filter-Funktionen.
	// rFileName ist der komplette Pfadname der zu importierenden bzw. zu exportierenden Datei.
	// pCallBack darf auch NULL sein. pCallerData wird der Callback-Funktion uebergeben.
	// pOptionsConfig darf NULL sein. Anderenfalls ist die Gruppe des Config schon gesetzt
	// und darf von dem Filter nicht geaendert werden!
	// Wenn bPrefDialog==TRUE gilt, wird ggf. ein Preferences-Dialog durchgefuehrt.

typedef BOOL ( *PFilterDlgCall )( FltCallDialogParameter& );
	// Von diesem Typ sind sowohl Export-Filter-Funktionen als auch Import-Filter-Funktionen.
	// Uebergeben wird ein Pointer auf ein Parent-Fenster und auf die Options-Config.
	// pOptions und pWindow duerfen NULL sein, in diesem Fall wird FALSE zurueckgeliefert.
	// Anderenfalls ist die Gruppe der Config schon gesetzt
	// und darf von dem Filter nicht geaendert werden!

}

#endif
