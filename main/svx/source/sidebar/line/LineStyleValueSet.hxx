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
/*
#include <sfx2/sidebar/propertypanel.hrc>
#include <sfx2/sidebar/Theme.hxx>
#include <sfx2/sidebar/ControlFactory.hxx>
#include <LinePropertyPanel.hxx>
#include <LinePropertyPanel.hrc>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/xlnclit.hxx>
#include <svx/xtable.hxx>
#include <svx/xdash.hxx>
#include <svx/drawitem.hxx>
#include <svx/svxitems.hrc>
#include <unotools/pathoptions.hxx>
#include <unotools/viewoptions.hxx>
#include <comphelper/processfactory.hxx>
#include <i18npool/mslangid.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlndsit.hxx>
#include <vcl/svapp.hxx>
#include <svx/xlnwtit.hxx>
#include <vcl/lstbox.hxx>
#include <svx/tbxcolorupdate.hxx>
#include <vcl/toolbox.hxx>
#include <svx/xlntrit.hxx>
#include <svx/xlnstit.hxx>
#include <svx/xlnedit.hxx>
#include <svx/xlncapit.hxx>
#include <svx/xlinjoit.hxx>
#include <svx/sidebar/PopupControl.hxx>
*/
#include <svtools/valueset.hxx>

namespace svx { namespace sidebar {

class LineStyleValueSet
    : public ValueSet
{
public:
    LineStyleValueSet (Window* pParent, const ResId& rResId);
    virtual ~LineStyleValueSet (void);
    
    void SetFirstSelect(bool bSel);
    void SetFirstString(XubString str);
    
	virtual void	UserDraw( const UserDrawEvent& rUDEvt );	

private:
    VirtualDevice* 	pVDev;
	bool			mbSelectFirst;
	XubString		strNone;
};

} } // end of namespace svx::sidebar
