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


#ifndef INCLUDED_REPORTHELPERDEFINES_HXX
#define INCLUDED_REPORTHELPERDEFINES_HXX

#define REPORTCONTROLMODEL_HEADER()  \
    virtual ::rtl::OUString SAL_CALL getDataField(); \
    virtual void SAL_CALL setDataField(const ::rtl::OUString & the_value); \
    virtual ::sal_Bool SAL_CALL getPrintWhenGroupChange(); \
    virtual void SAL_CALL setPrintWhenGroupChange(::sal_Bool the_value); \
    virtual ::rtl::OUString SAL_CALL getConditionalPrintExpression(); \
    virtual void SAL_CALL setConditionalPrintExpression(const ::rtl::OUString & the_value); \
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XFormatCondition > SAL_CALL createFormatCondition();

#define SHAPE_HEADER()  \
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition(); \
    virtual void SAL_CALL setPosition(const ::com::sun::star::awt::Point & aPosition); \
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(); \
    virtual void SAL_CALL setSize(const ::com::sun::star::awt::Size & aSize);

#define REPORTCOMPONENT_HEADER()  \
    virtual ::rtl::OUString SAL_CALL getName(); \
    virtual void SAL_CALL setName(const ::rtl::OUString & the_value); \
    virtual ::sal_Int32 SAL_CALL getHeight(); \
    virtual void SAL_CALL setHeight(::sal_Int32 the_value); \
    virtual ::sal_Int32 SAL_CALL getPositionX(); \
    virtual void SAL_CALL setPositionX(::sal_Int32 the_value); \
    virtual ::sal_Int32 SAL_CALL getPositionY(); \
    virtual void SAL_CALL setPositionY(::sal_Int32 the_value); \
    virtual ::sal_Int32 SAL_CALL getWidth(); \
    virtual void SAL_CALL setWidth(::sal_Int32 the_value); \
    virtual ::sal_Int16 SAL_CALL getControlBorder(); \
    virtual void SAL_CALL setControlBorder(::sal_Int16 the_value); \
    virtual ::sal_Int32 SAL_CALL getControlBorderColor(); \
    virtual void SAL_CALL setControlBorderColor(::sal_Int32 the_value); \
    virtual ::sal_Bool SAL_CALL getPrintRepeatedValues(); \
    virtual void SAL_CALL setPrintRepeatedValues(::sal_Bool the_value); \
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getMasterFields(); \
    virtual void SAL_CALL setMasterFields( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _masterfields ); \
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getDetailFields(); \
    virtual void SAL_CALL setDetailFields( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _detailfields ); \
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getSection();

#define REPORTCONTROLFORMAT_HEADER()  \
    virtual ::sal_Int32 SAL_CALL getControlBackground();\
    virtual void SAL_CALL setControlBackground( ::sal_Int32 _controlbackground );\
    virtual ::sal_Bool SAL_CALL getControlBackgroundTransparent();\
    virtual void SAL_CALL setControlBackgroundTransparent( ::sal_Bool _controlbackgroundtransparent );\
    virtual ::sal_Int16 SAL_CALL getParaAdjust();\
    virtual void SAL_CALL setParaAdjust( ::sal_Int16 _paraadjust );\
    virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFontDescriptor();\
    virtual void SAL_CALL setFontDescriptor( const ::com::sun::star::awt::FontDescriptor& _fontdescriptor );\
    virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFontDescriptorAsian();\
    virtual void SAL_CALL setFontDescriptorAsian( const ::com::sun::star::awt::FontDescriptor& _fontdescriptor );\
    virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFontDescriptorComplex();\
    virtual void SAL_CALL setFontDescriptorComplex( const ::com::sun::star::awt::FontDescriptor& _fontdescriptor );\
    virtual ::sal_Int16 SAL_CALL getControlTextEmphasis();\
    virtual void SAL_CALL setControlTextEmphasis( ::sal_Int16 _controltextemphasis );\
    virtual ::sal_Int16 SAL_CALL getCharEmphasis();\
    virtual void SAL_CALL setCharEmphasis( ::sal_Int16 _charemphasis );\
    virtual ::sal_Bool SAL_CALL getCharCombineIsOn();\
    virtual void SAL_CALL setCharCombineIsOn( ::sal_Bool _charcombineison );\
    virtual ::rtl::OUString SAL_CALL getCharCombinePrefix();\
    virtual void SAL_CALL setCharCombinePrefix( const ::rtl::OUString& _charcombineprefix );\
    virtual ::rtl::OUString SAL_CALL getCharCombineSuffix();\
    virtual void SAL_CALL setCharCombineSuffix( const ::rtl::OUString& _charcombinesuffix );\
    virtual ::sal_Bool SAL_CALL getCharHidden();\
    virtual void SAL_CALL setCharHidden( ::sal_Bool _charhidden );\
    virtual ::sal_Bool SAL_CALL getCharShadowed();\
    virtual void SAL_CALL setCharShadowed( ::sal_Bool _charshadowed );\
    virtual ::sal_Bool SAL_CALL getCharContoured();\
    virtual void SAL_CALL setCharContoured( ::sal_Bool _charcontoured );\
    virtual ::sal_Int16 SAL_CALL getCharCaseMap();\
    virtual void SAL_CALL setCharCaseMap( ::sal_Int16 _charcasemap );\
    virtual ::com::sun::star::lang::Locale SAL_CALL getCharLocale();\
    virtual void SAL_CALL setCharLocale( const ::com::sun::star::lang::Locale& _charlocale );\
    virtual ::sal_Int16 SAL_CALL getCharEscapement();\
    virtual void SAL_CALL setCharEscapement( ::sal_Int16 _charescapement );\
    virtual ::sal_Int8 SAL_CALL getCharEscapementHeight();\
    virtual void SAL_CALL setCharEscapementHeight( ::sal_Int8 _charescapementheight );\
    virtual ::sal_Bool SAL_CALL getCharAutoKerning();\
    virtual void SAL_CALL setCharAutoKerning( ::sal_Bool _charautokerning );\
    virtual ::sal_Int16 SAL_CALL getCharKerning();\
    virtual void SAL_CALL setCharKerning( ::sal_Int16 _charkerning );\
    virtual ::sal_Bool SAL_CALL getCharFlash();\
    virtual void SAL_CALL setCharFlash( ::sal_Bool _charflash );\
    virtual ::sal_Int16 SAL_CALL getCharRelief();\
    virtual void SAL_CALL setCharRelief( ::sal_Int16 _charrelief );\
    virtual ::rtl::OUString SAL_CALL getCharFontName();\
    virtual void SAL_CALL setCharFontName( const ::rtl::OUString& _charfontname );\
    virtual ::rtl::OUString SAL_CALL getCharFontStyleName();\
    virtual void SAL_CALL setCharFontStyleName( const ::rtl::OUString& _charfontstylename );\
    virtual ::sal_Int16 SAL_CALL getCharFontFamily();\
    virtual void SAL_CALL setCharFontFamily( ::sal_Int16 _charfontfamily );\
    virtual ::sal_Int16 SAL_CALL getCharFontCharSet();\
    virtual void SAL_CALL setCharFontCharSet( ::sal_Int16 _charfontcharset );\
    virtual ::sal_Int16 SAL_CALL getCharFontPitch();\
    virtual void SAL_CALL setCharFontPitch( ::sal_Int16 _charfontpitch );\
    virtual ::sal_Int32 SAL_CALL getCharColor();\
    virtual void SAL_CALL setCharColor( ::sal_Int32 _charcolor );\
    virtual ::sal_Int32 SAL_CALL getCharUnderlineColor();\
    virtual void SAL_CALL setCharUnderlineColor( ::sal_Int32 _charunderlinecolor );\
    virtual float SAL_CALL getCharHeight();\
    virtual void SAL_CALL setCharHeight( float _charheight );\
    virtual ::sal_Int16 SAL_CALL getCharUnderline();\
    virtual void SAL_CALL setCharUnderline( ::sal_Int16 _charunderline );\
    virtual float SAL_CALL getCharWeight();\
    virtual void SAL_CALL setCharWeight( float _charweight );\
    virtual ::com::sun::star::awt::FontSlant SAL_CALL getCharPosture();\
    virtual void SAL_CALL setCharPosture( ::com::sun::star::awt::FontSlant _charposture );\
    virtual ::sal_Int16 SAL_CALL getCharStrikeout();\
    virtual void SAL_CALL setCharStrikeout( ::sal_Int16 _charstrikeout );\
    virtual ::sal_Bool SAL_CALL getCharWordMode();\
    virtual void SAL_CALL setCharWordMode( ::sal_Bool _charwordmode );\
    virtual ::sal_Int16 SAL_CALL getCharRotation();\
    virtual void SAL_CALL setCharRotation( ::sal_Int16 _charrotation );\
    virtual ::sal_Int16 SAL_CALL getCharScaleWidth();\
    virtual void SAL_CALL setCharScaleWidth( ::sal_Int16 _charscalewidth );\
    virtual ::com::sun::star::style::VerticalAlignment SAL_CALL getVerticalAlign();\
    virtual void SAL_CALL setVerticalAlign( ::com::sun::star::style::VerticalAlignment _paravertalignment );\
    virtual ::rtl::OUString SAL_CALL getHyperLinkURL();\
    virtual void SAL_CALL setHyperLinkURL( const ::rtl::OUString& _hyperlinkurl );\
    virtual ::rtl::OUString SAL_CALL getHyperLinkTarget();\
    virtual void SAL_CALL setHyperLinkTarget( const ::rtl::OUString& _hyperlinktarget );\
    virtual ::rtl::OUString SAL_CALL getHyperLinkName();\
    virtual void SAL_CALL setHyperLinkName( const ::rtl::OUString& _hyperlinkname );\
    virtual ::rtl::OUString SAL_CALL getVisitedCharStyleName();\
    virtual void SAL_CALL setVisitedCharStyleName( const ::rtl::OUString& _visitedcharstylename );\
    virtual ::rtl::OUString SAL_CALL getUnvisitedCharStyleName();\
    virtual void SAL_CALL setUnvisitedCharStyleName( const ::rtl::OUString& _unvisitedcharstylename );\
    virtual float SAL_CALL getCharHeightAsian();\
    virtual void SAL_CALL setCharHeightAsian( float _charheightasian );\
    virtual float SAL_CALL getCharWeightAsian();\
    virtual void SAL_CALL setCharWeightAsian( float _charweightasian );\
    virtual ::rtl::OUString SAL_CALL getCharFontNameAsian();\
    virtual void SAL_CALL setCharFontNameAsian( const ::rtl::OUString& _charfontnameasian );\
    virtual ::rtl::OUString SAL_CALL getCharFontStyleNameAsian();\
    virtual void SAL_CALL setCharFontStyleNameAsian( const ::rtl::OUString& _charfontstylenameasian );\
    virtual ::sal_Int16 SAL_CALL getCharFontFamilyAsian();\
    virtual void SAL_CALL setCharFontFamilyAsian( ::sal_Int16 _charfontfamilyasian );\
    virtual ::sal_Int16 SAL_CALL getCharFontCharSetAsian();\
    virtual void SAL_CALL setCharFontCharSetAsian( ::sal_Int16 _charfontcharsetasian );\
    virtual ::sal_Int16 SAL_CALL getCharFontPitchAsian();\
    virtual void SAL_CALL setCharFontPitchAsian( ::sal_Int16 _charfontpitchasian );\
    virtual ::com::sun::star::awt::FontSlant SAL_CALL getCharPostureAsian();\
    virtual void SAL_CALL setCharPostureAsian( ::com::sun::star::awt::FontSlant _charpostureasian );\
    virtual ::com::sun::star::lang::Locale SAL_CALL getCharLocaleAsian();\
    virtual void SAL_CALL setCharLocaleAsian( const ::com::sun::star::lang::Locale& _charlocaleasian );\
    virtual float SAL_CALL getCharHeightComplex();\
    virtual void SAL_CALL setCharHeightComplex( float _charheightcomplex );\
    virtual float SAL_CALL getCharWeightComplex();\
    virtual void SAL_CALL setCharWeightComplex( float _charweightcomplex );\
    virtual ::rtl::OUString SAL_CALL getCharFontNameComplex();\
    virtual void SAL_CALL setCharFontNameComplex( const ::rtl::OUString& _charfontnamecomplex );\
    virtual ::rtl::OUString SAL_CALL getCharFontStyleNameComplex();\
    virtual void SAL_CALL setCharFontStyleNameComplex( const ::rtl::OUString& _charfontstylenamecomplex );\
    virtual ::sal_Int16 SAL_CALL getCharFontFamilyComplex();\
    virtual void SAL_CALL setCharFontFamilyComplex( ::sal_Int16 _charfontfamilycomplex );\
    virtual ::sal_Int16 SAL_CALL getCharFontCharSetComplex();\
    virtual void SAL_CALL setCharFontCharSetComplex( ::sal_Int16 _charfontcharsetcomplex );\
    virtual ::sal_Int16 SAL_CALL getCharFontPitchComplex();\
    virtual void SAL_CALL setCharFontPitchComplex( ::sal_Int16 _charfontpitchcomplex );\
    virtual ::com::sun::star::awt::FontSlant SAL_CALL getCharPostureComplex();\
    virtual void SAL_CALL setCharPostureComplex( ::com::sun::star::awt::FontSlant _charposturecomplex );\
    virtual ::com::sun::star::lang::Locale SAL_CALL getCharLocaleComplex();\
    virtual void SAL_CALL setCharLocaleComplex( const ::com::sun::star::lang::Locale& _charlocalecomplex );



#endif // INCLUDED_REPORTHELPERDEFINES_HXX
