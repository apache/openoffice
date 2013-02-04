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


#ifndef _DDEFLD_HXX
#define _DDEFLD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
#include "fldbas.hxx"
namespace binfilter {

class SwDoc;

/*--------------------------------------------------------------------
	Beschreibung: FieldType fuer DDE
 --------------------------------------------------------------------*/

class SwDDEFieldType : public SwFieldType
{
	String aName;
	String aExpansion;

	::binfilter::SvBaseLinkRef refLink;
	SwDoc* pDoc;

	USHORT nRefCnt;
	BOOL bCRLFFlag : 1;
	BOOL bDeleted : 1;

	void _RefCntChgd();
public:
	SwDDEFieldType( const String& rName, const String& rCmd,
                    USHORT = ::binfilter::LINKUPDATE_ONCALL );
	~SwDDEFieldType();

	const String& GetExpansion() const			{ return aExpansion; }
	void SetExpansion( const String& rStr )		{ aExpansion = rStr,
												  bCRLFFlag = FALSE; }

	virtual SwFieldType* Copy() const;
	virtual const String& GetName() const;

    virtual BOOL        QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
    virtual BOOL        PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );

	String GetCmd() const;
	void SetCmd( const String& rStr );

	USHORT GetType() const 			{ return refLink->GetUpdateMode();	}
	void SetType( USHORT nType )	{ refLink->SetUpdateMode( nType );	}

	BOOL IsDeleted() const 			{ return bDeleted; }
	void SetDeleted( BOOL b )		{ bDeleted = b; }

	BOOL IsConnected() const		{ return 0 != refLink->GetObj(); }
	void UpdateNow()				{ refLink->Update(); }
	void Disconnect()				{ refLink->Disconnect(); }

	const ::binfilter::SvBaseLink& GetBaseLink() const 	{ return *refLink; }
		  ::binfilter::SvBaseLink& GetBaseLink()			{ return *refLink; }

	const SwDoc* GetDoc() const 	{ return pDoc; }
		  SwDoc* GetDoc() 			{ return pDoc; }
	void SetDoc( SwDoc* pDoc );

	void IncRefCnt() {	if( !nRefCnt++ && pDoc ) _RefCntChgd();	}
	void DecRefCnt() {	if( !--nRefCnt && pDoc ) _RefCntChgd(); }

	void SetCRLFDelFlag( BOOL bFlag = TRUE )	{ bCRLFFlag = bFlag; }
	BOOL IsCRLFDelFlag() const 					{ return bCRLFFlag; }
};

/*--------------------------------------------------------------------
	Beschreibung: DDE-Feld
 --------------------------------------------------------------------*/

class SwDDEField : public SwField
{
public:
	SwDDEField(SwDDEFieldType*);
	~SwDDEField();

	virtual String	 Expand() const;
	virtual SwField* Copy() const;

	// ueber Typen Parameter ermitteln
	// Name kann nicht geaendert werden
    virtual const String& GetPar1() const;

	// Commando
    virtual String  GetPar2() const;
    virtual void    SetPar2(const String& rStr);
};


} //namespace binfilter
#endif // _DDEFLD_HXX
