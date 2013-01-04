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



// MyEDITVIEW, wegen exportiertem EditView
#ifndef _MyEDITVIEW_HXX
#define _MyEDITVIEW_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RSCSFX_HXX //autogen
#include <rsc/rscsfx.hxx>
#endif

#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif
class Window;
class Pointer;
class Cursor;
class KeyEvent;
class MouseEvent;
class Rectangle;
class Link;
class Pair;
class Point;
class Range;
class SvStream;
namespace binfilter {
class SvKeyValueIterator;

class SfxStyleSheet; 
class EditEngine;
class ImpEditEngine;
class ImpEditView;
class SvxFieldItem;
class DropEvent;
class CommandEvent;
}//end of namespace binfilter //namespace binfilter
#ifndef _EDITDATA_HXX
#include <bf_svx/editdata.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

namespace com {
namespace sun {
namespace star {
namespace datatransfer {
	class XTransferable;
}}}}

namespace binfilter {
class EditView
{
	friend class EditEngine;
	friend class ImpEditEngine;
	friend class EditSelFunctionSet;

public:	// brauche ich fuer Undo
	ImpEditView*	GetImpEditView() const 		{ return pImpEditView; }

private:
	ImpEditView*	pImpEditView;

					EditView( const EditView& );

public:
	virtual			~EditView();

	EditEngine*		GetEditEngine() const;

	Window*			GetWindow() const;


	void			ShowCursor( BOOL bGotoCursor = TRUE, BOOL bForceVisCursor = TRUE );
	void			HideCursor();

	BOOL			HasSelection() const;
	ESelection		GetSelection() const;
	void			SetSelection( const ESelection& rNewSel );


	const Rectangle&	GetOutputArea() const;

	const Rectangle&	GetVisArea() const;

	void			Cut();
	void			Copy();
	void			Paste();
};

}//end of namespace binfilter
#endif // _MyEDITVIEW_HXX
