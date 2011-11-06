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



#ifndef _SV_PRIVSPLT_HXX
#define	_SV_PRIVSPLT_HXX

#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif

namespace binfilter
{

enum PRIVSPLIT_DIRECTION {PRIVSPLIT_HORZ,PRIVSPLIT_VERT };

class SvPrivatSplit : public FixedLine
{
	private:

		Link				aCtrModifiedLink;
		BOOL				aMovingFlag;
		Pointer				aWinPointer;		
		PRIVSPLIT_DIRECTION	eAdrSplit;
		short				nOldX;
		short				nOldY;
		short				nNewX;
		short				nNewY;
		short				nMinPos;
		short				nMaxPos;
		Range				aXMovingRange;
		Range				aYMovingRange;
		short				nDeltaX;
		short				nDeltaY;
		void				ImplInitSettings( BOOL bFont, BOOL bForeground, BOOL bBackground );

	protected:
		virtual void		MouseButtonDown( const MouseEvent& rMEvt );
		virtual void		Tracking( const TrackingEvent& rTEvt );

	public:

		SvPrivatSplit( Window* pWindow, const ResId& rResId,PRIVSPLIT_DIRECTION eAdrSplit);

		SvPrivatSplit( Window* pParent,PRIVSPLIT_DIRECTION eAdrSplit, WinBits nStyle = 0);

		virtual short	GetDeltaX();
		virtual short	GetDeltaY();
		
		virtual void    CtrModified();

		void			SetXRange(Range cRgeX);
		void			SetYRange(Range cRgeY);

		void			MoveSplitTo(Point aPos);

		virtual	void	StateChanged( StateChangedType nType );
		virtual	void	DataChanged( const DataChangedEvent& rDCEvt );

		void            SetCtrModifiedHdl( const Link& rLink ) { aCtrModifiedLink = rLink; }
		const Link&     GetCtrModifiedHdl() const { return aCtrModifiedLink; }
};

}

#endif
