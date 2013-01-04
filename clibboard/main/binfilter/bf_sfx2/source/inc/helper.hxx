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


#ifndef _SFX_HELPER_HXX
#define _SFX_HELPER_HXX

// include ---------------------------------------------------------------

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

class String;



namespace binfilter {

// class SfxContentHelper ------------------------------------------------

class SfxContentHelper
{
private:

public:

	static ::com::sun::star::uno::Sequence< ::rtl::OUString >
								GetResultSet( const String& rURL );
	static String				GetActiveHelpString( const String& rURL );



	// please don't use this!
};

}//end of namespace binfilter
#endif // #ifndef _SFX_HELPER_HXX


