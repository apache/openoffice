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


#ifndef SYMBOL_HXX
#define SYMBOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _VOS_REFERNCE_HXX_
#include <vos/refernce.hxx>
#endif
#ifndef _FONT_HXX //autogen
#include <vcl/font.hxx>
#endif
#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _DYNARY_HXX
#include <tools/dynary.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef UTILITY_HXX
#include "utility.hxx"
#endif
#ifndef _SMMOD_HXX
#include <smmod.hxx>
#endif
namespace binfilter {

#define SS_ATTR_ACCESS		0x80

#define SYMBOLSET_NONE	0xFFFF
#define SYMBOL_NONE		0xFFFF

class SmSymSetManager;

////////////////////////////////////////////////////////////////////////////////



/*N*/ inline const String GetUiSymbolName( const String &rExportSymbolName )
/*N*/ {
/*N*/     return SM_MOD1()->GetLocSymbolData().GetUiSymbolName( rExportSymbolName );
/*N*/ }



/*N*/ inline const String GetUiSymbolSetName( const String &rExportSymbolSetName )
/*N*/ {
/*N*/     return SM_MOD1()->GetLocSymbolData().GetUiSymbolSetName( rExportSymbolSetName );
/*N*/ }

////////////////////////////////////////////////////////////////////////////////

class SmSym
{
	friend class SmSymSetManager;


	SmFace				 Face;
	String				 Name;
	String				 aExportName;
	String				 aSetName;
	SmSym			   	*pHashNext;
	SmSymSetManager		*pSymSetManager;
	sal_Unicode			 Character;
	BYTE				 Attribut;
	BOOL				 bPredefined;
    BOOL                 bDocSymbol;

public:
	SmSym();
	SmSym(const SmSym& rSymbol);
	SmSym(const String& rName, const Font& rFont, sal_Unicode cChar,
		  const String& rSet, BOOL bIsPredefined = FALSE);

	SmSym&	   	operator = (const SmSym& rSymbol);


	const Font& 	GetFace() const { return Face; }
	sal_Unicode		GetCharacter() const { return Character; }
	const String&	GetName() const { return Name; }

    void            SetFace( const Font& rFont )        { Face = rFont; }
    void            SetCharacter( sal_Unicode cChar )   { Character = cChar; }
    void            SetName( const String &rTxt )       { Name = rTxt; }

	BOOL			IsPredefined() const	{ return bPredefined; }
	const String &	GetSetName() const		{ return aSetName; }
    void            SetSetName( const String &rName )    { aSetName = rName; }
	const String &	GetExportName() const	{ return aExportName; }
	void			SetExportName( const String &rName )	{ aExportName = rName; }

    BOOL            IsDocSymbol() const         { return bDocSymbol; }
    void            SetDocSymbol( BOOL bVal )   { bDocSymbol = bVal; }
};

DECLARE_LIST(SmListSym, SmSym *)//STRIP008 DECLARE_LIST(SmListSym, SmSym *);
SV_DECL_PTRARR( SymbolArray, SmSym *, 32, 32 )//STRIP008 ;

/**************************************************************************/

class SmSymSet
{
	friend class SmSymSetManager;


	SmListSym		  	 SymbolList;
	String				 Name;
	SmSymSetManager		*pSymSetManager;

public:
    SmSymSet(const String& rName);


	const String&	GetName() const { return Name; }
	USHORT			GetCount() const { return (USHORT) SymbolList.Count(); }

	const SmSym&	GetSymbol(USHORT SymbolNo) const
	{
		DBG_ASSERT(SymbolList.GetObject(SymbolNo), "Symbol nicht vorhanden");
		return *SymbolList.GetObject(SymbolNo);
	}

    USHORT      AddSymbol(SmSym* pSymbol);
};

DECLARE_DYNARRAY(SmArraySymSet, SmSymSet *)

/**************************************************************************/



struct SmSymSetManager_Impl
{
    SmArraySymSet       SymbolSets;
    SmSymSetManager &   rSymSetMgr;
    SmSym**             HashEntries;
    USHORT              NoSymbolSets;
    USHORT              NoHashEntries;
    BOOL                Modified;

    SmSymSetManager_Impl( SmSymSetManager &rMgr, USHORT HashTableSize );

};


class SmSymSetManager : public SfxListener
{
    friend struct SmSymSetManager_Impl;

    SmSymSetManager_Impl *pImpl;


	UINT32		GetHashIndex(const String& rSymbolName);
    void        EnterHashTable(SmSym& rSymbol);
	void		EnterHashTable(SmSymSet& rSymbolSet);
	void		FillHashTable();

public:
    SmSymSetManager(USHORT HashTableSize = 137);



	USHORT		AddSymbolSet(SmSymSet* pSymbolSet);
	void		ChangeSymbolSet(SmSymSet* pSymbolSet);
	USHORT		GetSymbolSetPos(const String& rSymbolSetName) const;
        USHORT      GetSymbolSetCount() const { return pImpl->NoSymbolSets; }
	SmSymSet   *GetSymbolSet(USHORT SymbolSetNo) const 
	{ 
		return pImpl->SymbolSets.Get(SymbolSetNo);
	}

	SmSym	    *	GetSymbolByName(const String& rSymbolName);
	const SmSym *	GetSymbolByName(const String& rSymbolName) const
	{
		return ((SmSymSetManager *) this)->GetSymbolByName(rSymbolName);
	}

        void            AddReplaceSymbol( const SmSym & rSymbol );
        USHORT          GetSymbolCount() const;
        const SmSym *   GetSymbolByPos( USHORT nPos ) const;

	BOOL 		IsModified() const { return pImpl->Modified; }
	void		SetModified(BOOL Modify) { pImpl->Modified = Modify; }

        void            Load();
};

} //namespace binfilter
#endif

