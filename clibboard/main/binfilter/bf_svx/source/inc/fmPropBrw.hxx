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


#ifndef SVX_FMPROPBRW_HXX
#define SVX_FMPROPBRW_HXX

#ifndef _COM_SUN_STAR_AWT_XCONTROLCONTAINER_HPP_ 
#include <com/sun/star/awt/XControlContainer.hpp>
#endif
#ifndef _SFX_CHILDWIN_HXX 
#include <bf_sfx2/childwin.hxx>
#endif
namespace binfilter {

//========================================================================
class FmPropBrwMgr : public SfxChildWindow
{
protected:
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >
					m_xUnoRepresentation;
public:
	FmPropBrwMgr(Window *pParent, sal_uInt16 nId, SfxBindings *pBindings, SfxChildWinInfo *pInfo);
	SFX_DECL_CHILDWINDOW(FmPropBrwMgr);
};

class FmPropControl;
class SfxBindings;
//========================================================================
}//end of namespace binfilter
#endif //SVX_FMPROPBRW_HXX
