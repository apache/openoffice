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


#ifndef _SCH_TRANSFERABLE_HXX_
#define _SCH_TRANSFERABLE_HXX_

#ifndef _TRANSFER_HXX
#include <bf_svtools/transfer.hxx>
#endif

/*N*/ #include <tools/debug.hxx> //for stripping
class Graphic;
class VirtualDevice;
namespace binfilter {
class SdrModel;
class SdrExchangeView;

class SchTransferable :
	public TransferableHelper
{
private:
    SdrExchangeView  *              mpSourceView;

protected:

	// implementation of TransferableHelper methods

public:

    /** CTOR for clipboard, drag and drop and selection clipboard
        @param pObjModel the model containing all objects for the clipboard.
                         The transferable becomes the owner of this model.
        @param pSrcView  the view for drag and drop and the selection clipboard
                         The transferable does not care about destruction of this view
        @param rObjDesc  A descriptor object that holds properties like the starting
                         position of a drag action
        @param bLateInit If true, the actual data is created in GetData (on paste),
                         else data is created on construction
     */
    SchTransferable( SdrModel* pObjModel,
                     SdrExchangeView* pSrcView,
                     const TransferableObjectDescriptor& rObjDesc,
                     sal_Bool bLateInit = sal_False );
  	const SdrExchangeView* GetView() throw()		{ return mpSourceView; }
};

} //namespace binfilter
#endif	// _SCH_TRANSFERABLE_HXX_
