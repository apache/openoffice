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


#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif


#ifndef _CMDID_H
#include <cmdid.h>
#endif
namespace binfilter {


/******************************************************************************
 *	Methode		:	SwDocFac::SwDocFac( SwDoc *pDoc )
 *	Beschreibung:
 *	Erstellt	:	OK 01-24-94 11:32am
 *	Aenderung	:	OK 01-24-94 11:32am
 ******************************************************************************/


/*N*/ SwDocFac::SwDocFac( SwDoc *pDc )
/*N*/ 	: pDoc( pDc )
/*N*/ {
/*N*/ 	if( pDoc )
/*N*/ 		pDoc->AddLink();
/*N*/ }

/******************************************************************************
 *	Methode		:   SwDocFac::~SwDocFac()
 *	Beschreibung:
 *	Erstellt	:	OK 01-24-94 11:33am
 *	Aenderung	:	OK 01-24-94 11:33am
 ******************************************************************************/


/*N*/ SwDocFac::~SwDocFac()
/*N*/ {
/*N*/ 	if( pDoc && !pDoc->RemoveLink() )
/*?*/ 		delete pDoc;
/*N*/ }

/******************************************************************************
 *	Methode		:	SwDoc *SwDocFac::GetDoc()
 *	Beschreibung:	Diese Methode legt immer einen Drucker an.
 *	Erstellt	:	OK 01-24-94 11:34am
 *	Aenderung	:	OK 01-24-94 11:34am
 ******************************************************************************/


/*N*/ SwDoc *SwDocFac::GetDoc()
/*N*/ {
/*N*/ 	if( !pDoc )
/*N*/ 	{
/*N*/ 		pDoc = new SwDoc;
/*N*/ 		pDoc->AddLink();
/*N*/ 	}
/*N*/ 	return pDoc;
/*N*/ }


/******************************************************************************
 *  Erstellt    :   JP 01-27-94 11:37am
 *  Aenderung   :   JP 01-27-94 11:37am
 ******************************************************************************/



}
