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


#ifndef _LINKMGR_HXX
#define _LINKMGR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LINKSRC_HXX
#include <bf_so3/linksrc.hxx>
#endif
#ifndef _TOOLS_STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

namespace binfilter
{

class SvPersist;
class SvBaseLink;
class SvBaseLinkRef;

typedef SvBaseLinkRef* SvBaseLinkRefPtr;
SV_DECL_PTRARR( SvBaseLinks, SvBaseLinkRefPtr, 1, 1 )

typedef SvLinkSource* SvLinkSourcePtr;
SV_DECL_PTRARR( SvLinkSources, SvLinkSourcePtr, 1, 1 )

class SO3_DLLPUBLIC SvLinkManager
{
private:
	SvBaseLinks		aLinkTbl;
	SvLinkSources    aServerTbl;
	SvPersist	    *pPersist; // LinkMgr muss vor SvPersist freigegeben werden
protected:
	BOOL 		InsertLink( SvBaseLink* pLink, USHORT nObjType, USHORT nUpdateType,
							const String* pName = 0 );
public:
				SvLinkManager();
				virtual ~SvLinkManager();

	SvPersist*	GetPersist() const 				{ return pPersist; }
	void        SetPersist( SvPersist * p ) 	{ pPersist = p; }

	void		Remove( SvBaseLink *pLink );
	void		Remove( USHORT nPos, USHORT nCnt = 1 );
	BOOL		Insert( SvBaseLink* pLink );

				// den Link mit einem SvLinkSource verbinden und in die Liste eintragen
	BOOL 		InsertDDELink( SvBaseLink*,
						const String& rServer,
						const String& rTopic,
						const String& rItem );

				// falls am Link schon alles eingestellt ist !
	BOOL 		InsertDDELink( SvBaseLink* );

				// erfrage die Strings fuer den Dialog
	virtual BOOL GetDisplayNames( const SvBaseLink *,
									String* pType,
									String* pFile = 0,
									String* pLink = 0,
									String* pFilter = 0 ) const;

	virtual SvLinkSourceRef CreateObj( SvBaseLink * );

	void 		UpdateAllLinks( BOOL bCallErrHdl,
                                BOOL bUpdateGrfLinks );

				// Liste aller Links erfragen (z.B. fuer Verknuepfungs-Dialog)
	const 		SvBaseLinks& GetLinks() const { return aLinkTbl; }

	// ----------------- Serverseitige Verwaltung --------------------

				// Liste der zu serviereden Links erfragen
	const SvLinkSources& GetServers() const { return aServerTbl; }
				// einen zu servierenden Link eintragen/loeschen
	BOOL 		InsertServer( SvLinkSource* rObj );
	void 		RemoveServer( SvLinkSource* rObj );
	void 		RemoveServer( USHORT nPos, USHORT nCnt = 1 )
				{	aServerTbl.Remove( nPos, nCnt ); }

private:
				// diese Methoden gibts nicht!
	SO3_DLLPRIVATE 			SvLinkManager( const SvLinkManager& );
	SO3_DLLPRIVATE 			SvLinkManager& operator=( const SvLinkManager& );
};

// Trenner im LinkName fuer die DDE-/File-/Grafik- Links
// (nur wer es braucht, um einen SvLinkName zusammenzubasteln)
const sal_Char cTokenSeperator = '\xff';

// erzeuge einen String fuer den SvLinkName. Fuer
// - DDE die ersten 3 Strings, (Server, Topic, Item)
// - File-/Grf-LinkNms die letzen 3 Strings (FileName, Bereich, Filter)
void SO3_DLLPUBLIC MakeLnkName( String& rName,
				 const String* pType,		// kann auch 0 sein !!
				 const String& rFile,
				 const String& rLink,
				 const String* pFilter = 0 );

}

#endif

