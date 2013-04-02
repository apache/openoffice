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

#ifndef SVX_SIDEBAR_LINE_STYLE_CONTROL_HXX
#define SVX_SIDEBAR_LINE_STYLE_CONTROL_HXX

#include "svx/sidebar/PopupControl.hxx"
#include <vcl/button.hxx>
#include "LineStyleValueSet.hxx"


class XLineStyleItem;
class XLineDashItem;
class SfxBindings;
class Image;


namespace svx { namespace sidebar {

class LinePropertyPanel;

class LineStyleControl
    : public svx::sidebar::PopupControl
{
public:
	LineStyleControl (Window* pParent, svx::sidebar::LinePropertyPanel& rPanel);
	virtual ~LineStyleControl (void);
    
	void GetFocus();
	void SetStyleSelect(XLineStyleItem* pStyle, XLineDashItem* pDash, bool bStyleAvailable, bool bDashAvailable);
	virtual void Paint(const Rectangle& rect);
	ValueSet& GetValueSet();
	Control& GetPB();

private:	
	svx::sidebar::LinePropertyPanel&    mrLinePropertyPanel;
	SfxBindings*		                mpBindings;
	LineStyleValueSet maVSStyle;
    PushButton                          maPBOptions;
	Image*	                            maIMG;
	Image*	                            maIMGSel;
	Image*	                            maIMGH;		// high contrast
	bool	                            mbVS;

	void Initialize();
	void SetAllNoSel();

	DECL_LINK( PBStyleClickHdl, PushButton *);
	DECL_LINK(VSSelectHdl, void *);
};

} } // end of namespace svx::sidebar

#endif // SVX_SIDEBAR_LINE_STYLE_CONTROL_HXX

// eof
