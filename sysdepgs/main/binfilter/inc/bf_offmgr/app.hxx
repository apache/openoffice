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


#ifndef _OFF_APP_HXX
#define _OFF_APP_HXX

// include ---------------------------------------------------------------

#ifndef _SFXAPP_HXX //autogen
#include <bf_sfx2/app.hxx>
#endif

class ResMgr; 

namespace binfilter {


#define OFA_REFERER			"private:user"

// forward ---------------------------------------------------------------

class XColorTable;
class  OfficeData_Impl;

// class OfficeApplication -----------------------------------------------

class OfficeApplication : public SfxApplication
{
private:
	OfficeData_Impl*			pDataImpl;		

	virtual void                Init( );

public:
								TYPEINFO();

								OfficeApplication();
                                ~OfficeApplication();

	XColorTable*				GetStdColorTable();
};

#define OFF_APP()	( (::binfilter::OfficeApplication*)::binfilter::SfxGetpApp() )

} //namespace binfilter
#endif // #ifndef _OFF_APP_HXX

