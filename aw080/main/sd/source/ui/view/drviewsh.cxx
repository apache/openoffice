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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sd.hxx"

#include "DrawViewShell.hxx"
#include <svl/aeitem.hxx>
#include <svl/itemset.hxx>
#include <sfx2/request.hxx>
#ifndef _SVXIDS_HRC
#include <svx/svxids.hrc>
#endif


#include <svx/fmshell.hxx>
#include <sfx2/dispatch.hxx>

#include "app.hrc"
#include "strings.hrc"
#include "sdpage.hxx"
#ifndef SD_FRAME_VIEW
#include "FrameView.hxx"
#endif
#include "sdresid.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "Window.hxx"
#include "GraphicViewShell.hxx"
#include "drawview.hxx"

#include "slideshow.hxx"

namespace sd {

#define TABCONTROL_INITIAL_SIZE 	500

/*************************************************************************
|*
|* Sprung zu Bookmark
|*
\************************************************************************/

bool DrawViewShell::GotoBookmark(const String& rBookmark)
{
    bool bRet = false;
    ::sd::DrawDocShell* pDocSh = GetDocSh();
    if( pDocSh )
    {
        if( !pDocSh->GetViewShell() ) //#i26016# this case occurs if the jump-target-document was opened already with file open dialog before triggering the jump via hyperlink
            pDocSh->Connect(this);
	    bRet = (pDocSh->GotoBookmark(rBookmark));
    }
    return bRet;
}

/*************************************************************************
|*
|* Bereich sichtbar machen (Bildausschnitt scrollen)
|*
\************************************************************************/

void DrawViewShell::MakeVisibleAtView(const basegfx::B2DRange& rRange, ::Window& rWin)
{
	// #98568# In older versions, if in X or Y the size of the object was
	// smaller than the visible area, the user-defined zoom was
	// changed. This was decided to be a bug for 6.x, thus I developed a
	// version which instead handles X/Y bigger/smaller and visibility
	// questions seperately. The new behaviour is triggered with the
	// bZoomAllowed parameter which for old behaviour should be set to
	// sal_True. I looked at all uses of MakeVisibleAtView() in the application
	// and found no valid reason for really changing the zoom factor, thus I
	// decided to NOT expand (incompatible) this virtual method to get one
	// more parameter. If this is wanted in later versions, feel free to add
	// that bool to the parameter list.
	sal_Bool bZoomAllowed(sal_False);
	const basegfx::B2DVector aLogicSize(rRange.getRange());

	// Sichtbarer Bereich
	const Size aVisSizePixel(rWin.GetOutputSizePixel());
	basegfx::B2DRange aVisArea(rWin.GetInverseViewTransformation() *
		basegfx::B2DRange(0.0, 0.0, aVisSizePixel.Width(), aVisSizePixel.Height()));

	if(!aVisArea.isInside(rRange) && !SlideShow::IsRunning( GetViewShellBase() ) )
	{
		// Objekt liegt nicht komplett im sichtbaren Bereich
		basegfx::B2DVector aFreeSpace(aVisArea.getRange() - aLogicSize);

		if(bZoomAllowed && (aFreeSpace.getX() < 0.0 || aFreeSpace.getY() < 0.0))
		{
			// Objekt passt nicht in sichtbaren Bereich -> auf Objektgroesse zoomen
			SetZoomRange(rRange);
		}
		else
		{
			// #98568# allow a mode for move-only visibility without zooming.
			const double fPercentBorder(30.0 / 200.0);
			const basegfx::B2DRange aInnerRange(
				aVisArea.getMinX() + (aVisArea.getWidth() * fPercentBorder),
				aVisArea.getMinY() + (aVisArea.getHeight() * fPercentBorder),
				aVisArea.getMaxX() - (aVisArea.getWidth() * fPercentBorder),
				aVisArea.getMaxY() - (aVisArea.getHeight() * fPercentBorder));
			basegfx::B2DPoint aNewPos(aVisArea.getMinimum());

			if(aFreeSpace.getX() < 0.0)
			{
				if(aInnerRange.getMinX() > rRange.getMaxX())
				{
					// object moves out to the left
					aNewPos.setX(aNewPos.getX() - (aVisArea.getWidth() * 0.5));
				}

				if(aInnerRange.getMaxX() < rRange.getMinX())
				{
					// object moves out to the right
					aNewPos.setX(aNewPos.getX() + (aVisArea.getWidth() * 0.5));
				}
			}
			else
			{
				if(aFreeSpace.getX() > rRange.getWidth())
					aFreeSpace.setX(rRange.getWidth());

				while(rRange.getMaxX() > aNewPos.getX() + aVisArea.getWidth())
					aNewPos.setX(aNewPos.getX() + aFreeSpace.getX());
				
				while(rRange.getMinX() < aNewPos.getX())
					aNewPos.setX(aNewPos.getX() - aFreeSpace.getX());
			}

			if(aFreeSpace.getY() < 0.0)
			{
				if(aInnerRange.getMinY() > rRange.getMaxY())
				{
					// object moves out to the top
					aNewPos.setY(aNewPos.getY() - (aVisArea.getHeight() * 0.5));
				}

				if(aInnerRange.getMaxY() < rRange.getMinY())
				{
					// object moves out to the right
					aNewPos.setY(aNewPos.getY() + (aVisArea.getHeight() * 0.5));
				}
			}
			else
			{
				if(aFreeSpace.getX() > rRange.getHeight())
					aFreeSpace.setY(rRange.getHeight());

				while(rRange.getMaxY() > aNewPos.getY() + aVisArea.getHeight())
					aNewPos.setY(aNewPos.getY() + aFreeSpace.getY());
				
				while(rRange.getMinY() < aNewPos.getY())
					aNewPos.setY(aNewPos.getY() - aFreeSpace.getY());
			}

			// did position change? Does it need to be set?
			if(!aNewPos.equal(aVisArea.getMinimum()))
			{
				aVisArea = basegfx::B2DRange(aNewPos, aNewPos + aVisArea.getRange());

				SetZoomRange(aVisArea);
			}
		}
	}
}

}
