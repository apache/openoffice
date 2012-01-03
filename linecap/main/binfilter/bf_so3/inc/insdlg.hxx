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


#ifndef _INSDLG_HXX
#define _INSDLG_HXX

#ifndef _GLOBNAME_HXX //autogen
#include <tools/globname.hxx>
#endif
#ifndef _OWNCONT_HXX
#include <bf_svtools/ownlist.hxx>
#endif
#ifndef _SO2REF_HXX
#include <bf_so3/so2ref.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

class SvAppletObject;

#ifndef SV_DECL_SVBASELINK2_DEFINED
#define SV_DECL_SVBASELINK2_DEFINED
SV_DECL_REF(SvBaseLink2)
#endif

namespace binfilter
{
#ifndef SO2_DECL_SVINPLACEOBJECT_DEFINED
#define SO2_DECL_SVINPLACEOBJECT_DEFINED
SO2_DECL_REF(SvInPlaceObject)
#endif

/********************** SvObjectServer ***********************************
*************************************************************************/
class SO3_DLLPUBLIC SvObjectServer
{
private:
	SvGlobalName    aClassName;
	String          aHumanName;

public:
	SvObjectServer( const SvGlobalName & rClassP, const String & rHumanP ) :
		aClassName( rClassP ),
		aHumanName( rHumanP ) {}

	const SvGlobalName &    GetClassName() const { return aClassName; }
	const String &          GetHumanName() const { return aHumanName; }
};

class SO3_DLLPRIVATE SvObjectServerList
{
	PRV_SV_DECL_OWNER_LIST(SvObjectServerList,SvObjectServer)
	SO3_DLLPRIVATE const SvObjectServer *  Get( const SvGlobalName & ) const;
	SO3_DLLPRIVATE void                    Remove( const SvGlobalName & );

	SO3_DLLPRIVATE void					FillInsertObjects();
};

}   // namespace so3

#endif // _INSDLG_HXX

