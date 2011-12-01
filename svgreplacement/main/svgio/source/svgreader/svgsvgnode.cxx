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
#include "precompiled_svgio.hxx"

#include <svgio/svgreader/svgsvgnode.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgSvgNode::SvgSvgNode(
            SvgDocument& rDocument,
            SvgNode* pParent)
        :   SvgNode(SVGTokenSvg, rDocument, pParent),
            maSvgStyleAttributes(*this),
            mpViewBox(0),
            maSvgAspectRatio(),
            maX(),
            maY(),
            maWidth(),
            maHeight(),
            maVersion()
        {
            if(!getParent())
            {
                // initial fill is black
                maSvgStyleAttributes.setFill(SvgPaint(basegfx::BColor(0.0, 0.0, 0.0), true, true));
            }
        }

        SvgSvgNode::~SvgSvgNode()
        {
            if(mpViewBox) delete mpViewBox;
        }

        const SvgStyleAttributes* SvgSvgNode::getSvgStyleAttributes() const
        {
            return &maSvgStyleAttributes;
        }

        void SvgSvgNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
        {
            // call parent
            SvgNode::parseAttribute(rTokenName, aSVGToken, aContent);

            // read style attributes
            maSvgStyleAttributes.parseStyleAttribute(rTokenName, aSVGToken, aContent);

            // parse own
            switch(aSVGToken)
            {
                case SVGTokenStyle:
                {
                    maSvgStyleAttributes.readStyle(aContent);
                    break;
                }
                case SVGTokenViewBox:
                {
                    const basegfx::B2DRange aRange(readViewBox(aContent, *this));

                    if(!aRange.isEmpty())
                    {
                        setViewBox(&aRange);
                    }
                    break;
                }
                case SVGTokenPreserveAspectRatio:
                {
                    setSvgAspectRatio(readSvgAspectRatio(aContent));
                    break;
                }
                case SVGTokenX:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        setX(aNum);
                    }
                    break;
                }
                case SVGTokenY:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        setY(aNum);
                    }
                    break;
                }
                case SVGTokenWidth:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        if(aNum.isPositive())
                        {
                            setWidth(aNum);
                        }
                    }
                    break;
                }
                case SVGTokenHeight:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        if(aNum.isPositive())
                        {
                            setHeight(aNum);
                        }
                    }
                    break;
                }
                case SVGTokenVersion:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        setVersion(aNum);
                    }
                    break;
                }
            }
        }

        void SvgSvgNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DVector& rTarget, bool bReferenced) const
        {
            drawinglayer::primitive2d::Primitive2DVector aNewTarget;

            // decompose childs
            SvgNode::decomposeSvgNode(aNewTarget, bReferenced);

            if(!aNewTarget.empty())
            {
                // pack into Primitive2DSequence to ensure ownership
                const drawinglayer::primitive2d::Primitive2DSequence aSequence(Primitive2DVectorToPrimitive2DSequence(aNewTarget));

                if(getParent())
                {
                    if(getViewBox())
                    {
                        // Svg defines that with no width or no height the viewBox content is empty,
                        // so both need to exist
                        if(!basegfx::fTools::equalZero(getViewBox()->getWidth()) && !basegfx::fTools::equalZero(getViewBox()->getHeight()))
                        {
                            // create target range homing x,y, width and height as given
                            const double fX(getX().isSet() ? getX().solve(*this, xcoordinate) : 0.0);
                            const double fY(getY().isSet() ? getY().solve(*this, ycoordinate) : 0.0);
                            const double fW(getWidth().isSet() ? getWidth().solve(*this, xcoordinate) : getViewBox()->getWidth());
                            const double fH(getHeight().isSet() ? getHeight().solve(*this, ycoordinate) : getViewBox()->getHeight());
                            const basegfx::B2DRange aTarget(fX, fY, fX + fW, fY + fH);

                            if(aTarget.equal(*getViewBox()))
                            {
                                // no mapping needed, append
                                rTarget.push_back(
                                    new drawinglayer::primitive2d::GroupPrimitive2D(
                                        aSequence));
                            }
                            else
                            {
                                // create mapping
                                const SvgAspectRatio& rRatio = getSvgAspectRatio();

                                if(rRatio.isSet())
                                {
                                    // let mapping be created from SvgAspectRatio
                                    const basegfx::B2DHomMatrix aEmbeddingTransform(
                                        rRatio.createMapping(aTarget, *getViewBox()));

                                    // prepare embedding in transformation
                                    drawinglayer::primitive2d::TransformPrimitive2D* pNew = 
                                        new drawinglayer::primitive2d::TransformPrimitive2D(
                                            aEmbeddingTransform,
                                            aSequence);

                                    if(rRatio.isMeetOrSlice())
                                    {
                                        // embed in transformation
                                        rTarget.push_back(pNew);
                                    }
                                    else
                                    {
                                        // need to embed in MaskPrimitive2D, too
                                        const drawinglayer::primitive2d::Primitive2DReference xRef(pNew);

                                        rTarget.push_back(
                                            new drawinglayer::primitive2d::MaskPrimitive2D(
                                                basegfx::B2DPolyPolygon(basegfx::tools::createPolygonFromRect(aTarget)),
                                                drawinglayer::primitive2d::Primitive2DSequence(&xRef, 1)));
                                    }
                                }
                                else
                                {
                                    // choose default mapping
                                    const basegfx::B2DHomMatrix aEmbeddingTransform(
                                        rRatio.createLinearMapping(
                                            aTarget, *getViewBox()));
                                    
                                    // embed in transformation
                                    rTarget.push_back(
                                        new drawinglayer::primitive2d::TransformPrimitive2D(
                                            aEmbeddingTransform,
                                            aSequence));
                                }
                            }
                        }
                    }
                    else
                    {
                        // no viewBox, append
                        rTarget.push_back(
                            new drawinglayer::primitive2d::GroupPrimitive2D(
                                aSequence));
                    }
                }
                else
                {
                    // Outermost SVG element; create target range homing width and height as given.
                    // SVG defines that x,y has no meanig for the outermost SVG element. Use a fallback
                    // width and height of 8x8 cm (8 * 35.43307px)
                    const double fFallbackMetric(8.0 * 35.43307);
                    double fW(getWidth().isSet() ? getWidth().solve(*this, xcoordinate) : fFallbackMetric);
                    double fH(getHeight().isSet() ? getHeight().solve(*this, ycoordinate) : fFallbackMetric);

                    // Svg defines that a negative value is an error and that 0.0 disables rendering
                    if(basegfx::fTools::more(fW, 0.0) && basegfx::fTools::more(fH, 0.0))
                    {
                        // append embedded in transform primitive to scale to 1/100th mm
                        // where 1 mm == 3.543307 px
                        const double fScaleTo100thmm(100.0 / 3.543307);
                        basegfx::B2DHomMatrix aTransform(
                            basegfx::tools::createScaleB2DHomMatrix(
                                fScaleTo100thmm,
                                fScaleTo100thmm));

                        rTarget.push_back(
                            new drawinglayer::primitive2d::TransformPrimitive2D(
                                aTransform,
                                aSequence));
                    }
                }
            }
        }

        const basegfx::B2DRange* SvgSvgNode::getCurrentViewPort() const
        {
            if(getViewBox())
            {
                return getViewBox();
            }
            else
            {
                return SvgNode::getCurrentViewPort();
            }
        }

    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////
// eof
