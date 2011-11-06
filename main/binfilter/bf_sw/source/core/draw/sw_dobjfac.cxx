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




#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _DOBJFAC_HXX
#include <dobjfac.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
namespace binfilter {

SwObjectFactory aSwObjectFactory;

/*************************************************************************
|*
|* void SwObjectFactory::MakeObject()
|*
\************************************************************************/

/*N*/ IMPL_LINK( SwObjectFactory, MakeObject, SdrObjFactory*, pObjFactory )
/*N*/ {
/*N*/ 	if ( pObjFactory->nInventor == SWGInventor )
/*N*/ 	{
/*N*/ 		//Kein switch, derzeit gibt es nur einen.
/*?*/ 		ASSERT( pObjFactory->nIdentifier == SwFlyDrawObjIdentifier,
/*?*/ 										"Falscher Inventor oder identifier." );
/*?*/ 		pObjFactory->pNewObj = new SwFlyDrawObj();
/*N*/ 	}
/*N*/ 	return 0;
/*N*/ }


}
