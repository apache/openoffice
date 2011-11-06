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



#ifndef _EDITSTT2_HXX
#define _EDITSTT2_HXX

#include <editstat.hxx>
namespace binfilter {

class InternalEditStatus : public EditStatus
{

public:
			InternalEditStatus() { ; }

	void	TurnOnFlags( ULONG nFlags )
				{ nControlBits |= nFlags; }

	void	TurnOffFlags( ULONG nFlags )
				{ nControlBits &= ~nFlags; }

	void	TurnOnStatusBits( ULONG nBits )
				{ nStatusBits |= nBits; }

	void	TurnOffStatusBits( ULONG nBits )
				{ nStatusBits &= ~nBits; }


	BOOL	UseCharAttribs() const
				{ return ( ( nControlBits & EE_CNTRL_USECHARATTRIBS ) != 0 ); }

	BOOL	NotifyCursorMovements() const
				{ return ( ( nControlBits & EE_CNTRL_CRSRLEFTPARA ) != 0 ); }

	BOOL	UseIdleFormatter() const
				{ return ( ( nControlBits & EE_CNTRL_DOIDLEFORMAT) != 0 ); }

	BOOL	AllowPasteSpecial() const
				{ return ( ( nControlBits & EE_CNTRL_PASTESPECIAL ) != 0 ); }

	BOOL	DoAutoIndenting() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOINDENTING ) != 0 ); }

	BOOL	DoUndoAttribs() const
				{ return ( ( nControlBits & EE_CNTRL_UNDOATTRIBS ) != 0 ); }

	BOOL	OneCharPerLine() const
				{ return ( ( nControlBits & EE_CNTRL_ONECHARPERLINE ) != 0 ); }

	BOOL	IsOutliner() const
				{ return ( ( nControlBits & EE_CNTRL_OUTLINER ) != 0 ); }

	BOOL	IsOutliner2() const
				{ return ( ( nControlBits & EE_CNTRL_OUTLINER2 ) != 0 ); }

	BOOL	IsAnyOutliner() const
				{ return IsOutliner() || IsOutliner2(); }

	BOOL	DoNotUseColors() const
				{ return ( ( nControlBits & EE_CNTRL_NOCOLORS ) != 0 ); }

	BOOL	AllowBigObjects() const
				{ return ( ( nControlBits & EE_CNTRL_ALLOWBIGOBJS ) != 0 ); }

	BOOL	DoStretch() const
				{ return ( ( nControlBits & EE_CNTRL_STRETCHING ) != 0 ); }

	BOOL	AutoPageSize() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOPAGESIZE ) != 0 ); }
	BOOL	AutoPageWidth() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOPAGESIZEX ) != 0 ); }
	BOOL	AutoPageHeight() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOPAGESIZEY ) != 0 ); }

	BOOL	MarkFields() const
				{ return ( ( nControlBits & EE_CNTRL_MARKFIELDS ) != 0 ); }

	BOOL	DoRestoreFont() const
				{ return ( ( nControlBits & EE_CNTRL_RESTOREFONT ) != 0 ); }

	BOOL	DoImportRTFStyleSheets() const
				{ return ( ( nControlBits & EE_CNTRL_RTFSTYLESHEETS ) != 0 ); }

	BOOL	DoDrawRedLines() const
				{ return ( ( nControlBits & EE_CNTRL_NOREDLINES ) == 0 ); }

	BOOL	DoAutoCorrect() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOCORRECT ) != 0 ); }

	BOOL	DoAutoComplete() const
				{ return ( ( nControlBits & EE_CNTRL_AUTOCOMPLETE ) != 0 ); }

	BOOL	DoTabIndenting() const
				{ return ( ( nControlBits & EE_CNTRL_TABINDENTING ) != 0 ); }

	BOOL	DoFormat100() const
				{ return ( ( nControlBits & EE_CNTRL_FORMAT100 ) != 0 ); }
	
	BOOL	ULSpaceSummation() const
				{ return ( ( nControlBits & EE_CNTRL_ULSPACESUMMATION ) != 0 ); }
	
	BOOL	ULSpaceFirstParagraph() const
				{ return ( ( nControlBits & EE_CNTRL_ULSPACEFIRSTPARA ) != 0 ); }
};

}//end of namespace binfilter
#endif // _EDITSTT2_HXX

