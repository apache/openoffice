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



#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_TABLE_SHAPE_HXX
#define _SVX_ACCESSIBILITY_ACCESSIBLE_TABLE_SHAPE_HXX

#include <com/sun/star/table/XTable.hpp>
#include <com/sun/star/accessibility/XAccessibleTable.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/util/XModifyListener.hpp>

#include <rtl/ref.hxx>

#include <cppuhelper/implbase2.hxx>

#include <svx/AccessibleShape.hxx>
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#include <com/sun/star/accessibility/XAccessibleTableSelection.hpp>
#include <cppuhelper/compbase5.hxx>

#include <boost/noncopyable.hpp>

namespace sdr { namespace table {
	class SvxTableController;
} }

namespace accessibility
{
	class AccessibleTableShapeImpl;
	class AccessibleCell;

	/*typedef ::cppu::ImplInheritanceHelper2<	AccessibleShape,
											::com::sun::star::accessibility::XAccessibleSelection,
											::com::sun::star::accessibility::XAccessibleTable
										  >	AccessibleTableShape_Base;*/
	typedef ::cppu::ImplInheritanceHelper2<	AccessibleShape,
											::com::sun::star::accessibility::XAccessibleTable,
											::com::sun::star::view::XSelectionChangeListener
										  >	AccessibleTableShape_Base;
/**	@descr
*/
class AccessibleTableShape : boost::noncopyable, public AccessibleTableShape_Base, public ::com::sun::star::accessibility::XAccessibleTableSelection
{
public:
	AccessibleTableShape( const AccessibleShapeInfo& rShapeInfo, const AccessibleShapeTreeInfo& rShapeTreeInfo );
	virtual ~AccessibleTableShape( );

    virtual void Init (void);

	// XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw ();
    virtual void SAL_CALL release(  ) throw ();

	// XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL getAccessibleContext( );

	// XAccessibleContext
    virtual sal_Int32 SAL_CALL getAccessibleChildCount( );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
	virtual sal_Int16 SAL_CALL getAccessibleRole (void);

	// XAccessibleTable
	virtual sal_Int32 SAL_CALL getAccessibleRowCount();
	virtual sal_Int32 SAL_CALL getAccessibleColumnCount(  );
	virtual ::rtl::OUString SAL_CALL getAccessibleRowDescription( sal_Int32 nRow );
	virtual ::rtl::OUString SAL_CALL getAccessibleColumnDescription( sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleRowExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleColumnExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL getAccessibleRowHeaders(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL getAccessibleColumnHeaders(  );
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleRows(  );
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleColumns(  );
	virtual sal_Bool SAL_CALL isAccessibleRowSelected( sal_Int32 nRow );
	virtual sal_Bool SAL_CALL isAccessibleColumnSelected( sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleCellAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleCaption(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleSummary(  );
	virtual sal_Bool SAL_CALL isAccessibleSelected( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleIndex( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleRow( sal_Int32 nChildIndex );
	virtual sal_Int32 SAL_CALL getAccessibleColumn( sal_Int32 nChildIndex );

	// XAccessibleSelection
    virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex );
    virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex );
    virtual void SAL_CALL clearAccessibleSelection(  );
    virtual void SAL_CALL selectAllAccessibleChildren(  );
    virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );
    virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nChildIndex );
	//=====  XAccessibleTableSelection ============================================
	virtual sal_Bool SAL_CALL selectRow( sal_Int32 row ) ;
	virtual sal_Bool SAL_CALL selectColumn( sal_Int32 column ) ;
	virtual sal_Bool SAL_CALL unselectRow( sal_Int32 row ) ;
	virtual sal_Bool SAL_CALL unselectColumn( sal_Int32 column ) ;

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName( );

    // XComponent
    virtual void SAL_CALL disposing( );

    // XSelectionChangeListener
    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& Source);
    virtual void  SAL_CALL
        selectionChanged (const ::com::sun::star::lang::EventObject& rEvent);
	sal_Int32 mnPreviousSelectionCount;
	using AccessibleShape::disposing;
	friend class AccessibleTableHeaderShape;

	void getColumnAndRow( sal_Int32 nChildIndex, sal_Int32& rnColumn, sal_Int32& rnRow );
	// overwrite the SetState & ResetState to do special operation for table cell's internal text
	virtual sal_Bool SetState (sal_Int16 aState);
	virtual sal_Bool ResetState (sal_Int16 aState);
	// The following two methods are used to set state directly on table object, instread of the internal cell or paragraph.
	sal_Bool SetStateDirectly (sal_Int16 aState);
	sal_Bool ResetStateDirectly (sal_Int16 aState);
	// Get the currently active cell which is text editing
	AccessibleCell* GetActiveAccessibleCell();

protected:
	virtual ::rtl::OUString CreateAccessibleBaseName(void);

	sdr::table::SvxTableController* getTableController();

	void checkCellPosition( sal_Int32 nCol, sal_Int32 nRow );
	//IAccessibility2 Implementation 2009, move to public
	//void getColumnAndRow( sal_Int32 nChildIndex, sal_Int32& rnColumn, sal_Int32& rnRow ) throw (::com::sun::star::lang::IndexOutOfBoundsException );

private:
	rtl::Reference< AccessibleTableShapeImpl > mxImpl;
	sal_Int32 GetIndexOfSelectedChild( sal_Int32 nSelectedChildIndex ) const;
};

typedef ::cppu::WeakImplHelper5<
			::com::sun::star::accessibility::XAccessible,
			::com::sun::star::accessibility::XAccessibleComponent,
			::com::sun::star::accessibility::XAccessibleContext,
			::com::sun::star::accessibility::XAccessibleTable,
			::com::sun::star::accessibility::XAccessibleTableSelection >
			AccessibleTableHeaderShape_BASE;

class AccessibleTableHeaderShape : boost::noncopyable,
	public MutexOwner,
	public AccessibleTableHeaderShape_BASE
{
public:
	// bRow, sal_True means rowheader, sal_False means columnheader
	AccessibleTableHeaderShape( AccessibleTableShape* pTable, sal_Bool bRow );
	virtual ~AccessibleTableHeaderShape();

	// XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL getAccessibleContext( );

	// XAccessibleContext
	virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  );
    virtual sal_Int16 SAL_CALL getAccessibleRole(  );
    virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
    virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

	//XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual sal_Int32 SAL_CALL getForeground(  );
    virtual sal_Int32 SAL_CALL getBackground(  );
    virtual void SAL_CALL grabFocus(  );

	// XAccessibleTable
	virtual sal_Int32 SAL_CALL getAccessibleRowCount();
	virtual sal_Int32 SAL_CALL getAccessibleColumnCount(  );
	virtual ::rtl::OUString SAL_CALL getAccessibleRowDescription( sal_Int32 nRow );
	virtual ::rtl::OUString SAL_CALL getAccessibleColumnDescription( sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleRowExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleColumnExtentAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL getAccessibleRowHeaders(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL getAccessibleColumnHeaders(  );
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleRows(  );
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getSelectedAccessibleColumns(  );
	virtual sal_Bool SAL_CALL isAccessibleRowSelected( sal_Int32 nRow );
	virtual sal_Bool SAL_CALL isAccessibleColumnSelected( sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleCellAt( sal_Int32 nRow, sal_Int32 nColumn );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleCaption(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleSummary(  );
	virtual sal_Bool SAL_CALL isAccessibleSelected( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleIndex( sal_Int32 nRow, sal_Int32 nColumn );
	virtual sal_Int32 SAL_CALL getAccessibleRow( sal_Int32 nChildIndex );
	virtual sal_Int32 SAL_CALL getAccessibleColumn( sal_Int32 nChildIndex );

	//=====  XAccessibleTableSelection  ============================================
	virtual sal_Bool SAL_CALL selectRow( sal_Int32 row ) ;
	virtual sal_Bool SAL_CALL selectColumn( sal_Int32 column ) ;
	virtual sal_Bool SAL_CALL unselectRow( sal_Int32 row ) ;
	virtual sal_Bool SAL_CALL unselectColumn( sal_Int32 column ) ;
private:
	SVX_DLLPRIVATE explicit AccessibleTableHeaderShape( const ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>& rxParent );
	sal_Bool mbRow;
	rtl::Reference< AccessibleTableShape > mpTable;
};

} // end of namespace accessibility

#endif
