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
#include "precompiled_svx.hxx"

#include <svx/sdr/contact/viewcontactofsdrole2obj.hxx>
#include <svx/svdoole2.hxx>
#include <svx/sdr/contact/viewobjectcontactofsdrole2obj.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <svx/sdr/primitive2d/sdrole2primitive2d.hxx>
#include <drawinglayer/primitive2d/graphicprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <svtools/colorcfg.hxx>
#include <svx/sdr/primitive2d/sdrattributecreator.hxx>
#include <vcl/svapp.hxx>
#include <svx/sdr/primitive2d/sdrolecontentprimitive2d.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
	namespace contact
	{
		// Create a Object-Specific ViewObjectContact, set ViewContact and
		// ObjectContact. Always needs to return something.
		ViewObjectContact& ViewContactOfSdrOle2Obj::CreateObjectSpecificViewObjectContact(ObjectContact& rObjectContact)
		{
			ViewObjectContact* pRetval = new ViewObjectContactOfSdrOle2Obj(rObjectContact, *this);
			DBG_ASSERT(pRetval, "ViewContact::CreateObjectSpecificViewObjectContact() failed (!)");

			return *pRetval;
		}

		ViewContactOfSdrOle2Obj::ViewContactOfSdrOle2Obj(SdrOle2Obj& rOle2Obj)
		:	ViewContactOfSdrRectObj(rOle2Obj)
		{
		}

		ViewContactOfSdrOle2Obj::~ViewContactOfSdrOle2Obj()
		{
		}

        drawinglayer::primitive2d::Primitive2DSequence ViewContactOfSdrOle2Obj::createPrimitive2DSequenceWithParameters(
            bool bHighContrast) const
        {
			drawinglayer::primitive2d::Primitive2DSequence xRetval;

			// get object transformation
			const basegfx::B2DHomMatrix& rObjectMatrix(GetOle2Obj().getSdrObjectTransformation());

		    // Prepare attribute settings, will be used soon anyways
			const SfxItemSet& rItemSet = GetOle2Obj().GetMergedItemSet();
			const drawinglayer::attribute::SdrLineFillShadowTextAttribute aAttribute(
				drawinglayer::primitive2d::createNewSdrLineFillShadowTextAttribute(
                    rItemSet, 
                    GetOle2Obj().getText(0)));

            // #i102063# embed OLE content in an own primitive; this will be able to decompose accessing
            // the weak SdrOle2 reference and will also implement getB2DRange() for fast BoundRect
            // calculations without OLE Graphic access (which may trigger e.g. chart recalculation).
            // It will also take care of HighContrast and ScaleContent
            const drawinglayer::primitive2d::Primitive2DReference xOleContent(
                new drawinglayer::primitive2d::SdrOleContentPrimitive2D(
                    GetOle2Obj(),
                    rObjectMatrix,

                    // #i104867# add GraphicVersion number to be able to check for
                    // content change in the primitive later
                    GetOle2Obj().getEmbeddedObjectRef().getGraphicVersion(),

                    bHighContrast));
            
            // create primitive. Use Ole2 primitive here. Prepare attribute settings, will 
			// be used soon anyways. Always create primitives to allow the decomposition of 
			// SdrOle2Primitive2D to create needed invisible elements for HitTest and/or BoundRect
            const drawinglayer::primitive2d::Primitive2DSequence xOLEContent(&xOleContent, 1);
            const drawinglayer::primitive2d::Primitive2DReference xReference(
                new drawinglayer::primitive2d::SdrOle2Primitive2D(
			        xOLEContent,
			        rObjectMatrix, 
			        aAttribute));

			xRetval = drawinglayer::primitive2d::Primitive2DSequence(&xReference, 1);

			return xRetval;
        }

		drawinglayer::primitive2d::Primitive2DSequence ViewContactOfSdrOle2Obj::createViewIndependentPrimitive2DSequence() const
		{
            // do as if no HC and call standard creator
            return createPrimitive2DSequenceWithParameters(false);
		}
	} // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
