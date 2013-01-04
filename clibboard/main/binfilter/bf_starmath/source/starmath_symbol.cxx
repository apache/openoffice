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



#ifndef _SFXDOCFILE_HXX
#include <bf_sfx2/docfile.hxx>
#endif

#include "config.hxx"
#include "starmath.hrc"
namespace binfilter {


using namespace ::com::sun::star;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::uno;
using namespace ::rtl;

// Das hier muss auch mal alles "uberarbeitet werden. Insbesondere die nicht
// funktionierende und bei l"oschen/"andern von Symbolen nicht gepflegte
// Hash Tabelle!!!  Diese aktualisert sich erst im Wertzuweisungsoperator
// beim Verlassen des 'SmSymDefineDialog's!

/**************************************************************************/
/*
**
**	MACRO DEFINTION
**
**/

#define SF_SM20IDENT 0x03031963L
#define SF_IDENT	 0x30334D53L



/**************************************************************************/
/*
**
**	DATA DEFINITION
**
**/

long				SF_Ident = SF_IDENT;

/**************************************************************************/
/*
**
**	CLASS IMPLEMENTATION
**
**/

/*N*/ SmSym::SmSym() :
/*N*/ 	Name(C2S("unknown")),
/*N*/ 	Character('\0'),
/*N*/ 	pHashNext(0),
/*N*/ 	pSymSetManager(0),
/*N*/ 	bPredefined(FALSE),
/*N*/     bDocSymbol(FALSE),
/*N*/ 	aSetName(C2S("unknown"))
/*N*/ {
/*N*/ 	aExportName = Name;
/*N*/ 	Face.SetTransparent(TRUE);
/*N*/     Face.SetAlign(ALIGN_BASELINE);
/*N*/ }


/*N*/ SmSym::SmSym(const SmSym& rSymbol)
/*N*/ {
/*N*/ 	pSymSetManager = 0;
/*N*/     *this = rSymbol;
/*N*/ }


/*N*/ SmSym::SmSym(const String& rName, const Font& rFont, sal_Unicode aChar,
/*N*/ 			 const String& rSet, BOOL bIsPredefined)
/*N*/ {
/*N*/ 	Name		= aExportName	= rName;
/*N*/ 
/*N*/     Face        = rFont;
/*N*/ 	Face.SetTransparent(TRUE);
/*N*/     Face.SetAlign(ALIGN_BASELINE);
/*N*/ 
/*N*/     Character   = aChar;
/*N*/     if (RTL_TEXTENCODING_SYMBOL == rFont.GetCharSet())
/*?*/         Character |= 0xF000;
/*N*/ 	aSetName	= rSet;
/*N*/ 	bPredefined	= bIsPredefined;
/*N*/     bDocSymbol  = FALSE;
/*N*/ 
/*N*/ 	pHashNext	   = 0;
/*N*/ 	pSymSetManager = 0;
/*N*/ }


/*N*/ SmSym& SmSym::operator = (const SmSym& rSymbol)
/*N*/ {
/*N*/ 	Name		= rSymbol.Name;
/*N*/ 	Face		= rSymbol.Face;
/*N*/ 	Character	= rSymbol.Character;
/*N*/ 	aSetName	= rSymbol.aSetName;
/*N*/ 	bPredefined	= rSymbol.bPredefined;
/*N*/     bDocSymbol  = rSymbol.bDocSymbol;
/*N*/ 	aExportName	= rSymbol.aExportName;
/*N*/ 
/*N*/ 	pHashNext = 0;
/*N*/ 
/*N*/     if (pSymSetManager)
/*?*/ 		pSymSetManager->SetModified(TRUE);
/*N*/ 
/*N*/ 	return *this;
/*N*/ }




/**************************************************************************/



/*N*/ SmSymSet::SmSymSet(const String& rName)
/*N*/ {
/*N*/ 	Name = rName;
/*N*/ 	SymbolList.Clear();
/*N*/ 
/*N*/ 	pSymSetManager = 0;
/*N*/ }




/*N*/ USHORT SmSymSet::AddSymbol(SmSym* pSymbol)
/*N*/ {
/*N*/ 	DBG_ASSERT(pSymbol, "Kein Symbol");
/*N*/ 
/*N*/     if (pSymbol)
/*N*/         pSymbol->SetSetName( GetName() );
/*N*/ 	SymbolList.Insert(pSymbol, LIST_APPEND);
/*N*/ 	DBG_ASSERT(SymbolList.GetPos(pSymbol) == SymbolList.Count() - 1,
/*N*/ 		"Sm : ... ergibt falschen return Wert");
/*N*/ 
/*N*/ 	if (pSymSetManager)
/*N*/ 		pSymSetManager->SetModified(TRUE);
/*N*/ 
/*N*/ 	return (USHORT) SymbolList.Count() - 1;
/*N*/ }







/**************************************************************************/

/*N*/ SmSymSetManager_Impl::SmSymSetManager_Impl(
/*N*/         SmSymSetManager &rMgr, USHORT HashTableSize ) :
/*N*/ 
/*N*/     rSymSetMgr    (rMgr)
/*N*/ {
/*N*/     NoSymbolSets    = 0;
/*N*/     NoHashEntries   = HashTableSize;
/*N*/     HashEntries     = new SmSym *[NoHashEntries];
/*N*/     memset( HashEntries, 0, sizeof(SmSym *) * NoHashEntries );
/*N*/     Modified        = FALSE;
/*N*/ }





/**************************************************************************/





/*N*/ UINT32 SmSymSetManager::GetHashIndex(const String& rSymbolName)
/*N*/ {
/*N*/     UINT32 x = 1;
/*N*/ 	for (xub_StrLen i = 0; i < rSymbolName.Len(); i++)
/*N*/         x += x * rSymbolName.GetChar(i) + i;
/*N*/ 
/*N*/ 	return x % pImpl->NoHashEntries;
/*N*/ }


/*N*/ void SmSymSetManager::EnterHashTable(SmSym& rSymbol)
/*N*/ {
/*N*/     int j = GetHashIndex( rSymbol.GetName() );
/*N*/     if (pImpl->HashEntries[j] == 0)
/*N*/         pImpl->HashEntries[j] = &rSymbol;
/*N*/     else
/*N*/     {
/*N*/         SmSym *p = pImpl->HashEntries[j];
/*N*/         while (p->pHashNext)
/*N*/             p = p->pHashNext;
/*N*/         p->pHashNext = &rSymbol;
/*N*/     }
/*N*/     rSymbol.pHashNext = 0;
/*N*/ }


/*N*/ void SmSymSetManager::EnterHashTable(SmSymSet& rSymbolSet)
/*N*/ {
/*N*/ 	for (int i = 0; i < rSymbolSet.GetCount(); i++)
/*N*/         EnterHashTable( *rSymbolSet.SymbolList.GetObject(i) );
/*N*/ }

/*N*/ void SmSymSetManager::FillHashTable()
/*N*/ {
/*N*/ 	if (pImpl->HashEntries)
/*N*/ 	{
/*N*/ 		memset( pImpl->HashEntries, 0, pImpl->NoHashEntries * sizeof(SmSym *) );
/*N*/ 
/*N*/ 		for (UINT32 i = 0; i < pImpl->NoSymbolSets; i++)
/*N*/ 			EnterHashTable( *GetSymbolSet( (USHORT) i ) );
/*N*/ 	}
/*N*/ }





/*N*/ SmSymSetManager::SmSymSetManager(USHORT HashTableSize)
/*N*/ {
/*N*/     pImpl = new SmSymSetManager_Impl( *this, HashTableSize );
/*N*/ }






/*N*/ USHORT SmSymSetManager::AddSymbolSet(SmSymSet* pSymbolSet)
/*N*/ {
/*N*/ 	if (pImpl->NoSymbolSets >= pImpl->SymbolSets.GetSize())
/*?*/ 		pImpl->SymbolSets.SetSize(pImpl->NoSymbolSets + 1);
/*N*/ 
/*N*/ 	pImpl->SymbolSets.Put(pImpl->NoSymbolSets++, pSymbolSet);
/*N*/ 
/*N*/ 	pSymbolSet->pSymSetManager = this;
/*N*/ 
/*N*/ 	for (int i = 0; i < pSymbolSet->GetCount(); i++)
/*?*/ 		pSymbolSet->SymbolList.GetObject(i)->pSymSetManager = this;
/*N*/ 
/*N*/ 	FillHashTable();
/*N*/ 	pImpl->Modified = TRUE;
/*N*/ 
/*N*/ 	return (USHORT) (pImpl->NoSymbolSets - 1);
/*N*/ }

/*N*/ void SmSymSetManager::ChangeSymbolSet(SmSymSet* pSymbolSet)
/*N*/ {
/*N*/ 	if (pSymbolSet)
/*N*/ 	{
/*N*/ 		FillHashTable();
/*N*/ 		pImpl->Modified = TRUE;
/*N*/ 	}
/*N*/ }



/*N*/ USHORT SmSymSetManager::GetSymbolSetPos(const String& rSymbolSetName) const
/*N*/ {
/*N*/ 	for (USHORT i = 0; i < pImpl->NoSymbolSets; i++)
/*N*/ 		if (pImpl->SymbolSets.Get(i)->GetName() == rSymbolSetName)
/*N*/ 			return (i);
/*N*/ 
/*N*/ 	return SYMBOLSET_NONE;
/*N*/ }

/*N*/ SmSym *SmSymSetManager::GetSymbolByName(const String& rSymbolName)
/*N*/ {
/*N*/     SmSym *pSym = pImpl->HashEntries[GetHashIndex(rSymbolName)];
/*N*/     while (pSym)
/*N*/     {
/*N*/         if (pSym->Name == rSymbolName)
/*N*/             break;
/*N*/         pSym = pSym->pHashNext;
/*N*/     }
/*N*/ 
/*N*/ 	return pSym;
/*N*/ }


/*N*/ void SmSymSetManager::AddReplaceSymbol( const SmSym &rSymbol )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }


/*N*/ USHORT SmSymSetManager::GetSymbolCount() const
/*N*/ { 
/*N*/     USHORT nRes = 0;
/*N*/     USHORT nSets = GetSymbolSetCount();
/*N*/     for (USHORT i = 0;  i < nSets;  ++i)
/*N*/         nRes += GetSymbolSet(i)->GetCount();
/*N*/     return nRes;
/*N*/ }


/*N*/ const SmSym * SmSymSetManager::GetSymbolByPos( USHORT nPos ) const
/*N*/ {
/*N*/     const SmSym *pRes = 0;
/*N*/ 
/*N*/     INT16 nIdx = 0;
/*N*/     USHORT nSets = GetSymbolSetCount();
/*N*/     USHORT i = 0;
/*N*/     while (i < nSets  &&  !pRes)
/*N*/     {
/*N*/         USHORT nEntries = GetSymbolSet(i)->GetCount();
/*N*/         if (nPos < nIdx + nEntries)
/*N*/             pRes = &GetSymbolSet(i)->GetSymbol( nPos - nIdx );
/*N*/         else
/*N*/             nIdx += nEntries;
/*N*/         ++i;
/*N*/     }
/*N*/ 
/*N*/     return pRes;
/*N*/ }


/*N*/ void SmSymSetManager::Load()
/*N*/ {
/*N*/     SmMathConfig &rCfg = *SM_MOD1()->GetConfig();
/*N*/ 
/*N*/     USHORT nCount = rCfg.GetSymbolCount();
/*N*/     USHORT i;
/*N*/     for (i = 0;  i < nCount;  ++i)
/*N*/     {
/*N*/         const SmSym *pSym = rCfg.GetSymbol(i);
/*N*/         if (pSym)
/*N*/         {
/*N*/             SmSymSet *pSymSet = 0;
/*N*/             const String &rSetName = pSym->GetSetName();
/*N*/             USHORT nSetPos = GetSymbolSetPos( rSetName );
/*N*/             if (SYMBOLSET_NONE != nSetPos)
/*N*/                 pSymSet = GetSymbolSet( nSetPos );
/*N*/             else
/*N*/             {
/*N*/                 pSymSet = new SmSymSet( rSetName );
/*N*/                 AddSymbolSet( pSymSet );
/*N*/             }
/*N*/ 
/*N*/             pSymSet->AddSymbol( new SmSym( *pSym ) );
/*N*/         }
/*N*/     }
/*N*/     // build HashTables
/*N*/     nCount = GetSymbolSetCount();
/*N*/     for (i = 0;  i < nCount;  ++i)
/*N*/         ChangeSymbolSet( GetSymbolSet( i ) );
/*N*/ 
/*N*/     if (0 == nCount)
/*N*/     {
/*N*/         DBG_ERROR( "no symbol set found" );
/*N*/         pImpl->Modified = FALSE;
/*N*/     }
/*N*/ }





}
