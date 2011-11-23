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


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>		// GetCurFld
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _PAM_HXX
#include "pam.hxx"
#endif

#ifndef _DOC_HXX
#include "doc.hxx"
#endif

namespace binfilter {
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::rtl;


// wenn Selektion groesser Max Nodes oder mehr als Max Selektionen
// => keine Attribute
static const USHORT nMaxLookup = 40;

/*--------------------------------------------------------------------
	Beschreibung: Feldtypen zu einer ResId zaehlen
				  wenn 0 alle zaehlen
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung: Feldtypen zu einer ResId finden
				  wenn 0 alle finden
 --------------------------------------------------------------------*/

/*--------------------------------------------------------------------
	Beschreibung: Den ersten Typen mit ResId und Namen finden
 --------------------------------------------------------------------*/

/*--------------------------------------------------------------------
	Beschreibung: Feldtypen loeschen
 --------------------------------------------------------------------*/

/*--------------------------------------------------------------------
	Beschreibung: FieldType ueber Name loeschen
 --------------------------------------------------------------------*/



/*************************************************************************
|*
|*					SwEditShell::Insert( SwField )
|*
|*	  Beschreibung	an der Cursorposition ein Feld einfuegen
|*	  Quelle:		vgl. SwEditShell::Insert( String )
|*
*************************************************************************/


/*N*/ inline SwTxtFld *GetDocTxtFld( const SwPosition* pPos )
/*N*/ {
/*N*/ 	SwTxtNode *pNode = pPos->nNode.GetNode().GetTxtNode();
/*N*/ 	if( pNode )
/*N*/ 		return pNode->GetTxtFld( pPos->nContent );
/*N*/ 	else
/*N*/ 		return 0;
/*N*/ }

}
