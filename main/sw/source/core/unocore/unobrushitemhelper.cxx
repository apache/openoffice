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
#include "precompiled_sw.hxx"

#include <unobrushitemhelper.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/xgrscit.hxx>
#include <svx/xflbmtit.hxx>
#include <svx/xflbmpit.hxx>
#include <svx/xflbmsxy.hxx>
#include <svx/xflbmsxy.hxx>
#include <svx/xflftrit.hxx>
#include <svx/xsflclit.hxx>
#include <svx/xflbmsli.hxx>
#include <svx/xflbtoxy.hxx>
#include <svx/xflbstit.hxx>
#include <svx/xflboxy.hxx>
#include <svx/xflbckit.hxx>
#include <svx/unoshape.hxx>
#include <hintids.hxx>

//UUUU
void setSvxBrushItemAsFillAttributesToTargetSet(const SvxBrushItem& rBrush, SfxItemSet& rToSet)
{
    if(0xff != rBrush.GetColor().GetTransparency())
    {
        const Color aColor(rBrush.GetColor().GetRGBColor());
        const sal_uInt8 nTransparency(rBrush.GetColor().GetTransparency());

        rToSet.Put(XFillStyleItem(XFILL_SOLID));
        rToSet.Put(XFillColorItem(String(), aColor));

        if(0xff != nTransparency)
        {
            // nTransparency is in range [0..255]
            rToSet.Put(XFillTransparenceItem((((sal_Int32)nTransparency * 100) + 127) / 255));
        }
    }
    else if(GPOS_NONE != rBrush.GetGraphicPos())
    {
        const Graphic* pGraphic = rBrush.GetGraphic();

        if(pGraphic)
        {
            // set fill style and graphic itself
            rToSet.Put(XFillStyleItem(XFILL_BITMAP));
            rToSet.Put(XFillBitmapItem(String(), *pGraphic));

            // set defaults
            // already pool default rToSet.Put(XFillBmpPosItem(RP_MM));
            // already pool default rToSet.Put(XFillBmpTileOffsetXItem(0));
            // already pool default rToSet.Put(XFillBmpTileOffsetYItem(0));
            // already pool default rToSet.Put(XFillBmpPosOffsetXItem(0));
            // already pool default rToSet.Put(XFillBmpPosOffsetYItem(0));
            // already pool default rToSet.Put(XFillBmpSizeLogItem(true));
            // already pool default rToSet.Put(XFillBmpSizeXItem(0));
            // already pool default rToSet.Put(XFillBmpSizeYItem(0));

            if(GPOS_AREA == rBrush.GetGraphicPos())
            {
                // stretch, also means no tile (both items are defaulted to true)
                // rToSet.Put(XFillBmpStretchItem(true));
                rToSet.Put(XFillBmpTileItem(false));

                // default for strech is also top-left, but this will not be visible
                // rToSet.Put(XFillBmpPosItem(RP_LT));
            }
            else if(GPOS_TILED == rBrush.GetGraphicPos())
            {
                // tiled, also means no stretch (both items are defaulted to true)
                rToSet.Put(XFillBmpStretchItem(false));
                //rToSet.Put(XFillBmpTileItem(true));

                // default for tiled is top-left
                rToSet.Put(XFillBmpPosItem(RP_LT));
            }
            else
            {
                // everything else means no tile and no stretch
                rToSet.Put(XFillBmpStretchItem(false));
                rToSet.Put(XFillBmpTileItem(false));

                switch(rBrush.GetGraphicPos())
                {
                    case GPOS_LT: rToSet.Put(XFillBmpPosItem(RP_LT)); break;
                    case GPOS_MT: rToSet.Put(XFillBmpPosItem(RP_MT)); break;
                    case GPOS_RT: rToSet.Put(XFillBmpPosItem(RP_RT)); break;
                    case GPOS_LM: rToSet.Put(XFillBmpPosItem(RP_LM)); break;
                    case GPOS_MM: rToSet.Put(XFillBmpPosItem(RP_MM)); break;
                    case GPOS_RM: rToSet.Put(XFillBmpPosItem(RP_RM)); break;
                    case GPOS_LB: rToSet.Put(XFillBmpPosItem(RP_LB)); break;
                    case GPOS_MB: rToSet.Put(XFillBmpPosItem(RP_MB)); break;
                    case GPOS_RB: rToSet.Put(XFillBmpPosItem(RP_RB)); break;
                    default: break; // already handled GPOS_AREA, GPOS_TILED and GPOS_NONE
                }
            }

            // check for transparency
            const sal_Int8 nTransparency(rBrush.getGraphicTransparency());

            if(0 != nTransparency)
            {
                // nTransparency is in range [0..100]
                rToSet.Put(XFillTransparenceItem(nTransparency));
            }
        }
        else
        {
            OSL_ENSURE(false, "Could not get Graphic from SvxBrushItem (!)");
        }
    }
}

//UUUU
SvxBrushItem getSvxBrushItemFromSourceSet(const SfxItemSet& rSourceSet)
{
    SvxBrushItem aRetval(RES_BACKGROUND);

    const XFillStyleItem* pXFillStyleItem(static_cast< const XFillStyleItem*  >(rSourceSet.GetItem(XATTR_FILLSTYLE, false)));

    if(!pXFillStyleItem)
    {
        return aRetval;
    }

    switch(pXFillStyleItem->GetValue())
    {
        case XFILL_NONE:
        {
            // done; return default item
            break;
        }
        case XFILL_SOLID:
        {
            // create SvxBrushItem with fill color
            Color aFillColor(static_cast< const XFillColorItem& >(rSourceSet.Get(XATTR_FILLCOLOR)).GetColorValue());
            const sal_uInt16 nFillTransparence(static_cast< const XFillTransparenceItem& >(rSourceSet.Get(XATTR_FILLTRANSPARENCE)).GetValue());

            if(0 != nFillTransparence)
            {
                // nFillTransparence is in range [0..100] and needs to be in [0..255] unsigned
                aFillColor.SetTransparency(static_cast< sal_uInt8 >((nFillTransparence * 255) / 100));
            }

            aRetval = SvxBrushItem(aFillColor, RES_BACKGROUND);
        }
        case XFILL_GRADIENT:
        case XFILL_HATCH:
        {
            // cannot be supported
            break;
        }
        case XFILL_BITMAP:
        {
            // create SvxBrushItem with bitmap info and flags
            const XFillBitmapItem& rBmpItm = static_cast< const XFillBitmapItem& >(rSourceSet.Get(XATTR_FILLBITMAP));
            const Graphic aGraphic(rBmpItm.GetGraphicObject().GetGraphic());

            if(GRAPHIC_NONE != aGraphic.GetType())
            {
                // get graphic position
                SvxGraphicPosition aSvxGraphicPosition(GPOS_NONE);
                const XFillBmpStretchItem& rStretchItem = static_cast< const XFillBmpStretchItem& >(rSourceSet.Get(XATTR_FILLBMP_STRETCH));
                const XFillBmpTileItem& rTileItem = static_cast< const XFillBmpTileItem& >(rSourceSet.Get(XATTR_FILLBMP_TILE));

                if(rTileItem.GetValue())
                {
                    aSvxGraphicPosition = GPOS_TILED;
                }
                else if(rStretchItem.GetValue())
                {
                    aSvxGraphicPosition = GPOS_AREA;
                }
                else
                {
                    const XFillBmpPosItem& rPosItem = static_cast< const XFillBmpPosItem& >(rSourceSet.Get(XATTR_FILLBMP_POS));

                    switch(rPosItem.GetValue())
                    {
                        case RP_LT: aSvxGraphicPosition = GPOS_LT; break;
                        case RP_MT: aSvxGraphicPosition = GPOS_MT; break;
                        case RP_RT: aSvxGraphicPosition = GPOS_RT; break;
                        case RP_LM: aSvxGraphicPosition = GPOS_LM; break;
                        case RP_MM: aSvxGraphicPosition = GPOS_MM; break;
                        case RP_RM: aSvxGraphicPosition = GPOS_RM; break;
                        case RP_LB: aSvxGraphicPosition = GPOS_LB; break;
                        case RP_MB: aSvxGraphicPosition = GPOS_MB; break;
                        case RP_RB: aSvxGraphicPosition = GPOS_RB; break;
                    }
                }

                // create with given graphic and position
                aRetval = SvxBrushItem(aGraphic, aSvxGraphicPosition, RES_BACKGROUND);

                const sal_uInt16 nFillTransparence(static_cast< const XFillTransparenceItem& >(rSourceSet.Get(XATTR_FILLTRANSPARENCE)).GetValue());

                if(0 != nFillTransparence)
                {
                    // nFillTransparence is in range [0..100] and needs to be in [0..100] signed
                    aRetval.setGraphicTransparency(static_cast< sal_Int8 >(nFillTransparence));
                }
            }

            break;
        }
    }

    return aRetval;
}

//eof
