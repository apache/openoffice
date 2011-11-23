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


#ifndef _CAPTION_HXX
#define _CAPTION_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GLOBNAME_HXX //autogen
#include <tools/globname.hxx>
#endif
namespace binfilter {

enum SwCapObjType
{
	FRAME_CAP, GRAPHIC_CAP, TABLE_CAP, OLE_CAP
};

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

class InsCaptionOpt
{
private:
	BOOL		 bUseCaption;
	SwCapObjType eObjType;
	SvGlobalName aOleId;
	String		 sCategory;
	USHORT		 nNumType;
	String		 sCaption;
	USHORT		 nPos;
	USHORT		 nLevel;
	sal_Unicode	 cSeparator;
	BOOL		 bIgnoreSeqOpts;	// wird nicht gespeichert
	BOOL		 bCopyAttributes;   // 			-""-

public:
	InsCaptionOpt(const SwCapObjType eType = FRAME_CAP, const SvGlobalName* pOleId = 0);
	~InsCaptionOpt();

	inline BOOL&			UseCaption()					{ return bUseCaption; }
	inline BOOL				UseCaption() const				{ return bUseCaption; }

	inline SwCapObjType		GetObjType() const				{ return eObjType; }
	inline void				SetObjType(const SwCapObjType eType) { eObjType = eType; }

	inline const SvGlobalName&	GetOleId() const				{ return aOleId; }
	inline void				SetOleId(const SvGlobalName &rId)	{ aOleId = rId; }

	inline const String&	GetCategory() const				{ return sCategory; }
	inline void				SetCategory(const String& rCat) { sCategory = rCat; }

	inline USHORT			GetNumType() const				{ return nNumType; }
	inline void				SetNumType(const USHORT nNT)	{ nNumType = nNT; }

	inline const String&	GetCaption() const				{ return sCaption; }
	inline void				SetCaption(const String& rCap)	{ sCaption = rCap; }

	inline USHORT			GetPos() const					{ return nPos; }
	inline void				SetPos(const USHORT nP)			{ nPos = nP; }

	inline USHORT			GetLevel() const				{ return nLevel; }
	inline void				SetLevel(const USHORT nLvl)		{ nLevel = nLvl; }

	inline sal_Unicode		GetSeparator() const				{ return cSeparator; }
	inline void				SetSeparator(const sal_Unicode cSep){ cSeparator = cSep; }

	inline BOOL&			IgnoreSeqOpts()					{ return bIgnoreSeqOpts; }
	inline BOOL				IgnoreSeqOpts() const			{ return bIgnoreSeqOpts; }

	inline BOOL& 			CopyAttributes() 				{ return bCopyAttributes; }
	inline BOOL 			CopyAttributes() const			{ return bCopyAttributes; }

	inline BOOL				operator< ( const InsCaptionOpt & rObj ) const
												{ return aOleId < rObj.aOleId; }
};

} //namespace binfilter
#endif


