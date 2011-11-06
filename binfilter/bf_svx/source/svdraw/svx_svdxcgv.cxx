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



#include <vector>
#include "svdxcgv.hxx"
#include "svdstr.hrc"   // Namen aus der Resource


// #i13033#
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrExchangeView::SdrExchangeView(SdrModel* pModel1, OutputDevice* pOut):
/*N*/ 	SdrObjEditView(pModel1,pOut)
/*N*/ {
/*N*/ }

/*?*/ GDIMetaFile SdrExchangeView::GetMarkedObjMetaFile( BOOL bNoVDevIfOneMtfMarked ) const
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); GDIMetaFile aMtf; return aMtf;//STRIP001 
/*?*/ }
}
