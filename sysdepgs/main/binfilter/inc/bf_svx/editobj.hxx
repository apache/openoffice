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



#ifndef _EDITOBJ_HXX
#define _EDITOBJ_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _RSCSFX_HXX
#include <rsc/rscsfx.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif
#ifndef _EEITEM_HXX 
#include <bf_svx/eeitem.hxx>
#endif
namespace binfilter {

class SfxItemPool;
class SfxStyleSheetPool; 

DBG_NAMEEX( EE_EditTextObject )//STRIP008

class SvxFieldItem;
class EECharAttribArray; 

#define EDTOBJ_SETTINGS_ULITEMSUMMATION		0x00000001
#define EDTOBJ_SETTINGS_ULITEMFIRSTPARA		0x00000002

class EditTextObject
{
private:
	USHORT				nWhich;

protected:
						EditTextObject( USHORT nWhich );
						EditTextObject( const EditTextObject& r );

	virtual void		StoreData( SvStream& rOStream ) const;
	virtual void		CreateData( SvStream& rIStream );

public:
	virtual				~EditTextObject();

	USHORT				Which() const { return nWhich; }

	virtual USHORT		GetUserType() const;	// Fuer OutlinerMode, der kann das aber nicht kompatibel speichern
	virtual void		SetUserType( USHORT n );


	virtual	BOOL		IsVertical() const;
	virtual
	void				SetVertical( BOOL bVertical );

	virtual USHORT		GetVersion() const;	// Solange der Outliner keine Recordlaenge speichert

	virtual EditTextObject*	Clone() const = 0;

	BOOL					Store( SvStream& rOStream ) const;
	static EditTextObject*	Create( SvStream& rIStream,
								SfxItemPool* pGlobalTextObjectPool = 0 );

	// Zur 5.1 hat sich die Bedeutung des LRSpaceItems fuer den Outliner geaendert...
	virtual void			AdjustImportedLRSpaceItems( BOOL bTurnOfBullets );
	virtual	void 			PrepareStore( SfxStyleSheetPool* pStyleSheetPool );
	virtual	void 			FinishStore();
	virtual	void 			FinishLoad( SfxStyleSheetPool* pStyleSheetPool );

	virtual USHORT		GetParagraphCount() const;

	virtual XubString	GetText( USHORT nParagraph ) const;
	virtual void		Insert( const EditTextObject& rObj, USHORT nPara );

	virtual void		ClearPortionInfo();




	virtual	void		MergeParaAttribs( const SfxItemSet& rAttribs, USHORT nStart = EE_CHAR_START, USHORT nEnd = EE_CHAR_END );

	virtual BOOL		HasField( TypeId aType = NULL ) const;

	virtual SfxItemSet	GetParaAttribs( USHORT nPara ) const;

	virtual void		GetStyleSheet( USHORT nPara, XubString& rName, SfxStyleFamily& eFamily ) const;
	virtual void		SetStyleSheet( USHORT nPara, const XubString& rName, const SfxStyleFamily& eFamily );
	virtual BOOL		ChangeStyleSheets( 	const XubString& rOldName, SfxStyleFamily eOldFamily,
											const XubString& rNewName, SfxStyleFamily eNewFamily );
	virtual void		ChangeStyleSheetName( SfxStyleFamily eFamily, const XubString& rOldName, const XubString& rNewName );
};

}//end of namespace binfilter
#endif	// _EDITOBJ_HXX
