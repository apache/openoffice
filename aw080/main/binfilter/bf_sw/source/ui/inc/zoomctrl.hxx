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


#ifndef _ZOOMCTRL_HXX
#define _ZOOMCTRL_HXX

#ifndef _SVX_ZOOMCTRL_HXX //autogen
#include <bf_svx/zoomctrl.hxx>
#endif
namespace binfilter {

class SwZoomControl : public SvxZoomStatusBarControl
{
private:
	String  sPreviewZoom;
public:
	virtual void	StateChanged( USHORT nSID, SfxItemState eState,
								  const SfxPoolItem* pState );

	SFX_DECL_STATUSBAR_CONTROL();

	SwZoomControl( USHORT nId, StatusBar& rStb, SfxBindings& rBind );
	~SwZoomControl();

};


} //namespace binfilter
#endif


