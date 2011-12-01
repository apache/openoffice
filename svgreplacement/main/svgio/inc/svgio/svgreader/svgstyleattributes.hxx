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

#ifndef INCLUDED_SVGIO_SVGREADER_SVGSTYLEATTRIBUTES_HXX
#define INCLUDED_SVGIO_SVGREADER_SVGSTYLEATTRIBUTES_HXX

#include <svgio/svgiodllapi.h>
#include <svgio/svgreader/svgpaint.hxx>
#include <svgio/svgreader/svgnode.hxx>
#include <vcl/vclenum.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace svgio { namespace svgreader { class SvgGradientNode; }}

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        enum StrokeLinecap
        {
            StrokeLinecap_notset,
            StrokeLinecap_butt,
            StrokeLinecap_round,
            StrokeLinecap_square
        };

        enum StrokeLinejoin
        {
            StrokeLinejoin_notset,
            StrokeLinejoin_miter,
            StrokeLinejoin_round,
            StrokeLinejoin_bevel
        };

        enum FontStretch
        {
            FontStretch_notset,
            FontStretch_normal,
            FontStretch_wider,
            FontStretch_narrower,
            FontStretch_ultra_condensed,
            FontStretch_extra_condensed,
            FontStretch_condensed,
            FontStretch_semi_condensed,
            FontStretch_semi_expanded,
            FontStretch_expanded,
            FontStretch_extra_expanded,
            FontStretch_ultra_expanded
        };

        FontStretch getWider(FontStretch aSource);
        FontStretch getNarrower(FontStretch aSource);

        enum FontStyle
        {
            FontStyle_notset,
            FontStyle_normal,
            FontStyle_italic,
            FontStyle_oblique
        };

        enum FontVariant
        {
            FontVariant_notset,
            FontVariant_normal,
            FontVariant_small_caps
        };

        enum FontWeight
        {
            FontWeight_notset,
            FontWeight_100,
            FontWeight_200,
            FontWeight_300,
            FontWeight_400, // same as FontWeight_normal
            FontWeight_500,
            FontWeight_600,
            FontWeight_700, // same as FontWeight_bold
            FontWeight_800,
            FontWeight_900,
            FontWeight_bolder,
            FontWeight_lighter,
        };

        FontWeight getBolder(FontWeight aSource);
        FontWeight getLighter(FontWeight aSource);
        ::FontWeight getVclFontWeight(FontWeight aSource);

        enum TextAlign
        {
            TextAlign_notset,
            TextAlign_left,
            TextAlign_right,
            TextAlign_center,
            TextAlign_justify
        };

        enum TextDecoration
        {
            TextDecoration_notset,
            TextDecoration_none,
            TextDecoration_underline,
            TextDecoration_overline,
            TextDecoration_line_through,
            TextDecoration_blink
        };

        enum TextAnchor
        {
            TextAnchor_notset,
            TextAnchor_start,
            TextAnchor_middle,
            TextAnchor_end
        };

        class SvgStyleAttributes
        {
        private:
            SvgNode&                    mrOwner;
            const SvgStyleAttributes*   mpCssStyleParent;

            SvgPaint                    maFill;
            SvgPaint                    maStroke;
            SvgPaint                    maStopColor;
            SvgNumber                   maStrokeWidth;
            SvgNumber                   maStopOpacity;
            const SvgGradientNode*      mpSvgGradientNodeFill;
            const SvgGradientNode*      mpSvgGradientNodeStroke;
            SvgNumber                   maFillOpacity;
            SvgNumberVector             maStrokeDasharray;
            SvgNumber                   maStrokeDashOffset;
            StrokeLinecap               maStrokeLinecap;
            StrokeLinejoin              maStrokeLinejoin;
            SvgNumber                   maStrokeMiterLimit;
            SvgNumber                   maStrokeOpacity;
            SvgStringVector             maFontFamily;
            SvgNumber                   maFontSize;
            FontStretch                 maFontStretch;
            FontStyle                   maFontStyle;
            FontVariant                 maFontVariant;
            FontWeight                  maFontWeight;
            TextAlign                   maTextAlign;
            TextDecoration              maTextDecoration;
            TextAnchor                  maTextAnchor;

            /// bitfield
            bool                        maFillRule : 1; // true: NonZero, false: EvenOdd
            bool                        maFillRuleSet : 1;

            /// internal helpers
            void add_fillGradient(
                const basegfx::B2DPolyPolygon& rPath, 
                drawinglayer::primitive2d::Primitive2DVector& rTarget, 
                const SvgGradientNode& rFillGradient,
                const basegfx::B2DRange& rGeoRange) const;
            void add_fill(
                const basegfx::B2DPolyPolygon& rPath, 
                drawinglayer::primitive2d::Primitive2DVector& rTarget,
                const basegfx::B2DRange& rGeoRange) const;
            void add_stroke(
                const basegfx::B2DPolyPolygon& rPath, 
                drawinglayer::primitive2d::Primitive2DVector& rTarget,
                const basegfx::B2DRange& rGeoRange) const;

        public:
            /// local attribute scanner
            void parseStyleAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent);

            /// helper which does the necessary with a given path
            void add_text(
                drawinglayer::primitive2d::Primitive2DVector& rTarget, 
                drawinglayer::primitive2d::Primitive2DVector& rSource) const;
            void add_path(
                const basegfx::B2DPolyPolygon& rPath, 
                drawinglayer::primitive2d::Primitive2DVector& rTarget, 
                const basegfx::B2DHomMatrix* pTransform) const;

            /// helper to evtl. link to css style
            void checkForCssStyle(const rtl::OUString& rClassStr) const;

            /// scan helpers
            void readStyle(const rtl::OUString& rCandidate);
            const SvgStyleAttributes* getParentStyle() const;

            SvgStyleAttributes(SvgNode& rOwner);
            ~SvgStyleAttributes();

            /// fill content
            const basegfx::BColor* getFill() const;
            void setFill(const SvgPaint& rFill) { maFill = rFill; }

            /// stroke content
            const basegfx::BColor* getStroke() const;
            void setStroke(const SvgPaint& rStroke) { maStroke = rStroke; }

            /// stop color content
            const basegfx::BColor& getStopColor() const { return maStopColor.getBColor(); }
            void setStopColor(const SvgPaint& rStopColor) { maStopColor = rStopColor; }

            /// stroke-width content
            const SvgNumber getStrokeWidth() const;
            void setStrokeWidth(const SvgNumber& rStrokeWidth = SvgNumber()) { maStrokeWidth = rStrokeWidth; }

            /// stop opacity content
            const SvgNumber getStopOpacity() const;
            void setStopOpacity(const SvgNumber& rStopOpacity = SvgNumber()) { maStopOpacity = rStopOpacity; }

            /// access to evtl. set fill gradient
            const SvgGradientNode* getSvgGradientNodeFill() const;
            void setSvgGradientNodeFill(const SvgGradientNode* pNew) { mpSvgGradientNodeFill = pNew; }

            /// access to evtl. set stroke gradient
            const SvgGradientNode* getSvgGradientNodeStroke() const;
            void setSvgGradientNodeStroke(const SvgGradientNode* pNew) { mpSvgGradientNodeStroke = pNew; }

            /// fill opacity content
            const SvgNumber getFillOpacity() const;
            void setFillOpacity(const SvgNumber& rFillOpacity = SvgNumber()) { maFillOpacity = rFillOpacity; }

            /// fill rule content
            bool getFillRule() const;
            void setFillRule(const bool* pFillRule = 0);

            /// fill StrokeDasharray content
            const SvgNumberVector& getStrokeDasharray() const;
            void setStrokeDasharray(const SvgNumberVector& rStrokeDasharray = SvgNumberVector()) { maStrokeDasharray = rStrokeDasharray; }

            /// StrokeDashOffset content
            const SvgNumber getStrokeDashOffset() const;
            void setStrokeDashOffset(const SvgNumber& rStrokeDashOffset = SvgNumber()) { maStrokeDashOffset = rStrokeDashOffset; }

            /// StrokeLinecap content
            const StrokeLinecap getStrokeLinecap() const;
            void setStrokeLinecap(const StrokeLinecap aStrokeLinecap = StrokeLinecap_notset) { maStrokeLinecap = aStrokeLinecap; }

            /// StrokeLinejoin content
            const StrokeLinejoin getStrokeLinejoin() const;
            void setStrokeLinejoin(const StrokeLinejoin aStrokeLinejoin = StrokeLinejoin_notset) { maStrokeLinejoin = aStrokeLinejoin; }

            /// StrokeMiterLimit content
            const SvgNumber getStrokeMiterLimit() const;
            void setStrokeMiterLimit(const SvgNumber& rStrokeMiterLimit = SvgNumber()) { maStrokeMiterLimit = rStrokeMiterLimit; }

            /// StrokeOpacity content
            const SvgNumber getStrokeOpacity() const;
            void setStrokeOpacity(const SvgNumber& rStrokeOpacity = SvgNumber()) { maStrokeOpacity = rStrokeOpacity; }

            /// Font content
            const SvgStringVector& getFontFamily() const;
            void setFontFamily(const SvgStringVector& rSvgStringVector = SvgStringVector()) { maFontFamily = rSvgStringVector; }

            /// FontSize content
            const SvgNumber getFontSize() const;
            void setFontSize(const SvgNumber& rFontSize = SvgNumber()) { maFontSize = rFontSize; }

            /// FontStretch content
            const FontStretch getFontStretch() const;
            void setFontStretch(const FontStretch aFontStretch = FontStretch_notset) { maFontStretch = aFontStretch; }

            /// FontStyle content
            const FontStyle getFontStyle() const;
            void setFontStyle(const FontStyle aFontStyle = FontStyle_notset) { maFontStyle = aFontStyle; }

            /// FontVariant content
            const FontVariant getFontVariant() const;
            void setFontVariant(const FontVariant aFontVariant = FontVariant_notset) { maFontVariant = aFontVariant; }

            /// FontWeight content
            const FontWeight getFontWeight() const;
            void setFontWeight(const FontWeight aFontWeight = FontWeight_notset) { maFontWeight = aFontWeight; }

            /// TextAlign content
            const TextAlign getTextAlign() const;
            void setTextAlign(const TextAlign aTextAlign = TextAlign_notset) { maTextAlign = aTextAlign; }

            /// TextDecoration content
            const SvgStyleAttributes* getTextDecorationDefiningSvgStyleAttributes() const;
            const TextDecoration getTextDecoration() const;
            void setTextDecoration(const TextDecoration aTextDecoration = TextDecoration_notset) { maTextDecoration = aTextDecoration; }

            /// TextAnchor content
            const TextAnchor getTextAnchor() const;
            void setTextAnchor(const TextAnchor aTextAnchor = TextAnchor_notset) { maTextAnchor = aTextAnchor; }

        };
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_SVGIO_SVGREADER_SVGSTYLEATTRIBUTES_HXX

// eof
