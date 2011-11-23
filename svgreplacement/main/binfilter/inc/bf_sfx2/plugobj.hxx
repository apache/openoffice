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



#ifndef _PLUGOBJ_HXX
#define _PLUGOBJ_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _PLUGIN_HXX //autogen
#include <bf_so3/plugin.hxx>
#endif
#include "bf_sfx2/app.hxx"

namespace binfilter {

//=========================================================================
struct SfxPluginObject_Impl;
class SfxFrameDescriptor;


struct SfxPluginObjectFactoryPtr
{
	// Ist n"otig, da im SO2_DECL_BASIC_CLASS_DLL-Macro ein Pointer auf
	// eine exportierbare struct/class "ubergeben werden mu\s
	SotFactory *pSfxPluginObjectFactory;
	SfxPluginObjectFactoryPtr();
};

class SfxPluginObject : public SvPlugInObject
/*	[Beschreibung]

*/
{
	SfxPluginObject_Impl*	pImpl;

#if _SOLAR__PRIVATE
	DECL_STATIC_LINK( SfxPluginObject, MIMEAvailable_Impl, String* );
#endif

protected:
	virtual void    		FillClass( SvGlobalName * pClassName,
							   ULONG * pFormat,
							   String * pAppName,
							   String * pFullTypeName,
							   String * pShortTypeName ) const;

							// Protokoll
	virtual void    		InPlaceActivate( BOOL );

							~SfxPluginObject();
public:

	static SfxPluginObjectFactoryPtr*
							GetFactoryPtr();
							SfxPluginObject();

							SO2_DECL_BASIC_CLASS_DLL(SfxPluginObject, GetFactoryPtr())
};

inline SfxPluginObjectFactoryPtr::SfxPluginObjectFactoryPtr()
	: pSfxPluginObjectFactory(0) 	// sonst funzt ClassFactory() nicht!
{
}

SO2_DECL_IMPL_REF(SfxPluginObject)


}//end of namespace binfilter
#endif
