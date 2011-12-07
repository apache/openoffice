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

#include <svgio/svgreader/svgimagenode.hxx>
#include <sax/tools/converter.hxx>
#include <tools/stream.hxx>
#include <vcl/bitmapex.hxx>
#include <svtools/filter.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <drawinglayer/primitive2d/groupprimitive2d.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgImageNode::SvgImageNode(
            SvgDocument& rDocument,
            SvgNode* pParent)
        :   SvgNode(SVGTokenRect, rDocument, pParent),
            maSvgStyleAttributes(*this),
            maSvgAspectRatio(),
            mpaTransform(0),
            maX(0),
            maY(0),
            maWidth(0),
            maHeight(0),
            maXLink(),
            maUrl(),
            maMimeType(),
            maData()
        {
        }

        SvgImageNode::~SvgImageNode()
        {
            if(mpaTransform) delete mpaTransform;
        }

        const SvgStyleAttributes* SvgImageNode::getSvgStyleAttributes() const
        {
            static rtl::OUString aClassStr(rtl::OUString::createFromAscii("image"));
            maSvgStyleAttributes.checkForCssStyle(aClassStr);

            return &maSvgStyleAttributes;
        }

        void SvgImageNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
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
                case SVGTokenPreserveAspectRatio:
                {
                    setSvgAspectRatio(readSvgAspectRatio(aContent));
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
                case SVGTokenXlinkHref:
                {
                    const sal_Int32 nLen(aContent.getLength());

                    if(nLen)
                    {
                        readImageLink(aContent, maXLink, maUrl, maMimeType, maData);
                    }
                    break;
                }
            }
        }

        void SvgImageNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DVector& rTarget, bool bReferenced) const
        {
            // get size range and create path
            const SvgStyleAttributes* pStyle = getSvgStyleAttributes();

            if(pStyle && getWidth().isSet() && getHeight().isSet())
            {
                const double fWidth(getWidth().solve(*this, xcoordinate));
                const double fHeight(getHeight().solve(*this, ycoordinate));

                if(fWidth > 0.0 && fHeight > 0.0)
                {
                    BitmapEx aBitmapEx;
                    drawinglayer::primitive2d::Primitive2DVector aNewTarget;
                    drawinglayer::primitive2d::Primitive2DSequence aEmbedded;

                    // prepare Target and ViewBox for evtl. AspectRatio mappings
                    const double fX(getX().isSet() ? getX().solve(*this, xcoordinate) : 0.0);
                    const double fY(getY().isSet() ? getY().solve(*this, ycoordinate) : 0.0);
                    const basegfx::B2DRange aTarget(fX, fY, fX + fWidth, fY + fHeight);
                    basegfx::B2DRange aViewBox(aTarget);

                    if(maMimeType.getLength() && maData.getLength())
                    {
                        // use embedded base64 encoded data
                        ::com::sun::star::uno::Sequence< sal_Int8 > aPass;
                        ::sax::Converter::decodeBase64(aPass, maData);

                        if(aPass.hasElements())
                        {
                            SvMemoryStream aStream(aPass.getArray(), aPass.getLength(), STREAM_READ);
                            Graphic aGraphic;

                            if(GRFILTER_OK == GraphicFilter::GetGraphicFilter()->ImportGraphic(
                                aGraphic, 
                                String(), 
                                aStream))
                            {
                                if(GRAPHIC_BITMAP == aGraphic.GetType())
                                {
                                    if(aGraphic.getSvgData().get())
                                    {
                                        // embedded Svg
                                        aEmbedded = aGraphic.getSvgData()->getPrimitive2DSequence();

                                        // fill aViewBox
                                        aViewBox = aGraphic.getSvgData()->getRange();
                                    }
                                    else
                                    {
                                        // get bitmap
                                        aBitmapEx = aGraphic.GetBitmapEx();
                                    }
                                }
                                else
                                {
                                    // evtl. convert to bitmap
                                    aBitmapEx = aGraphic.GetBitmapEx();
                                }
                            }
                        }
                    }

                    if(!aBitmapEx.IsEmpty())
                    {
                        // create content from bitmap
                        aNewTarget.push_back(
                            new drawinglayer::primitive2d::BitmapPrimitive2D(
                                aBitmapEx,
                                basegfx::B2DHomMatrix()));

                        // fill aViewBox. No size set yet, use unit size
                        aViewBox = basegfx::B2DRange(0.0, 0.0, 1.0, 1.0);
                    }
                    else if(aEmbedded.hasElements())
                    {
                        // add to aNewTarget as Group
                        aNewTarget.push_back(
                            new drawinglayer::primitive2d::GroupPrimitive2D(
                                aEmbedded));
                    }

                    if(aNewTarget.size())
                    {
                        if(aTarget.equal(aViewBox))
                        {
                            // just add to rTarget
                            rTarget.insert(rTarget.end(), aNewTarget.begin(), aNewTarget.end());
                        }
                        else
                        {
                            // create mapping
                            const SvgAspectRatio& rRatio = getSvgAspectRatio();
                            const drawinglayer::primitive2d::Primitive2DSequence aSequence(Primitive2DVectorToPrimitive2DSequence(aNewTarget));

                            if(rRatio.isSet())
                            {
                                // let mapping be created from SvgAspectRatio
                                const basegfx::B2DHomMatrix aEmbeddingTransform(
                                    rRatio.createMapping(aTarget, aViewBox));

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
                                        aTarget, aViewBox));
                                    
                                // embed in transformation
                                rTarget.push_back(
                                    new drawinglayer::primitive2d::TransformPrimitive2D(
                                        aEmbeddingTransform,
                                        aSequence));
                            }
                        }
                    }
                }
            }
        }

    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////
// eof
