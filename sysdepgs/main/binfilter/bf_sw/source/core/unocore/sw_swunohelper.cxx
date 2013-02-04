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

#define _SVSTDARR_STRINGS

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP_
#include <com/sun/star/uno/Exception.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTIDENTIFIER_HPP_
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDER_HPP_
#include <com/sun/star/ucb/XContentProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_TRANSFERINFO_HPP_
#include <com/sun/star/ucb/TransferInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_NAMECLASH_HDL_
#include <com/sun/star/ucb/NameClash.hdl>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif
#ifndef _UCBHELPER_CONTENTBROKER_HXX
#include <ucbhelper/contentbroker.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#include <bf_svtools/svstdarr.hxx>

#ifndef _SWUNOHELPER_HXX
#include <swunohelper.hxx>
#endif
#ifndef _SWUNODEF_HXX
#include <swunodef.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {
namespace SWUnoHelper {

sal_Int32 GetEnumAsInt32( const UNO_NMSPC::Any& rVal )
{
	sal_Int32 eVal;
	try
	{
		eVal = comphelper::getEnumAsINT32( rVal );
	}
	catch( UNO_NMSPC::Exception & )
	{
		eVal = 0;
		ASSERT( FALSE, "can't get EnumAsInt32" );
	}
	return eVal;
}


// methods for UCB actions
BOOL UCB_DeleteFile( const String& rURL )
{
	BOOL bRemoved;
	try
    {
		ucbhelper::Content aTempContent( rURL,
								STAR_REFERENCE( ucb::XCommandEnvironment )());
		aTempContent.executeCommand(
						::rtl::OUString::createFromAscii( "delete" ),
						UNO_NMSPC::makeAny( sal_Bool( sal_True ) ) );
		bRemoved = TRUE;
    }
    catch( UNO_NMSPC::Exception& )
    {
		bRemoved = FALSE;
		ASSERT( FALSE, "Exeception from executeCommand( delete )" );
    }
	return bRemoved;
}

}
}
