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


#define UNICODE

#include <string.h>
#include "ddeimp.hxx"
#include <bf_svtools/svdde.hxx>

namespace binfilter
{

// --- DdeInternal::InfCallback() ----------------------------------

#ifdef WNT
HDDEDATA CALLBACK DdeInternal::InfCallback(
				WORD, WORD, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD )
#else
#if defined ( MTW ) || ( defined ( GCC ) && defined ( OS2 )) || defined( ICC )
HDDEDATA CALLBACK __EXPORT DdeInternal::InfCallback(
				WORD, WORD, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD )
#else
HDDEDATA CALLBACK _export DdeInternal::InfCallback(
				WORD, WORD, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD )
#endif
#endif
{
	return (HDDEDATA)DDE_FNOTPROCESSED;
}

}
