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

#ifndef _MATH_CFGITEM_HXX_
#define _MATH_CFGITEM_HXX_

#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/uno/Sequence.hxx>

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#include <bf_svtools/bf_solar.h>

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif

#include <symbol.hxx>
#include <types.hxx>
class Font; 
namespace binfilter {

using namespace ::com::sun::star;

class SmSym;
class SmFormat;

struct SmCfgOther;

/////////////////////////////////////////////////////////////////

class SmMathConfigItem : public ::utl::ConfigItem
{
	// disallow copy-constructor and assignment-operator for now
	SmMathConfigItem( const SmMathConfigItem & );
	SmMathConfigItem & operator = ( const SmMathConfigItem & );

public:
    inline SmMathConfigItem( const ::rtl::OUString &rPath,
                      sal_Int16 nMode = CONFIG_MODE_IMMEDIATE_UPDATE );

    uno::Sequence< ::rtl::OUString > GetFormatPropertyNames();
    uno::Sequence< ::rtl::OUString > GetOtherPropertyNames();

	uno::Sequence< uno::Any >
		GetProperties( const uno::Sequence< ::rtl::OUString > &rNames )
		{
			return ConfigItem::GetProperties(rNames);
		}

	sal_Bool
		PutProperties( const uno::Sequence< ::rtl::OUString > &rNames,
					   const uno::Sequence< uno::Any > &rValues)
		{
			return ConfigItem::PutProperties( rNames, rValues);
		}

	sal_Bool
		SetSetProperties( const ::rtl::OUString &rNode,
						  uno::Sequence< beans::PropertyValue > rValues )
		{
			return ConfigItem::SetSetProperties( rNode, rValues );
		}

	uno::Sequence< ::rtl::OUString >
		GetNodeNames( const ::rtl::OUString &rNode )
		{
			return ConfigItem::GetNodeNames( rNode );
		}

    sal_Bool
        ReplaceSetProperties( const ::rtl::OUString& rNode,
                              uno::Sequence< beans::PropertyValue > rValues )
        {
            return ConfigItem::ReplaceSetProperties( rNode, rValues );
        }
	void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
	void Commit();
};


inline SmMathConfigItem::SmMathConfigItem(
        const ::rtl::OUString &rPath,
        sal_Int16 nMode ) :
    ConfigItem( rPath, nMode )
{
}


/////////////////////////////////////////////////////////////////


struct SmFontFormat
{
    String      aName;
    INT16       nCharSet;
    INT16       nFamily;
    INT16       nPitch;
    INT16       nWeight;
    INT16       nItalic;

    SmFontFormat();
//    SmFontFormat( const Font &rFont );

    const Font      GetFont() const;
};


struct SmFntFmtListEntry
{
    String          aId;
    SmFontFormat    aFntFmt;

    SmFntFmtListEntry( const String &rId, const SmFontFormat &rFntFmt );
};


SV_DECL_OBJARR( SmFntFmtListEntryArr, SmFntFmtListEntry, 8, 8 )//STRIP008 ;


class SmFontFormatList
{
    SmFntFmtListEntryArr    aEntries;
    BOOL                    bModified;

	// disallow copy-constructor and assignment-operator for now
    SmFontFormatList( const SmFontFormatList & );
    SmFontFormatList & operator = ( const SmFontFormatList & );

public:
    SmFontFormatList();

    void    AddFontFormat( const String &rFntFmtId, const SmFontFormat &rFntFmt );

    const SmFontFormat *    GetFontFormat( const String &rFntFmtId ) const;
//    const SmFontFormat *    GetFontFormat( USHORT nPos ) const;
    USHORT                  GetCount() const    { return aEntries.Count(); }

    BOOL    IsModified() const          { return bModified; }
    void    SetModified( BOOL bVal )    { bModified = bVal; }
};


/////////////////////////////////////////////////////////////////

class SmMathConfig
{
    Timer               aSaveTimer;
    SmFormat *          pFormat;
    SmCfgOther *        pOther;
    SmFontFormatList *  pFontFormatList;
    SmSym *             pSymbols;
    USHORT              nSymbolCount;
    BOOL                bIsOtherModified;
    BOOL                bIsFormatModified;

	// disallow copy-constructor and assignment-operator for now
	SmMathConfig( const SmMathConfig & );
	SmMathConfig & operator = ( const SmMathConfig & );

	void	LoadSymbols();

    void    LoadOther();
    void    SaveOther();
    void    LoadFormat();
    void    SaveFormat();
    void    LoadFontFormatList();
    void    SaveFontFormatList();



    void    Save();

    SmSym           ReadSymbol( SmMathConfigItem &rCfg,
						const ::rtl::OUString &rSymbolName,
						const ::rtl::OUString &rBaseNode ) const;
    SmFontFormat    ReadFontFormat( SmMathConfigItem &rCfg,
						const ::rtl::OUString &rSymbolName,
						const ::rtl::OUString &rBaseNode ) const;


	DECL_LINK( TimeOut, Timer * );

protected:
    void        SetOtherModified( BOOL bVal );
    inline BOOL IsOtherModified() const     { return bIsOtherModified; }
    void        SetFormatModified( BOOL bVal );
    inline BOOL IsFormatModified() const    { return bIsFormatModified; }

    SmFontFormatList &          GetFontFormatList();
    const SmFontFormatList &    GetFontFormatList() const
    {
        return ((SmMathConfig *) this)->GetFontFormatList();
    }

public:
	SmMathConfig();
	~SmMathConfig();

	USHORT			GetSymbolCount() const;
	const SmSym *	GetSymbol( USHORT nIndex ) const;

    const SmFormat &    GetStandardFormat() const;

    BOOL            IsPrintTitle() const;
    BOOL            IsPrintFormulaText() const;
    BOOL            IsPrintFrame() const;
    SmPrintSize     GetPrintSize() const;
    USHORT          GetPrintZoomFactor() const;

    BOOL            IsIgnoreSpacesRight() const;
    BOOL            IsAutoRedraw() const;
};

/////////////////////////////////////////////////////////////////

} //namespace binfilter
#endif

