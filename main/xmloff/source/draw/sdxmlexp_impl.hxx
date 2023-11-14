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



#ifndef _SDXMLEXP_IMPL_HXX
#define _SDXMLEXP_IMPL_HXX

#include <xmloff/xmlexp.hxx>


#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <comphelper/stl_types.hxx>

//////////////////////////////////////////////////////////////////////////////

class SvXMLUnitConverter;
class SvXMLExportItemMapper;
class SfxPoolItem;
class SfxItemSet;
class OUStrings_Impl;
class OUStringsSort_Impl;
class Rectangle;

class ImpPresPageDrawStylePropMapper;
class ImpXMLEXPPageMasterList;
class ImpXMLEXPPageMasterInfo;
class ImpXMLDrawPageInfoList;
class ImpXMLAutoLayoutInfoList;
class SvXMLAutoStylePoolP;
class XMLSdPropHdlFactory;
class ImpXMLShapeStyleInfo;
class XMLShapeExportPropertyMapper;
class XMLPageExportPropertyMapper;

//////////////////////////////////////////////////////////////////////////////

enum XmlPlaceholder
{
	XmlPlaceholderTitle,
	XmlPlaceholderOutline,
	XmlPlaceholderSubtitle,
	XmlPlaceholderText,
	XmlPlaceholderGraphic,
	XmlPlaceholderObject,
	XmlPlaceholderChart,
	XmlPlaceholderOrgchart,
	XmlPlaceholderTable,
	XmlPlaceholderPage,
	XmlPlaceholderNotes,
	XmlPlaceholderHandout,
	XmlPlaceholderVerticalTitle,
	XmlPlaceholderVerticalOutline
};

DECLARE_STL_STDKEY_SET( sal_Int32, SdXMLFormatMap );

//////////////////////////////////////////////////////////////////////////////

struct HeaderFooterPageSettingsImpl
{
	rtl::OUString maStrHeaderDeclName;
	rtl::OUString maStrFooterDeclName;
	rtl::OUString maStrDateTimeDeclName;
};

struct DateTimeDeclImpl
{
	rtl::OUString maStrText;
	sal_Bool mbFixed;
	sal_Int32 mnFormat;
};


//////////////////////////////////////////////////////////////////////////////

class SdXMLExport : public SvXMLExport
{
	com::sun::star::uno::Reference< com::sun::star::container::XNameAccess > mxDocStyleFamilies;
	com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess > mxDocMasterPages;
	com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess > mxDocDrawPages;
	sal_Int32					mnDocMasterPageCount;
	sal_Int32					mnDocDrawPageCount;
	sal_uInt32					mnShapeStyleInfoIndex;
	sal_uInt32					mnObjectCount;

	// temporary infos
	ImpXMLEXPPageMasterList*	mpPageMasterInfoList;
	ImpXMLEXPPageMasterList*	mpPageMasterUsageList;
	ImpXMLEXPPageMasterList*	mpNotesPageMasterUsageList;
	ImpXMLEXPPageMasterInfo*	mpHandoutPageMaster;
	ImpXMLAutoLayoutInfoList*	mpAutoLayoutInfoList;

	com::sun::star::uno::Sequence< ::rtl::OUString > maDrawPagesAutoLayoutNames;

	::std::vector< ::rtl::OUString >		maDrawPagesStyleNames;
	::std::vector< ::rtl::OUString >		maDrawNotesPagesStyleNames;
	::std::vector< ::rtl::OUString >		maMasterPagesStyleNames;
	::rtl::OUString							maHandoutMasterStyleName;
	::std::vector< HeaderFooterPageSettingsImpl >	maDrawPagesHeaderFooterSettings;
	::std::vector< HeaderFooterPageSettingsImpl >	maDrawNotesPagesHeaderFooterSettings;

	::std::vector< ::rtl::OUString >		maHeaderDeclsVector;
	::std::vector< ::rtl::OUString >		maFooterDeclsVector;
	::std::vector< DateTimeDeclImpl >		maDateTimeDeclsVector;

	HeaderFooterPageSettingsImpl			maHandoutPageHeaderFooterSettings;

	XMLSdPropHdlFactory*				mpSdPropHdlFactory;
	XMLShapeExportPropertyMapper*		mpPropertySetMapper;
	XMLPageExportPropertyMapper*		mpPresPagePropsMapper;

	SdXMLFormatMap	maUsedDateStyles;			// this is a vector with the used formattings for date fields
	SdXMLFormatMap	maUsedTimeStyles;			// this is a vector with the used formattings for time fields

	sal_Bool					mbIsDraw;
	sal_Bool					mbFamilyGraphicUsed;
	sal_Bool					mbFamilyPresentationUsed;

	const rtl::OUString			msZIndex;
	const rtl::OUString			msEmptyPres;
	const rtl::OUString			msModel;
	const rtl::OUString			msStartShape;
	const rtl::OUString			msEndShape;
	const rtl::OUString			msPageLayoutNames;

	virtual void _ExportStyles(sal_Bool bUsed);
	virtual void _ExportAutoStyles();
	virtual void _ExportMasterStyles();
	virtual void _ExportContent();
	// #82003#
	virtual void _ExportMeta();

	ImpXMLEXPPageMasterInfo* ImpGetOrCreatePageMasterInfo( com::sun::star::uno::Reference< com::sun::star::drawing::XDrawPage > xMasterPage );
	void ImpPrepPageMasterInfos();
	void ImpPrepDrawMasterInfos();
	void ImpWritePageMasterInfos();
	void ImpPrepAutoLayoutInfos();
	HeaderFooterPageSettingsImpl ImpPrepDrawPageHeaderFooterDecls( const com::sun::star::uno::Reference< com::sun::star::drawing::XDrawPage >& xDrawPage );
	ImpXMLEXPPageMasterInfo* ImpGetPageMasterInfoByName(const rtl::OUString& rName);

	void ImpPrepDrawPageInfos();
	void ImpPrepMasterPageInfos();
	void ImpWritePresentationStyles();
	::rtl::OUString ImpCreatePresPageStyleName( com::sun::star::uno::Reference<com::sun::star::drawing::XDrawPage> xDrawPage, bool bExportBackground = true );

	sal_Bool ImpPrepAutoLayoutInfo(const com::sun::star::uno::Reference< com::sun::star::drawing::XDrawPage >& xPage, rtl::OUString& rName);
	void ImpWriteAutoLayoutInfos();
	void ImpWriteAutoLayoutPlaceholder(XmlPlaceholder ePl, const Rectangle& rRect);
	void ImpWriteHeaderFooterDecls();
	void ImplExportHeaderFooterDeclAttributes( const HeaderFooterPageSettingsImpl& aSettings );

	void exportFormsElement( com::sun::star::uno::Reference< com::sun::star::drawing::XDrawPage > xDrawPage );
	void exportPresentationSettings();

	// #82003# helper function for recursive object count
	sal_uInt32 ImpRecursiveObjectCount( com::sun::star::uno::Reference< com::sun::star::drawing::XShapes > xShapes);

	rtl::OUString getNavigationOrder( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& xDrawPage );

	void collectAnnotationAutoStyles( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& xDrawPage );
	void exportAnnotations( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& xDrawPage );

protected:
	virtual void GetViewSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps);
	virtual void GetConfigurationSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps);

public:
	// #110680#
	SdXMLExport( 
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
		sal_Bool bIsDraw, sal_uInt16 nExportFlags = EXPORT_ALL );
	virtual ~SdXMLExport();

	void SetProgress(sal_Int32 nProg);

	// XExporter
	virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

	// get factories and mappers
	XMLSdPropHdlFactory* GetSdPropHdlFactory() const { return mpSdPropHdlFactory; }
	XMLShapeExportPropertyMapper* GetPropertySetMapper() const { return mpPropertySetMapper; }
	XMLPageExportPropertyMapper* GetPresPagePropsMapper() const { return mpPresPagePropsMapper; }

	sal_Bool IsDraw() const { return mbIsDraw; }
	sal_Bool IsImpress() const { return !mbIsDraw; }

	sal_Bool IsFamilyGraphicUsed() const { return mbFamilyGraphicUsed; }
	void SetFamilyGraphicUsed() { mbFamilyGraphicUsed = sal_True; }
	sal_Bool IsFamilyPresentationUsed() const { return mbFamilyPresentationUsed; }
	void SetFamilyPresentationUsed() { mbFamilyPresentationUsed = sal_True; }

	virtual void addDataStyle(const sal_Int32 nNumberFormat, sal_Bool bTimeFormat = sal_False );
	virtual void exportDataStyles();
	virtual void exportAutoDataStyles();
	virtual rtl::OUString getDataStyleName(const sal_Int32 nNumberFormat, sal_Bool bTimeFormat = sal_False ) const;

    // XServiceInfo ( : SvXMLExport )
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
};

#endif	//  _SDXMLEXP_HXX

