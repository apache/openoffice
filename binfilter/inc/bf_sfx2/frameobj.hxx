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



#ifndef _FRAMEOBJ_HXX
#define _FRAMEOBJ_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _IPOBJ_HXX //autogen
#include <bf_so3/ipobj.hxx>
#endif
namespace binfilter {

//=========================================================================
struct SfxFrameObject_Impl;
class SfxFrameDescriptor;

struct SfxFrameObjectFactoryPtr
{
	// Ist n"otig, da im SO2_DECL_BASIC_CLASS_DLL-Macro ein Pointer auf
	// eine exportierbare struct/class "ubergeben werden mu\s
	SotFactory *pSfxFrameObjectFactory;
	SfxFrameObjectFactoryPtr();
};

class SfxFrameObject : public SvInPlaceObject
/*	[Beschreibung]

*/
{
	SfxFrameObject_Impl*	pImpl;

#if _SOLAR__PRIVATE
	DECL_LINK(				NewObjectHdl_Impl, Timer* );
#endif

protected:
	BOOL					ConstructFrame();
							// Protokoll

							// Datenaustausch

							// Laden speichern
	virtual BOOL    		Load( SvStorage * );
	virtual BOOL    		Save();
	virtual BOOL    		SaveAs( SvStorage * );
	virtual BOOL    		SaveCompleted( SvStorage * );

							~SfxFrameObject();
public:

	static SfxFrameObjectFactoryPtr*
							GetFactoryPtr();

							// Macro mu\s exportiert werden, sonst geht das
							// SO2-RTTI schief, da GetFactoryAdress() die
							// Factory-Adresse der Basisklasse liefert.
							// Da das zweite Argument in einer Inline-Funktion
							// verwendet wird, mu\s dieses exportierbare
							// Funktionsaufrufe verwenden, also nicht z.B.
							// SFX_APP()->Get_Impl()
							SO2_DECL_BASIC_CLASS_DLL(SfxFrameObject, GetFactoryPtr())

							SfxFrameObject();

	void					SetFrameDescriptor( const SfxFrameDescriptor*);
	const SfxFrameDescriptor*
							GetFrameDescriptor() const;

#if _SOLAR__PRIVATE
#endif
};

inline SfxFrameObjectFactoryPtr::SfxFrameObjectFactoryPtr()
	: pSfxFrameObjectFactory(0) 	// sonst funzt ClassFactory() nicht!
{
	// hier Factory nicht erzeugen, da GetFactoryPtr() erst nach Beenden dieses
	// ctors einen vern"unftigen Wert liefern kann!
}

SO2_DECL_IMPL_REF(SfxFrameObject)


}//end of namespace binfilter
#endif
