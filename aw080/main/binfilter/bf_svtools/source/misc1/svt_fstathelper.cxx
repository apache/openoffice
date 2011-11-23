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



// MARKER(update_precomp.py): autogen include statement, do not remove


#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _TIME_HXX
#include <tools/time.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif

#include <fstathelper.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::rtl;

namespace binfilter
{


sal_Bool GetModifiedDateTimeOfFile( const UniString& rURL,
										Date* pDate, Time* pTime )
{
	sal_Bool bRet = FALSE;
	try
	{
		::ucbhelper::Content aTestContent( rURL,
								uno::Reference< XCommandEnvironment > ());
		uno::Any aAny = aTestContent.getPropertyValue(
			OUString::createFromAscii(  "DateModified" ) );
		if( aAny.hasValue() )
		{
			bRet = sal_True;
			const util::DateTime* pDT = (util::DateTime*)aAny.getValue();
			if( pDate )
				*pDate = Date( pDT->Day, pDT->Month, pDT->Year );
			if( pTime )
				*pTime = Time( pDT->Hours, pDT->Minutes,
							   pDT->Seconds, pDT->HundredthSeconds );
		}
	}
	catch(...)
	{
	}

	return bRet;
}

sal_Bool IsDocument( const UniString& rURL )
{
	BOOL bExist = FALSE;
	try
	{
		::ucbhelper::Content aTestContent( rURL,
								uno::Reference< XCommandEnvironment > ());
		bExist = aTestContent.isDocument();
	}
	catch(...)
	{
	}
	return bExist;
}

}
