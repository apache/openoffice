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




#ifndef _SC_ACCESSIBLEPREVIEWTABLE_HXX
#define _SC_ACCESSIBLEPREVIEWTABLE_HXX

#include "AccessibleContextBase.hxx"
#include <com/sun/star/accessibility/XAccessibleTable.hpp>
#include <cppuhelper/implbase1.hxx>

class ScPreviewShell;
class ScPreviewTableInfo;

typedef cppu::ImplHelper1< ::com::sun::star::accessibility::XAccessibleTable>
					ScAccessiblePreviewTableImpl;

class ScAccessiblePreviewTable :
		public ScAccessibleContextBase,
		public ScAccessiblePreviewTableImpl
{
public:
	ScAccessiblePreviewTable( const ::com::sun::star::uno::Reference<
						        ::com::sun::star::accessibility::XAccessible>& rxParent,
							ScPreviewShell* pViewShell, sal_Int32 nIndex );

protected:
	virtual ~ScAccessiblePreviewTable();

    using ScAccessibleContextBase::IsDefunc;

public:
    using ScAccessibleContextBase::disposing;
 	virtual void SAL_CALL disposing();

	//=====  SfxListener  =====================================================

	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

	///=====  XInterface  =====================================================

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
		::com::sun::star::uno::Type const & rType );

	virtual void SAL_CALL acquire() throw ();

	virtual void SAL_CALL release() throw ();

	//=====  XAccessibleTable  ================================================

    virtual sal_Int32 SAL_CALL getAccessibleRowCount();
    virtual sal_Int32 SAL_CALL getAccessibleColumnCount();
    virtual ::rtl::OUString SAL_CALL getAccessibleRowDescription( sal_Int32 nRow );
    virtual ::rtl::OUString SAL_CALL getAccessibleColumnDescription( sal_Int32 nColumn );
    virtual sal_Int32 SAL_CALL getAccessibleRowExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
    virtual sal_Int32 SAL_CALL getAccessibleColumnExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    						getAccessibleRowHeaders();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    						getAccessibleColumnHeaders();
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleRows();
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleColumns();
    virtual sal_Bool SAL_CALL isAccessibleRowSelected( sal_Int32 nRow );
    virtual sal_Bool SAL_CALL isAccessibleColumnSelected( sal_Int32 nColumn );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    						getAccessibleCellAt( sal_Int32 nRow, sal_Int32 nColumn );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    						getAccessibleCaption();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    						getAccessibleSummary();
    virtual sal_Bool SAL_CALL isAccessibleSelected( sal_Int32 nRow, sal_Int32 nColumn );
    virtual sal_Int32 SAL_CALL getAccessibleIndex( sal_Int32 nRow, sal_Int32 nColumn );
    virtual sal_Int32 SAL_CALL getAccessibleRow( sal_Int32 nChildIndex );
    virtual sal_Int32 SAL_CALL getAccessibleColumn( sal_Int32 nChildIndex );

	//=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    						getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual void SAL_CALL	grabFocus();

	//=====  XAccessibleContext  ==============================================

    virtual sal_Int32 SAL_CALL getAccessibleChildCount();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    						getAccessibleChild( sal_Int32 i );
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL
    						getAccessibleStateSet();

	//=====  XServiceInfo  ====================================================

	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	//=====  XTypeProvider  ===================================================

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL
		getTypes();

	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

    //=====  internal  ========================================================
    void SetCurrentIndexInParent(sal_Int32 nNew) { mnIndex = nNew; }

protected:
	virtual ::rtl::OUString SAL_CALL createAccessibleDescription(void);
	virtual ::rtl::OUString SAL_CALL createAccessibleName(void);

	virtual Rectangle GetBoundingBoxOnScreen(void) const;
	virtual Rectangle GetBoundingBox(void) const;

private:
	ScPreviewShell*		mpViewShell;
	sal_Int32			mnIndex;
	mutable ScPreviewTableInfo*	mpTableInfo;

	sal_Bool IsDefunc(
		const com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessibleStateSet>& rxParentStates);

	void	FillTableInfo() const;
};


#endif
