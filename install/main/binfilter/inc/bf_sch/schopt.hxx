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



#ifndef _SCH_SCHOPT_HXX
#define _SCH_SCHOPT_HXX

#include <bf_svtools/bf_solar.h>

// header for TYPEINFO
#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif
// header for ConfigItem
#ifndef _UTL_CONFIGITEM_HXX_ 
#include <unotools/configitem.hxx>
#endif
// header for SfxPoolItem
#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
// header for Color
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif
// header for Table
#ifndef _TOOLS_TABLE_HXX
#include <tools/table.hxx>
#endif
// header for XColorEntry
#ifndef _XTABLE_HXX
#include <bf_svx/xtable.hxx>   //STRIP002 
#endif
namespace binfilter {

// --------------------
// default colors
// --------------------
class SchColorTable : private Table
{
public:
	SchColorTable();
	virtual ~SchColorTable() {}


	// make public
	Table::Count;

	// specialized methods from Table
	void			ClearAndDestroy();

	BOOL			Insert( ULONG nKey, XColorEntry* );
	XColorEntry*	Get( ULONG nKey ) const;

	Color			GetColor( ULONG nKey ) const;
	ColorData		GetColorData( ULONG nKey ) const;

	BOOL			operator ==( const SchColorTable& rCol ) const
					{ return Table::operator ==( rCol ); }
	BOOL			operator !=( const SchColorTable& rCol ) const
					{ return Table::operator !=( rCol ); }
};

// ====================
// all options
// ====================
class SchOptions : public ::utl::ConfigItem
{
private:
	SchColorTable		maDefColors;
	BOOL				mbIsInitialized;
	::com::sun::star::uno::Sequence< ::rtl::OUString > maPropertyNames;

	::com::sun::star::uno::Sequence< ::rtl::OUString > GetPropertyNames() const
		{ return maPropertyNames; }
	BOOL RetrieveOptions();

public:
	SchOptions();
	virtual ~SchOptions();
void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
void Commit();

	const SchColorTable& GetDefaultColors();

};

// ====================
// items
// ====================

} //namespace binfilter
#endif	// _SCH_SCHOPT_HXX

