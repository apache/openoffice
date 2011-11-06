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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "printopt.hxx"
namespace binfilter {

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star::uno;

// -----------------------------------------------------------------------


// -----------------------------------------------------------------------

/*N*/ ScPrintOptions::ScPrintOptions()
/*N*/ {
/*N*/ 	SetDefaults();
/*N*/ }

/*N*/ ScPrintOptions::ScPrintOptions( const ScPrintOptions& rCpy ) :
/*N*/ 	bSkipEmpty( rCpy.bSkipEmpty ),
/*N*/ 	bAllSheets( rCpy.bAllSheets )
/*N*/ {
/*N*/ }

/*N*/ ScPrintOptions::~ScPrintOptions()
/*N*/ {
/*N*/ }

/*N*/ void ScPrintOptions::SetDefaults()
/*N*/ {
/*N*/ 	bSkipEmpty = FALSE;
/*N*/ 	bAllSheets = TRUE;
/*N*/ }




// -----------------------------------------------------------------------








// -----------------------------------------------------------------------

#define CFGPATH_PRINT			"Office.Calc/Print"

#define SCPRINTOPT_EMPTYPAGES		0
#define SCPRINTOPT_ALLSHEETS		1
#define SCPRINTOPT_COUNT			2


/*N*/ ScPrintCfg::ScPrintCfg() :
/*N*/ 	ConfigItem( OUString::createFromAscii( CFGPATH_PRINT ) )
/*N*/ {
	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 Sequence<OUString> aNames = GetPropertyNames();
/*N*/ }


void ScPrintCfg::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames )
{
}
void ScPrintCfg::Commit()
{
}




}
