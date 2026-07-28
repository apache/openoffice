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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_filter.hxx"

#include <string.h>
#include <vcl/gdimtf.hxx>
#include <vcl/graph.hxx>
#include <vcl/virdev.hxx>
#include <svtools/FilterConfigItem.hxx>
#include "dxf2mtf.hxx"
#include <math.h>

//================== GraphicImport - die exportierte Funktion ================

extern "C" sal_Bool __LOADONCALLAPI GraphicImport(SvStream & rStream, Graphic & rGraphic, FilterConfigItem* pConfigItem, sal_Bool )
{
	DXFRepresentation aDXF;
	DXF2GDIMetaFile aConverter;
	GDIMetaFile aMTF;

	// A DXF may not declare its text encoding ($DWGCODEPAGE). In that case the
	// caller (a higher layer that knows the user's intent) can supply a fallback
	// encoding as a "CharacterSet" filter option; it is applied before reading so
	// that a $DWGCODEPAGE actually present in the file still wins over it.
	// See idxf/cases/case-99892 (Phase B concept) for how this option is meant to
	// be populated. When neither the file nor the caller provides one,
	// DXFRepresentation defaults to the system ANSI encoding.
	if ( pConfigItem )
	{
		sal_Int32 nEncoding = pConfigItem->ReadInt32(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CharacterSet" ) ),
			RTL_TEXTENCODING_DONTKNOW );
		if ( nEncoding != RTL_TEXTENCODING_DONTKNOW )
			aDXF.setTextEncoding( (rtl_TextEncoding)nEncoding );
	}

	if ( aDXF.Read( rStream, 0, 60 ) == sal_False )
		return sal_False;
	if ( aConverter.Convert( aDXF, aMTF, 60, 100 ) == sal_False )
		return sal_False;
	rGraphic=Graphic(aMTF);

	return sal_True;
}
