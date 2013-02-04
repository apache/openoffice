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



#ifndef SC_PRINTOPT_HXX
#define SC_PRINTOPT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXPOOLITEM_HXX 
#include <bf_svtools/poolitem.hxx>
#endif

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
namespace binfilter {


class ScPrintOptions
{
private:
	BOOL	bSkipEmpty;
	BOOL	bAllSheets;

public:
				ScPrintOptions();
				ScPrintOptions( const ScPrintOptions& rCpy );
				~ScPrintOptions();

	BOOL	GetSkipEmpty() const			{ return bSkipEmpty; }
	void	SetSkipEmpty( BOOL bVal )		{ bSkipEmpty = bVal; }
	BOOL	GetAllSheets() const			{ return bAllSheets; }
	void	SetAllSheets( BOOL bVal )		{ bAllSheets = bVal; }

	void	SetDefaults();

};

//==================================================================
// item for the dialog / options page
//==================================================================


//==================================================================
// config item
//==================================================================

class ScPrintCfg : public ScPrintOptions, public ::utl::ConfigItem
{

public:
	ScPrintCfg();

	void			SetOptions( const ScPrintOptions& rNew ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 	void			SetOptions( const ScPrintOptions& rNew );
    virtual void                Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void                Commit();
};

} //namespace binfilter
#endif

