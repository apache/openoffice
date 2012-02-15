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


#ifndef _REFFLD_HXX
#define _REFFLD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
namespace binfilter {

class SfxPoolItem;
class SwDoc;
class SwTxtAttr;
class SwTxtNode;

enum REFERENCESUBTYPE
{
	REF_SETREFATTR = 0,
	REF_SEQUENCEFLD,
	REF_BOOKMARK,
	REF_OUTLINE,
	REF_FOOTNOTE,
	REF_ENDNOTE
};

enum REFERENCEMARK
{
	REF_BEGIN,
	REF_PAGE = REF_BEGIN,
	REF_CHAPTER,
	REF_CONTENT,
	REF_UPDOWN,
	REF_PAGE_PGDESC,
	REF_ONLYNUMBER,
	REF_ONLYCAPTION,
	REF_ONLYSEQNO,
	REF_END
};


/*--------------------------------------------------------------------
	Beschreibung: Referenz holen
 --------------------------------------------------------------------*/

class SwGetRefFieldType : public SwFieldType
{
	SwDoc* pDoc;
public:
	SwGetRefFieldType(SwDoc* pDoc );
 	virtual SwFieldType*    Copy() const;

	SwDoc* 					GetDoc() const { return pDoc; }
	// ueberlagert, um alle Ref-Felder zu updaten
	virtual void Modify( SfxPoolItem *, SfxPoolItem * );

	void MergeWithOtherDoc( SwDoc& rDestDoc );

	static SwTxtNode* FindAnchor( SwDoc* pDoc, const String& rRefMark,
										USHORT nSubType, USHORT nSeqNo,
										USHORT* pStt, USHORT* pEnd = 0 );
};

/*--------------------------------------------------------------------
	Beschreibung: Referenzfeld
 --------------------------------------------------------------------*/

class SwGetRefField : public SwField
{
	String sSetRefName;
	String sTxt;
	USHORT nSubType;
	USHORT nSeqNo;
public:
	SwGetRefField( SwGetRefFieldType*, const String& rSetRef,
					USHORT nSubType, USHORT nSeqNo, ULONG nFmt );

	virtual	String		GetCntnt(BOOL bName = FALSE) const;
	virtual String	 	Expand() const;
	virtual SwField* 	Copy() const;

	const String& 		GetSetRefName() const { return sSetRefName; }

	void 				UpdateField();
	void 				SetExpand( const String& rStr ) { sTxt = rStr; }

	// SubType erfragen/setzen
	virtual USHORT 		GetSubType() const;
    virtual void        SetSubType( USHORT n );

	// SequenceNo erfragen/setzen (nur fuer REF_SEQUENCEFLD interressant)
	USHORT 				GetSeqNo() const		{ return nSeqNo; }
	void  				SetSeqNo( USHORT n ) 	{ nSeqNo = n; }

	// Name der Referenz
	virtual const String& GetPar1() const;
    virtual void        SetPar1(const String& rStr);

    virtual String      GetPar2() const;
	virtual	BOOL       	QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
	virtual	BOOL		PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );

    void                ConvertProgrammaticToUIName();
};


} //namespace binfilter
#endif // _REFFLD_HXX

