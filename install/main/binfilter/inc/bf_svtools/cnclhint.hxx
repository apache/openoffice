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


#ifndef _SFXCNCLHINT_HXX
#define _SFXCNCLHINT_HXX

#ifndef _SFXHINT_HXX
#include <bf_svtools/hint.hxx>
#endif

#ifndef _SFXCANCEL_HXX
#include <bf_svtools/cancel.hxx>
#endif

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

namespace binfilter {

#define SFXCANCELHINT_REMOVED		1

class SfxCancelHint: public SfxHint
{
private:
	SfxCancellable* pCancellable;
	USHORT			nAction;
public:
	TYPEINFO();
	SfxCancelHint( SfxCancellable*, USHORT nAction );
	USHORT GetAction() const { return nAction; }
	const SfxCancellable& GetCancellable() const { return *pCancellable; }
};

}

#endif
