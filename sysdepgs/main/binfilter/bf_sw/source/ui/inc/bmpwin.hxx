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



#ifndef _BMPWIN_HXX
#define _BMPWIN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
namespace binfilter {


/*--------------------------------------------------------------------
	Beschreibung:	Extended Page fuer Grafiken
 --------------------------------------------------------------------*/

class BmpWindow : public Window
{
	void Paint(const Rectangle& rRect);
	Graphic	aGraphic;
	Bitmap 	aBmp;
	BOOL  	bHorz : 1;
	BOOL  	bVert : 1;
	BOOL 	bGraphic : 1;
	BOOL  	bLeftAlign : 1;

public:
	BmpWindow(Window* pPar, USHORT nId,
				const Graphic& rGraphic, const Bitmap& rBmp);
	BmpWindow(Window* pParent, const ResId rResId) :
		Window(pParent, rResId), bLeftAlign(TRUE),
		bHorz(FALSE), bVert(FALSE),bGraphic(FALSE){}
	~BmpWindow();
	void MirrorVert(BOOL bMirror) { bVert = bMirror; Invalidate(); }
	void MirrorHorz(BOOL bMirror) { bHorz = bMirror; Invalidate(); }
    void SetGraphic(const Graphic& rGrf);

};

} //namespace binfilter
#endif
