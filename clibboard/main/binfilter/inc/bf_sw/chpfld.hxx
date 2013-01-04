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


#ifndef _CHPFLD_HXX
#define _CHPFLD_HXX

#include <bf_svtools/bf_solar.h>


class SwFrm;
class SwTxtNode;

#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
namespace binfilter {

enum SwChapterFormat
{
	CF_BEGIN,
	CF_NUMBER = CF_BEGIN,		// nur die Kapitelnummer
	CF_TITLE,					// nur die "Ueberschrift"
	CF_NUM_TITLE,				// Kapitelnummer und "Ueberschrift"
	CF_NUMBER_NOPREPST,			// nur die Kapitelnummer ohne Post/Prefix
	CF_NUM_NOPREPST_TITLE,		// Kapitelnummer ohne Post/Prefix und "Ueberschrift"
	CF_END
};

/*--------------------------------------------------------------------
	Beschreibung: Kapitel
 --------------------------------------------------------------------*/

class SwChapterFieldType : public SwFieldType
{
public:
	SwChapterFieldType();

    virtual SwFieldType*    Copy() const;

};



/*--------------------------------------------------------------------
	Beschreibung: Kapitelnummer
 --------------------------------------------------------------------*/
class SwChapterField : public SwField
{
	friend class SwChapterFieldType;
	BYTE nLevel;
	String sTitle, sNumber, sPre, sPost;
public:
	SwChapterField(SwChapterFieldType*, sal_uInt32 nFmt = 0);

	void ChangeExpansion( const SwFrm*, const SwTxtNode*, BOOL bSrchNum = FALSE);

	virtual String	 Expand() const;
	virtual SwField* Copy() const;

	inline BYTE GetLevel() const;
	inline void	SetLevel(BYTE);

	inline const String& GetNumber() const;
	inline const String& GetTitle() const;
	virtual	BOOL       	QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
	virtual	BOOL		PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );
};

inline BYTE SwChapterField::GetLevel() const 	{ return nLevel; }
inline void	SwChapterField::SetLevel(BYTE nLev) { nLevel = nLev; }
inline const String& SwChapterField::GetNumber() const { return sNumber; }
inline const String& SwChapterField::GetTitle() const { return sTitle; }

} //namespace binfilter
#endif // _CHPFLD_HXX
