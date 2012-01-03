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


#ifndef _DPAGE_HXX
#define _DPAGE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FM_FMPAGE_HXX
#include <bf_svx/fmpage.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include <bf_svx/svdobj.hxx>
#endif
namespace binfilter {

class SdrPageGridFrameList;
class SwDrawDocument;
class SwDoc;

class SwDPage : public FmFormPage, public SdrObjUserCall
{
	SdrPageGridFrameList*	pGridLst;
	SwDoc& 					rDoc;

public:
	SwDPage(SwDrawDocument& rNewModel, BOOL bMasterPage=FALSE);
	~SwDPage();

	// #i3694#
	// This GetOffset() method is not needed anymore, it even leads to errors.
	// virtual Point GetOffset() const;
	virtual SdrObject* ReplaceObject( SdrObject* pNewObj, ULONG nObjNum );





	virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createUnoPage();
};

} //namespace binfilter
#endif	   // _DPAGE_HXX



