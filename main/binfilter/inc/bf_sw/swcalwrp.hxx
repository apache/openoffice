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



#ifndef _SWCALWRP_HXX
#define _SWCALWRP_HXX

#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _UNOTOOLS_CALENDARWRAPPER_HXX
#include <unotools/calendarwrapper.hxx>
#endif
namespace binfilter {

class SwCalendarWrapper : public CalendarWrapper
{
	String sUniqueId;
	USHORT nLang;

public:
	SwCalendarWrapper( const ::com::sun::star::uno::Reference<
					::com::sun::star::lang::XMultiServiceFactory > & xMSF )
		: CalendarWrapper( xMSF ), nLang( LANGUAGE_SYSTEM )
	{}

};


extern SwCalendarWrapper* pCalendarWrapper;


} //namespace binfilter
#endif

