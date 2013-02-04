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


#ifndef _SVX_FMTOOLS_HXX
#define _SVX_FMTOOLS_HXX

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTION_HPP_
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XINTERCEPTORINFO_HPP_
#include <com/sun/star/frame/XInterceptorInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif


#ifndef _SVSTDARR_ULONGS
#define _SVSTDARR_ULONGS
#endif

#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_H_
#include <cppuhelper/interfacecontainer.h>
#endif
#ifndef _CPPUHELPER_COMPBASE3_HXX_
#include <cppuhelper/compbase3.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _CPPUHELPER_COMPONENT_HXX_
#include <cppuhelper/component.hxx>
#endif

#ifndef _FM_IMPLEMENTATION_IDS_HXX_
#include "fmimplids.hxx"
#endif

class Window;
namespace binfilter {



//==================================================================
// allgemeine Typen
//==================================================================
// displaying a database exception for the user
// display info about a simple ::com::sun::star::sdbc::SQLException
// display info about chained SQLExceptions

// StringConversion
namespace svxform
{
	// modes for the XModeSelector
	DECLARE_CONSTASCII_USTRING(DATA_MODE);
	DECLARE_CONSTASCII_USTRING(FILTER_MODE);

}	// namespace svxform


// Kopieren von Persistenten Objecten


// Suchen in einer Indexliste nach einem Element

sal_Int32 getElementPos(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& xCont, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& xElement);




// ===================================================================================================
// = class CursorWrapper - eine Hilfsklasse, die parallel mit je einem ::com::sun::star::uno::Reference<XDatabaseUpdateCursor>,
//							XDatabaseBookmarkCursor und XDatabaseDirectCursor arbeitet
// ===================================================================================================



}//end of namespace binfilter
#endif // _SVX_FMTOOLS_HXX

