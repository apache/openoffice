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


#ifndef _SRCEDTW_HXX
#define _SRCEDTW_HXX

#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif




class ScrollBar; 
namespace binfilter {


class SwSrcView;
class TextEngine;
class ExtTextView;
class DataChangedEvent;

class TextViewOutWin : public Window
{
	ExtTextView*	pTextView;

protected:
	virtual void	Paint( const Rectangle& );
	virtual void	KeyInput( const KeyEvent& rKeyEvt );
	virtual void	MouseMove( const MouseEvent& rMEvt );
	virtual void	MouseButtonDown( const MouseEvent& rMEvt );
	virtual void	MouseButtonUp( const MouseEvent& rMEvt );
	virtual void	Command( const CommandEvent& rCEvt );
	virtual void	DataChanged( const DataChangedEvent& );

public:
		TextViewOutWin(Window* pParent, WinBits nBits) :
			Window(pParent, nBits), pTextView(0){}

	void	SetTextView( ExtTextView* pView ) {pTextView = pView;}

};

//------------------------------------------------------------
class SourceViewConfig;

} //namespace binfilter
#endif
