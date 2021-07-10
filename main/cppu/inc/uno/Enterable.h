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



#ifndef INCLUDED_uno_Enterable_h
#define INCLUDED_uno_Enterable_h

#include "uno/environment.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** Generic function type declaration for entering an Environment.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@param context
	@since UDK 3.2.7
*/
typedef void uno_Enterable_enter     (void * context);


/** Generic function type declaration for levaing an Environment.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@param context
	@since UDK 3.2.7
*/
typedef void uno_Enterable_leave     (void * context);


/** Generic function type declaration for calling into an Environment.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@param context
	@param pCallee  the function to be called
	@param pParam   the parameter pointer to be passed to the function
	@since UDK 3.2.7
*/
typedef void uno_Enterable_callInto_v(void * context, uno_EnvCallee * pCallee, va_list * pParam);


/** Generic function type declaration for calling out of an Environment.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@param context
	@param pCallee  the function to be called
	@param pParam   the parameter pointer to be passed to the function
	@since UDK 3.2.7
*/
typedef void uno_Enterable_callOut_v (void * context, uno_EnvCallee * pCallee, va_list * pParam);


/** Generic function type declaration for checking if calling on managed object is
	valid.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@param  context
	@param  ppReason  the reason, in case calling is not valid
	@return           0 == calling is not valid, 1 == calling is valid
	@since UDK 3.2.7
*/
typedef int  uno_Enterable_isValid_v (void * context, rtl_uString ** ppReason);


/** A struct pReserved needs to point to, if implementing a purpose environment.
	(https://wiki.openoffice.org/wiki/Uno/Binary/Spec/Environment_Stack)

	@since UDK 3.2.7
*/
typedef struct
{
	uno_Enterable_enter      * m_enter;
	uno_Enterable_leave      * m_leave;
	uno_Enterable_callInto_v * m_callInto_v;
	uno_Enterable_callOut_v  * m_callOut_v;
	uno_Enterable_isValid_v  * m_isValid;
}
uno_Enterable;


#ifdef __cplusplus
}
#endif

#endif

