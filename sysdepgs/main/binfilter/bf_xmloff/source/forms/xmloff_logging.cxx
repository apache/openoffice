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



#ifndef XMLOFF_FORMS_LOGGING_HXX
#include "logging.hxx"
#endif
#ifndef _RTL_LOGFILE_HXX_ 
#include <rtl/logfile.hxx>
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

#ifdef TIMELOG
	//=====================================================================
	//= OStackedLogging
	//=====================================================================
	//---------------------------------------------------------------------
	void OStackedLogging::enterContext( const sal_Char* _pContextName )
	{
		m_aLogger.push( new ::rtl::Logfile( _pContextName ) );
	}

	//---------------------------------------------------------------------
	void OStackedLogging::leaveTopContext( )
	{
		delete m_aLogger.top();
		m_aLogger.pop();
	}
#endif

//.........................................................................
}	// namespace xmloff
//.........................................................................


}//end of namespace binfilter
