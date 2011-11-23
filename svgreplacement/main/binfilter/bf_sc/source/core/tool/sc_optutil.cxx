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


#include "optutil.hxx"
#include "global.hxx"       // for pSysLocale

#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX
#include <bf_svtools/syslocale.hxx>
#endif
namespace binfilter {

//------------------------------------------------------------------

// static
/*N*/ BOOL ScOptionsUtil::IsMetricSystem()
/*N*/ {
/*N*/ 	//!	which language should be used here - system language or installed office language?
/*N*/ 
/*N*/ //	MeasurementSystem eSys = Application::GetAppInternational().GetMeasurementSystem();
/*N*/     MeasurementSystem eSys = ScGlobal::pLocaleData->getMeasurementSystemEnum();
/*N*/ 
/*N*/ 	return ( eSys == MEASURE_METRIC );
/*N*/ }

//------------------------------------------------------------------

/*N*/ ScLinkConfigItem::ScLinkConfigItem( const ::rtl::OUString rSubTree ) :
/*N*/ 	ConfigItem( rSubTree )
/*N*/ {
/*N*/ }

/*N*/ void ScLinkConfigItem::SetCommitLink( const Link& rLink )
/*N*/ {
/*N*/ 	aCommitLink = rLink;
/*N*/ }

void  ScLinkConfigItem::Notify( const com::sun::star::uno::Sequence< rtl::OUString >& )
{
}

void  ScLinkConfigItem::Commit()
{
}

}
