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



#ifndef _SVX_CELL_HXX_
#define _SVX_CELL_HXX_

#include <com/sun/star/table/XMergeableCell.hpp>
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#include <com/sun/star/beans/XMultiPropertyStates.hpp>
#include <com/sun/star/lang/XEventListener.hpp>

#include <rtl/ref.hxx>
#include <svl/style.hxx>
#include "svx/sdtaitm.hxx"
#include "tablemodel.hxx"
#include "editeng/unotext.hxx"
#include "svx/svdtext.hxx"

// -----------------------------------------------------------------------------

class SfxItemSet;
class OutlinerParaObject;
class SdrObject;

namespace sdr { namespace properties {
	class TextProperties;
} }

// -----------------------------------------------------------------------------

namespace sdr { namespace table {

// -----------------------------------------------------------------------------

class SVX_DLLPUBLIC Cell :	public SdrText,
				public SvxUnoTextBase,
				public ::com::sun::star::table::XMergeableCell,
				public ::com::sun::star::awt::XLayoutConstrains,
                public ::com::sun::star::lang::XEventListener,
				public ::cppu::OWeakObject
{
    friend class CellUndo;

public:
    SVX_DLLPRIVATE static rtl::Reference< Cell > create( SdrTableObj& rTableObj, OutlinerParaObject* pOutlinerParaObject );

    // private
    SVX_DLLPRIVATE void dispose();

    // SdrTextShape proxy
    bool IsTextEditActive();
    SVX_DLLPRIVATE bool hasText() const;

    SVX_DLLPRIVATE void cloneFrom( const CellRef& rCell );

    SVX_DLLPRIVATE void setCellRect( ::Rectangle& rCellRect ) { maCellRect = rCellRect; }
    SVX_DLLPRIVATE const ::Rectangle& getCellRect() const { return maCellRect; }
    SVX_DLLPRIVATE ::Rectangle& getCellRect() { return maCellRect; }

    OutlinerParaObject* GetEditOutlinerParaObject() const;
    SVX_DLLPRIVATE void SetStyleSheet( SfxStyleSheet* pStyleSheet, sal_Bool bDontRemoveHardAttr );
    SVX_DLLPRIVATE virtual SfxStyleSheet* GetStyleSheet() const;
    SfxStyleSheetPool* GetStyleSheetPool() const;
    SVX_DLLPRIVATE virtual const Rectangle& GetCurrentBoundRect() const;
    SVX_DLLPRIVATE virtual void TakeTextAnchorRect(Rectangle& rAnchorRect) const;

    SVX_DLLPRIVATE virtual const SfxItemSet& GetItemSet() const;
    SVX_DLLPRIVATE void SetMergedItemSetAndBroadcast(const SfxItemSet& rSet, sal_Bool bClearAllItems);
    void SetMergedItem(const SfxPoolItem& rItem);

    SVX_DLLPRIVATE sal_Int32 getMinimumWidth();
    SVX_DLLPRIVATE sal_Int32 getMinimumHeight();

    SVX_DLLPRIVATE long GetTextLeftDistance() const;
    SVX_DLLPRIVATE long GetTextRightDistance() const;
    SVX_DLLPRIVATE long GetTextUpperDistance() const;
    SVX_DLLPRIVATE long GetTextLowerDistance() const;

    SVX_DLLPRIVATE SdrTextVertAdjust GetTextVerticalAdjust() const;
    SdrTextHorzAdjust GetTextHorizontalAdjust() const;

    SVX_DLLPRIVATE virtual void SetModel(SdrModel* pNewModel);

    SVX_DLLPRIVATE void merge( sal_Int32 nColumnSpan, sal_Int32 nRowSpan );
    SVX_DLLPRIVATE void mergeContent( const CellRef& xSourceCell );
    SVX_DLLPRIVATE void replaceContentAndFormatting( const CellRef& xSourceCell );

    SVX_DLLPRIVATE void setMerged();

	// XInterface
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& Type );
    SVX_DLLPRIVATE virtual void SAL_CALL acquire() throw ();
    SVX_DLLPRIVATE virtual void SAL_CALL release() throw ();

    // XTypeProvider
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getImplementationId(  );

    // XServiceInfo
    SVX_DLLPRIVATE virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    SVX_DLLPRIVATE virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XLayoutConstrains
    SVX_DLLPRIVATE virtual ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& aNewSize );

    // XMergeableCell
    SVX_DLLPRIVATE virtual ::sal_Int32 SAL_CALL getRowSpan();
    SVX_DLLPRIVATE virtual ::sal_Int32 SAL_CALL getColumnSpan();
    SVX_DLLPRIVATE virtual ::sal_Bool SAL_CALL isMerged();

	// XCell
    SVX_DLLPRIVATE virtual ::rtl::OUString SAL_CALL getFormula();
    SVX_DLLPRIVATE virtual void SAL_CALL setFormula( const ::rtl::OUString& aFormula );
    SVX_DLLPRIVATE virtual double SAL_CALL getValue();
    SVX_DLLPRIVATE virtual void SAL_CALL setValue( double nValue );
    SVX_DLLPRIVATE virtual ::com::sun::star::table::CellContentType SAL_CALL getType();
    SVX_DLLPRIVATE virtual sal_Int32 SAL_CALL getError();

	// ::com::sun::star::beans::XPropertySet
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    SVX_DLLPRIVATE virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    SVX_DLLPRIVATE virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    SVX_DLLPRIVATE virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    SVX_DLLPRIVATE virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    SVX_DLLPRIVATE virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	// XMultiPropertySet
    SVX_DLLPRIVATE virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    SVX_DLLPRIVATE virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    SVX_DLLPRIVATE virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    SVX_DLLPRIVATE virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

	// ::com::sun::star::beans::XPropertyState
    SVX_DLLPRIVATE virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    SVX_DLLPRIVATE virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

	// XMultiPropertyStates
    SVX_DLLPRIVATE virtual void SAL_CALL setAllPropertiesToDefault(  );
    SVX_DLLPRIVATE virtual void SAL_CALL setPropertiesToDefault( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyDefaults( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );

	// XFastPropertySet
    SVX_DLLPRIVATE virtual void SAL_CALL setFastPropertyValue( ::sal_Int32 nHandle, const ::com::sun::star::uno::Any& aValue );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Any SAL_CALL getFastPropertyValue( ::sal_Int32 nHandle );

	// XText
    SVX_DLLPRIVATE virtual void SAL_CALL insertTextContent( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xRange, const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >& xContent, ::sal_Bool bAbsorb );
    SVX_DLLPRIVATE virtual void SAL_CALL removeTextContent( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >& xContent );

    // XSimpleText
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL createTextCursor(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL createTextCursorByRange( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& aTextPosition );
    SVX_DLLPRIVATE virtual void SAL_CALL insertString( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xRange, const ::rtl::OUString& aString, ::sal_Bool bAbsorb );
    SVX_DLLPRIVATE virtual void SAL_CALL insertControlCharacter( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xRange, ::sal_Int16 nControlCharacter, ::sal_Bool bAbsorb );

    // XTextRange
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > SAL_CALL getText(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getStart(  );
    SVX_DLLPRIVATE virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getEnd(  );
    SVX_DLLPRIVATE virtual ::rtl::OUString SAL_CALL getString(  );
    SVX_DLLPRIVATE virtual void SAL_CALL setString( const ::rtl::OUString& aString );

    // XEventListener
    SVX_DLLPRIVATE virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

	SVX_DLLPRIVATE virtual void SetOutlinerParaObject( OutlinerParaObject* pTextObject );

    SVX_DLLPRIVATE void	AddUndo();

    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

    SVX_DLLPRIVATE sdr::properties::TextProperties* CloneProperties( SdrObject& rNewObj, Cell& rNewCell );

    SVX_DLLPRIVATE static sdr::properties::TextProperties* CloneProperties( sdr::properties::TextProperties* pProperties, SdrObject& rNewObj, Cell& rNewCell );

	SVX_DLLPRIVATE void notifyModified();

    ::rtl::OUString getName();

protected:
	SVX_DLLPRIVATE virtual const SfxItemSet& GetObjectItemSet();
	SVX_DLLPRIVATE virtual void SetObjectItem(const SfxPoolItem& rItem);

    SVX_DLLPRIVATE ::com::sun::star::uno::Any GetAnyForItem( SfxItemSet& aSet, const SfxItemPropertySimpleEntry* pMap );

private:
	SVX_DLLPRIVATE Cell( SdrTableObj& rTableObj, OutlinerParaObject* pOutlinerParaObject ) throw();
	SVX_DLLPRIVATE virtual ~Cell() throw();

	const SvxItemPropertySet* mpPropSet;

	sdr::properties::TextProperties*	mpProperties;

	::com::sun::star::table::CellContentType mnCellContentType;

	::rtl::OUString	msFormula;
	double			mfValue;
	::sal_Int32		mnError;
	::sal_Bool		mbMerged;
	::sal_Int32		mnRowSpan;
	::sal_Int32		mnColSpan;

	Rectangle maCellRect;

	::com::sun::star::uno::Reference< ::com::sun::star::table::XTable > mxTable;
};

// -----------------------------------------------------------------------------

} }

#endif
