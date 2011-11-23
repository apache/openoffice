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


#ifdef _MSC_VER
#pragma hdrstop
#endif





#ifndef _FM_FMOBJFAC_HXX
#include "fmobjfac.hxx"
#endif

#ifndef _FM_FMGLOB_HXX
#include "fmglob.hxx"
#endif

#ifndef _FM_FMOBJ_HXX
#include "fmobj.hxx"
#endif

#ifndef _COM_SUN_STAR_SDBC_XROWSETLISTENER_HPP_
#include <com/sun/star/sdbc/XRowSetListener.hpp>
#endif

#ifndef _COM_SUN_STAR_SDB_XSQLQUERYCOMPOSERFACTORY_HPP_
#include <com/sun/star/sdb/XSQLQueryComposerFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _SVX_FMSHIMP_HXX
#include "fmshimp.hxx"
#endif


#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif


#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif



#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif



#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif

namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::binfilter::svxform;//STRIP008 using namespace ::svxform;

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
/*N*/ FmFormObjFactory::FmFormObjFactory()
/*N*/ {
/*N*/ 	SdrObjFactory::InsertMakeObjectHdl(LINK(this, FmFormObjFactory, MakeObject));
/*N*/ 
/*N*/ 	// ImplSmartRegisterUnoServices();
/*N*/ 
/*N*/ }


/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/
/*N*/ FmFormObjFactory::~FmFormObjFactory()
/*N*/ {
/*N*/ }


/*************************************************************************
|*
|* ::com::sun::star::form::Form-Objekte erzeugen
|*
\************************************************************************/

/*N*/ IMPL_LINK(FmFormObjFactory, MakeObject, SdrObjFactory*, pObjFactory)
/*N*/ {
/*N*/ 	if (pObjFactory->nInventor == FmFormInventor)
/*N*/ 	{
/*N*/ 		switch (pObjFactory->nIdentifier)
/*N*/ 		{
/*N*/ 			case OBJ_FM_CONTROL:	// allgemeines Object
/*N*/ 			{
/*N*/ 				pObjFactory->pNewObj = new FmFormObj(pObjFactory->nIdentifier);
/*N*/ 			}	break;
/*?*/ 			default:
/*?*/ 				{DBG_BF_ASSERT(0, "STRIP");}//STRIP001 return 0;
/*?*/ 		}
/*?*/ 	}
/*N*/ 
/*N*/ 	return 0;
/*N*/ }



}
