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



#ifndef SC_FMTUNO_HXX
#define SC_FMTUNO_HXX

#include "address.hxx"
#include "conditio.hxx"
#include <formula/grammar.hxx>
#include <tools/list.hxx>
#include <svl/itemprop.hxx>
#include <com/sun/star/sheet/XSheetConditionalEntries.hpp>
#include <com/sun/star/sheet/XSheetCondition.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntry.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <com/sun/star/sheet/XMultiFormulaTokens.hpp>
#include <com/sun/star/sheet/FormulaToken.hpp>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>


class ScDocument;
class ScTableConditionalEntry;
class ScConditionalFormat;
class ScValidationData;


struct ScCondFormatEntryItem
{
    ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > maTokens1;
    ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > maTokens2;
    String              maExpr1;
    String              maExpr2;
    String              maExprNmsp1;
    String              maExprNmsp2;
    String              maPosStr;  // formula position as text
    String              maStyle;   // display name as stored in ScStyleSheet
    ScAddress           maPos;
    formula::FormulaGrammar::Grammar meGrammar1; // grammar used with maExpr1
    formula::FormulaGrammar::Grammar meGrammar2; // grammar used with maExpr2
    ScConditionMode     meMode;

    // Make sure the grammar is initialized for API calls.
    ScCondFormatEntryItem();
};

class ScTableConditionalFormat : public cppu::WeakImplHelper5<
							com::sun::star::sheet::XSheetConditionalEntries,
							com::sun::star::container::XNameAccess,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::lang::XUnoTunnel,
							com::sun::star::lang::XServiceInfo >
{
private:
	List	aEntries;

	ScTableConditionalEntry*	GetObjectByIndex_Impl(sal_uInt16 nIndex) const;
    void                        AddEntry_Impl(const ScCondFormatEntryItem& aEntry);

	ScTableConditionalFormat(); // disable
public:
							ScTableConditionalFormat(ScDocument* pDoc, sal_uLong nKey,
                                formula::FormulaGrammar::Grammar eGrammar);
	virtual					~ScTableConditionalFormat();

    void                    FillFormat( ScConditionalFormat& rFormat, ScDocument* pDoc,
                                formula::FormulaGrammar::Grammar eGrammar) const;

							// XSheetConditionalEntries
	virtual void SAL_CALL	addNew( const ::com::sun::star::uno::Sequence<
									::com::sun::star::beans::PropertyValue >& aConditionalEntry );
	virtual void SAL_CALL	removeByIndex( sal_Int32 nIndex );
	virtual void SAL_CALL	clear();

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScTableConditionalFormat* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::sheet::XSheetConditionalEntries> xObj );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};



class ScTableConditionalEntry : public cppu::WeakImplHelper3<
							com::sun::star::sheet::XSheetCondition,
							com::sun::star::sheet::XSheetConditionalEntry,
							com::sun::star::lang::XServiceInfo >
{
private:
    ScCondFormatEntryItem       aData;

	ScTableConditionalEntry(); // disabled
public:
                            ScTableConditionalEntry(const ScCondFormatEntryItem& aItem);
	virtual					~ScTableConditionalEntry();

    void                    GetData(ScCondFormatEntryItem& rData) const;

							// XSheetCondition
	virtual ::com::sun::star::sheet::ConditionOperator SAL_CALL getOperator();
	virtual void SAL_CALL	setOperator( ::com::sun::star::sheet::ConditionOperator nOperator );
	virtual ::rtl::OUString SAL_CALL getFormula1();
	virtual void SAL_CALL	setFormula1( const ::rtl::OUString& aFormula1 );
	virtual ::rtl::OUString SAL_CALL getFormula2();
	virtual void SAL_CALL	setFormula2( const ::rtl::OUString& aFormula2 );
	virtual ::com::sun::star::table::CellAddress SAL_CALL getSourcePosition();
	virtual void SAL_CALL setSourcePosition( const ::com::sun::star::table::CellAddress& aSourcePosition );

							// XSheetConditionalEntry
	virtual ::rtl::OUString	SAL_CALL getStyleName();
	virtual void SAL_CALL	setStyleName( const ::rtl::OUString& aStyleName );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};



class ScTableValidationObj : public cppu::WeakImplHelper5<
							com::sun::star::sheet::XSheetCondition,
                            com::sun::star::sheet::XMultiFormulaTokens,
							com::sun::star::beans::XPropertySet,
							com::sun::star::lang::XUnoTunnel,
							com::sun::star::lang::XServiceInfo >
{
private:
	SfxItemPropertySet	aPropSet;
	sal_uInt16				nMode;			// enum ScConditionMode
	String				aExpr1;
	String				aExpr2;
    String              maExprNmsp1;
    String              maExprNmsp2;
    formula::FormulaGrammar::Grammar  meGrammar1;      // grammar used with aExpr1 and aExpr2
    formula::FormulaGrammar::Grammar  meGrammar2;      // grammar used with aExpr1 and aExpr2
    ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > aTokens1;
    ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > aTokens2;
	ScAddress			aSrcPos;
    String              aPosString;     // formula position as text
	sal_uInt16				nValMode;		// enum ScValidationMode
	sal_Bool				bIgnoreBlank;
    sal_Int16           nShowList;
	sal_Bool				bShowInput;
	String				aInputTitle;
	String				aInputMessage;
	sal_Bool				bShowError;
	sal_uInt16				nErrorStyle;	// enum ScValidErrorStyle
	String				aErrorTitle;
	String				aErrorMessage;

	void					ClearData_Impl();

	ScTableValidationObj(); // disabled
public:
							ScTableValidationObj(ScDocument* pDoc, sal_uLong nKey,
												const formula::FormulaGrammar::Grammar eGrammar);
	virtual					~ScTableValidationObj();

	ScValidationData*		CreateValidationData( ScDocument* pDoc,
												formula::FormulaGrammar::Grammar eGrammar ) const;

							// XSheetCondition
	virtual ::com::sun::star::sheet::ConditionOperator SAL_CALL getOperator();
	virtual void SAL_CALL	setOperator( ::com::sun::star::sheet::ConditionOperator nOperator );
	virtual ::rtl::OUString SAL_CALL getFormula1();
	virtual void SAL_CALL	setFormula1( const ::rtl::OUString& aFormula1 );
	virtual ::rtl::OUString SAL_CALL getFormula2();
	virtual void SAL_CALL	setFormula2( const ::rtl::OUString& aFormula2 );
	virtual ::com::sun::star::table::CellAddress SAL_CALL getSourcePosition();
	virtual void SAL_CALL setSourcePosition( const ::com::sun::star::table::CellAddress& aSourcePosition );

                            // XMultiFormulaTokens
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken >
                            SAL_CALL getTokens( sal_Int32 nIndex );
    virtual void SAL_CALL setTokens( sal_Int32 nIndex,
                                     const ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken >& aTokens );
    virtual sal_Int32 SAL_CALL getCount();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScTableValidationObj* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::beans::XPropertySet> xObj );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


#endif
