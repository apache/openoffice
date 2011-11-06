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



#ifndef _SVX_FMGLOB_HXX
#define _SVX_FMGLOB_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDOBJ_HXX
#include <bf_svx/svdobj.hxx>
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif
namespace binfilter {

const sal_uInt32 FmFormInventor = sal_uInt32('F')*0x00000001+
							  sal_uInt32('M')*0x00000100+
							  sal_uInt32('0')*0x00010000+
							  sal_uInt32('1')*0x01000000;

const sal_uInt16 nOffset = (sal_uInt16)OBJ_UNO;	// Offset, damit Ids nicht mit Ids aus SvDraw kollidieren

const sal_uInt16 OBJ_FM_CONTROL			=	::com::sun::star::form::FormComponentType::CONTROL+nOffset;		// allgemeiner Kennzeichner
																	// fuer FormularKomponenten
const sal_uInt16 OBJ_FM_EDIT			=	::com::sun::star::form::FormComponentType::TEXTFIELD+nOffset;
const sal_uInt16 OBJ_FM_BUTTON			=	::com::sun::star::form::FormComponentType::COMMANDBUTTON+nOffset;
const sal_uInt16 OBJ_FM_FIXEDTEXT		=	::com::sun::star::form::FormComponentType::FIXEDTEXT+nOffset;
const sal_uInt16 OBJ_FM_LISTBOX			=	::com::sun::star::form::FormComponentType::LISTBOX+nOffset;
const sal_uInt16 OBJ_FM_CHECKBOX		=	::com::sun::star::form::FormComponentType::CHECKBOX+nOffset;
const sal_uInt16 OBJ_FM_COMBOBOX		=	::com::sun::star::form::FormComponentType::COMBOBOX+nOffset;
const sal_uInt16 OBJ_FM_RADIOBUTTON		=	::com::sun::star::form::FormComponentType::RADIOBUTTON+nOffset;
const sal_uInt16 OBJ_FM_GROUPBOX		=	::com::sun::star::form::FormComponentType::GROUPBOX+nOffset;
const sal_uInt16 OBJ_FM_GRID			=	::com::sun::star::form::FormComponentType::GRIDCONTROL+nOffset;
const sal_uInt16 OBJ_FM_IMAGEBUTTON		=	::com::sun::star::form::FormComponentType::IMAGEBUTTON+nOffset;
const sal_uInt16 OBJ_FM_FILECONTROL		=	::com::sun::star::form::FormComponentType::FILECONTROL+nOffset;
const sal_uInt16 OBJ_FM_DATEFIELD		=	::com::sun::star::form::FormComponentType::DATEFIELD+nOffset;
const sal_uInt16 OBJ_FM_TIMEFIELD		=	::com::sun::star::form::FormComponentType::TIMEFIELD+nOffset;
const sal_uInt16 OBJ_FM_NUMERICFIELD	=	::com::sun::star::form::FormComponentType::NUMERICFIELD+nOffset;
const sal_uInt16 OBJ_FM_CURRENCYFIELD	=	::com::sun::star::form::FormComponentType::CURRENCYFIELD+nOffset;
const sal_uInt16 OBJ_FM_PATTERNFIELD	=	::com::sun::star::form::FormComponentType::PATTERNFIELD+nOffset;
const sal_uInt16 OBJ_FM_HIDDEN			=	::com::sun::star::form::FormComponentType::HIDDENCONTROL+nOffset;
const sal_uInt16 OBJ_FM_IMAGECONTROL	=	::com::sun::star::form::FormComponentType::IMAGECONTROL+nOffset;
const sal_uInt16 OBJ_FM_FORMATTEDFIELD	=	::com::sun::star::form::FormComponentType::PATTERNFIELD+nOffset+1;
	// keine eigene ID fuer formatted field, sondern groesste bekannte + 1

}//end of namespace binfilter
#endif      // _FM_FMGLOB_HXX

