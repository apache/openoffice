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

#include <svgio/svgreader/svgmasknode.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <drawinglayer/primitive2d/transparenceprimitive2d.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgMaskNode::SvgMaskNode(
            SvgDocument& rDocument,
            SvgNode* pParent)
        :   SvgNode(SVGTokenMask, rDocument, pParent),
            maSvgStyleAttributes(*this),
            maX(SvgNumber(-10.0, Unit_percent, true)),
            maY(SvgNumber(-10.0, Unit_percent, true)),
            maWidth(SvgNumber(120.0, Unit_percent, true)),
            maHeight(SvgNumber(120.0, Unit_percent, true)),
            mpaTransform(0),
            maMaskUnits(objectBoundingBox),
            maMaskContentUnits(userSpaceOnUse)
        {
        }

        SvgMaskNode::~SvgMaskNode()
        {
            if(mpaTransform) delete mpaTransform;
        }

        const SvgStyleAttributes* SvgMaskNode::getSvgStyleAttributes() const
        {
            static rtl::OUString aClassStr(rtl::OUString::createFromAscii("mask"));
            maSvgStyleAttributes.checkForCssStyle(aClassStr);

            return &maSvgStyleAttributes;
        }

        void SvgMaskNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
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
                case SVGTokenTransform:
                {
                    const basegfx::B2DHomMatrix aMatrix(readTransform(aContent, *this));

                    if(!aMatrix.isIdentity())
                    {
                        setTransform(&aMatrix);
                    }
                    break;
                }
                case SVGTokenMaskUnits:
                {
                    if(aContent.getLength())
                    {
                        if(aContent.match(commonStrings::aStrUserSpaceOnUse, 0))
                        {
                            setMaskUnits(userSpaceOnUse);
                        }
                        else if(aContent.match(commonStrings::aStrObjectBoundingBox, 0))
                        {
                            setMaskUnits(objectBoundingBox);
                        }
                    }
                    break;
                }
                case SVGTokenMaskContentUnits:
                {
                    if(aContent.getLength())
                    {
                        if(aContent.match(commonStrings::aStrUserSpaceOnUse, 0))
                        {
                            setMaskContentUnits(userSpaceOnUse);
                        }
                        else if(aContent.match(commonStrings::aStrObjectBoundingBox, 0))
                        {
                            setMaskContentUnits(objectBoundingBox);
                        }
                    }
                    break;
                }
            }
        }

        void SvgMaskNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DVector& rTarget, bool bReferenced) const
        {
            drawinglayer::primitive2d::Primitive2DVector aNewTarget;

            // decompose childs
            SvgNode::decomposeSvgNode(aNewTarget, bReferenced);

            if(!aNewTarget.empty())
            {
                if(getTransform())
                {
                    // create embedding group element with transformation
                    rTarget.push_back(
                        new drawinglayer::primitive2d::TransformPrimitive2D(
                            *getTransform(),
                            Primitive2DVectorToPrimitive2DSequence(aNewTarget)));
                    aNewTarget.clear();
                }
                else
                {
                    // append to current target
                    rTarget.insert(rTarget.end(), aNewTarget.begin(), aNewTarget.end());
                }
            }
        }

        void SvgMaskNode::apply(drawinglayer::primitive2d::Primitive2DVector& rTarget) const
        {
            if(rTarget.size())
            {
                drawinglayer::primitive2d::Primitive2DVector aMaskTarget;

                // get mask definition as primitives
                decomposeSvgNode(aMaskTarget, true);

                if(aMaskTarget.size())
                {
                    // put content and clip definition to primitive sequence
                    const drawinglayer::primitive2d::Primitive2DSequence aContent(drawinglayer::primitive2d::Primitive2DVectorToPrimitive2DSequence(rTarget));
                    drawinglayer::primitive2d::Primitive2DSequence aMask(drawinglayer::primitive2d::Primitive2DVectorToPrimitive2DSequence(aMaskTarget));

                    // get range of content to be masked
                    const basegfx::B2DRange aContentRange(
                        drawinglayer::primitive2d::getB2DRangeFromPrimitive2DSequence(
                            aContent,
                            drawinglayer::geometry::ViewInformation2D()));
                    const double fContentWidth(aContentRange.getWidth());
                    const double fContentHeight(aContentRange.getHeight());

                    if(fContentWidth > 0.0 && fContentHeight > 0.0)
                    {
                        // create OffscreenBufferRange
                        basegfx::B2DRange aOffscreenBufferRange;

                        if(objectBoundingBox == getMaskUnits())
                        {
                            // fractions or percentages of the bounding box of the element to which the mask is applied
                            const double fX(Unit_percent == getX().getUnit() ? getX().getNumber() * 0.01 : getX().getNumber());
                            const double fY(Unit_percent == getY().getUnit() ? getY().getNumber() * 0.01 : getY().getNumber());
                            const double fW(Unit_percent == getWidth().getUnit() ? getWidth().getNumber() * 0.01 : getWidth().getNumber());
                            const double fH(Unit_percent == getHeight().getUnit() ? getHeight().getNumber() * 0.01 : getHeight().getNumber());

                            aOffscreenBufferRange = basegfx::B2DRange(
                                aContentRange.getMinX() + (fX * fContentWidth), 
                                aContentRange.getMinY() + (fY * fContentHeight),
                                aContentRange.getMinX() + ((fX + fW) * fContentWidth), 
                                aContentRange.getMinY() + ((fY + fH) * fContentHeight));
                        }
                        else
                        {
                            const double fX(getX().isSet() ? getX().solve(*this, xcoordinate) : 0.0);
                            const double fY(getY().isSet() ? getY().solve(*this, ycoordinate) : 0.0);

                            aOffscreenBufferRange = basegfx::B2DRange(
                                fX,
                                fY,
                                fX + (getWidth().isSet() ? getWidth().solve(*this, xcoordinate) : 0.0),
                                fY + (getHeight().isSet() ? getHeight().solve(*this, ycoordinate) : 0.0));
                        }

                        if(objectBoundingBox == getMaskContentUnits())
                        {
                            // mask is object-relative, embed in content transformation
                            const drawinglayer::primitive2d::Primitive2DReference xTransform(
                                new drawinglayer::primitive2d::TransformPrimitive2D(
                                    basegfx::tools::createScaleTranslateB2DHomMatrix(
                                        aContentRange.getRange(),
                                        aContentRange.getMinimum()),
                                    aMask));

                            aMask = drawinglayer::primitive2d::Primitive2DSequence(&xTransform, 1);
                        }

                        // embed content to a ModifiedColorPrimitive2D since the definitions
                        // how content is used as alpha is special for Svg
                        {
                            const drawinglayer::primitive2d::Primitive2DReference xInverseMask(
                                new drawinglayer::primitive2d::ModifiedColorPrimitive2D(
                                    aMask,
                                    basegfx::BColorModifier(
                                        basegfx::BColor(0.0, 0.0, 0.0),
                                        0.5,
                                        basegfx::BCOLORMODIFYMODE_LUMINANCE_TO_ALPHA)));

                            aMask = drawinglayer::primitive2d::Primitive2DSequence(&xInverseMask, 1);
                        }

                        // prepare new content
                        drawinglayer::primitive2d::BasePrimitive2D* pNewContent = new drawinglayer::primitive2d::TransparencePrimitive2D(
                            aContent,
                            aMask);

                        // output up to now is defined by aContentRange and mask is oriented
                        // relative to it. It is possible that aOffscreenBufferRange defines
                        // a smaller area. In that case, embed to a mask primitive
                        if(!aOffscreenBufferRange.isInside(aContentRange))
                        {
                            const drawinglayer::primitive2d::Primitive2DReference xContent(pNewContent);

                            pNewContent = new drawinglayer::primitive2d::MaskPrimitive2D(
                                basegfx::B2DPolyPolygon(basegfx::tools::createPolygonFromRect(aOffscreenBufferRange)),
                                drawinglayer::primitive2d::Primitive2DSequence(&xContent, 1));
                        }

                        // redefine target. Use TransparencePrimitive2D with created mask
                        // geometry
                        rTarget.clear();
                        rTarget.push_back(pNewContent);
                    }
                }
                else
                {
                    // An empty clipping path will completely clip away the element that had 
                    // the ‘clip-path’ property applied. (Svg spec)
                    rTarget.clear();
                }
            }
        }

    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////
// eof
