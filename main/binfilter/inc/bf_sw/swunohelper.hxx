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


#ifndef _SWUNOHELPER_HXX
#define _SWUNOHELPER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif


namespace com { namespace sun { namespace star {
	namespace uno {
		class Any;
	}
}}}
class String; 
class SvStrings; 

namespace binfilter {

namespace SWUnoHelper {

// calls over the compherl the getEnumAsInt32 function and handle the
// exceptions.
sal_Int32 GetEnumAsInt32( const ::com::sun::star::uno::Any& rVal );


// methods for UCB actions:
	// delete the file under this URL
BOOL UCB_DeleteFile( const String& rURL );

}

} //namespace binfilter
#endif
