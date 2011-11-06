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


#ifndef _DOCTXM_HXX
#define _DOCTXM_HXX


#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

#ifndef _TOX_HXX
#include <tox.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
class  SvUShorts;
class  SvStringsDtor;
class  SvPtrarr;
namespace binfilter {

class  SwTOXInternational;

class  SwTxtNode;
class  SwTxtFmtColl;
struct SwPosition;
struct SwTOXSortTabBase;

typedef SwTOXSortTabBase* SwTOXSortTabBasePtr;

SV_DECL_PTRARR(SwTOXSortTabBases, SwTOXSortTabBasePtr, 0, 5 )//STRIP008 ;

/*--------------------------------------------------------------------
	 Beschreibung: Ring der speziellen Verzeichnisse
 --------------------------------------------------------------------*/

class SwTOXBaseSection : public SwTOXBase, public SwSection
{
	SwTOXSortTabBases aSortArr;

	void	UpdateAll();

	// Sortiert einfuegen ins Array fuer die Generierung

	// Alpha-Trennzeichen bei der Generierung einfuegen

	// Textrumpf generieren
    // OD 18.03.2003 #106329# - add parameter <_TOXSectNdIdx> and <_pDefaultPageDesc>

	// Seitennummerplatzhalter gegen aktuelle Nummern austauschen

	// Bereich fuer Stichwort einfuegen suchen

	// returne die TextCollection ueber den Namen / aus Format-Pool

public:
	SwTOXBaseSection( const SwTOXBase& rBase );
	virtual ~SwTOXBaseSection();

    // OD 19.03.2003 #106329# - add parameter <_bNewTOX> in order to distinguish
    // between the creation of a new table-of-content or an update of
    // a table-of-content. Default value: false
    void Update( const SfxItemSet* pAttr = 0,
                 const bool        _bNewTOX = false ); // Formatieren
	void UpdatePageNum();   			// Seitennummern einfuegen
	TYPEINFO();							// fuers rtti

};
/* -----------------02.09.99 07:52-------------------

 --------------------------------------------------*/
struct SwDefTOXBase_Impl
{
	SwTOXBase* pContBase;
	SwTOXBase* pIdxBase;
	SwTOXBase* pUserBase;
	SwTOXBase* pTblBase;
	SwTOXBase* pObjBase;
	SwTOXBase* pIllBase;
	SwTOXBase* pAuthBase;

	SwDefTOXBase_Impl() :
	pContBase(0),
	pIdxBase(0),
	pUserBase(0),
	pTblBase(0),
	pObjBase(0),
	pIllBase(0),
	pAuthBase(0)
	{}
	~SwDefTOXBase_Impl()
	{
		delete pContBase;
		delete pIdxBase;
		delete pUserBase;
		delete pTblBase;
		delete pObjBase;
		delete pIllBase;
		delete pAuthBase;
	}

};

} //namespace binfilter
#endif	// _DOCTXM_HXX
