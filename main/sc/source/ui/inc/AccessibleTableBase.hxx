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




#ifndef _SC_ACCESSIBLETABLEBASE_HXX
#define _SC_ACCESSIBLETABLEBASE_HXX

#include "AccessibleContextBase.hxx"
#include "global.hxx"
#include "address.hxx"
#include <com/sun/star/accessibility/XAccessibleTable.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/accessibility/XAccessibleTableSelection.hpp>
#include <cppuhelper/implbase2.hxx>

class ScTabViewShell;

/**	@descr
        This base class provides an implementation of the
        <code>AccessibleTable</code> service.
*/

typedef cppu::ImplHelper2< ::com::sun::star::accessibility::XAccessibleTable,
					::com::sun::star::accessibility::XAccessibleSelection>
					ScAccessibleTableBaseImpl;

class ScAccessibleTableBase :
			public ScAccessibleContextBase,
			public	 ::com::sun::star::accessibility::XAccessibleTableSelection,
			public ScAccessibleTableBaseImpl
{
public:
	//=====  internal  ========================================================
	ScAccessibleTableBase(
        const ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>& rxParent,
		ScDocument* pDoc,
		const ScRange& rRange);
protected:
	virtual ~ScAccessibleTableBase();
public:

    using ScAccessibleContextBase::disposing;
 	virtual void SAL_CALL disposing();

	///=====  XInterface  =====================================================

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
		::com::sun::star::uno::Type const & rType );

	virtual void SAL_CALL acquire() throw ();

	virtual void SAL_CALL release() throw ();

	///=====  XAccessibleTable  ================================================

    /// Returns the number of rows in the table.
    virtual sal_Int32 SAL_CALL
    			getAccessibleRowCount(  );

    /// Returns the number of columns in the table.
    virtual sal_Int32 SAL_CALL
    			getAccessibleColumnCount(  );

    /// Returns the description of the specified row in the table.
    virtual ::rtl::OUString SAL_CALL
    			getAccessibleRowDescription( sal_Int32 nRow );

    /// Returns the description text of the specified column in the table.
    virtual ::rtl::OUString SAL_CALL
    			getAccessibleColumnDescription( sal_Int32 nColumn );

    /** Returns the number of rows occupied by the Accessible at a specified row and column in the table.
    Returns 1 if it is only a cell and the number of rows the cell is merged if the cell is a merged cell.
    */
    virtual sal_Int32 SAL_CALL
    			getAccessibleRowExtentAt( sal_Int32 nRow, sal_Int32 nColumn );

    /** Returns the number of columns occupied by the Accessible at a specified row and column in the table.
    Returns 1 if it is only a cell and the number of columns the cell is merged if the cell is a merged cell.
    */
    virtual sal_Int32 SAL_CALL
    			getAccessibleColumnExtentAt( sal_Int32 nRow, sal_Int32 nColumn );

    /// Returns the row headers as an AccessibleTable.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    			getAccessibleRowHeaders(  );

    /// Returns the column headers as an AccessibleTable.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    			getAccessibleColumnHeaders(  );

    /// Returns the selected rows in a table.
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL
    			getSelectedAccessibleRows(  );

    /// Returns the selected columns in a table.
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL
    			getSelectedAccessibleColumns(  );

    /// Returns a boolean value indicating whether the specified row is selected.
    virtual sal_Bool SAL_CALL
    			isAccessibleRowSelected( sal_Int32 nRow );

    /// Returns a boolean value indicating whether the specified column is selected.
    virtual sal_Bool SAL_CALL
    			isAccessibleColumnSelected( sal_Int32 nColumn );

	/// Returns the Accessible at a specified row and column in the table.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    			getAccessibleCellAt( sal_Int32 nRow, sal_Int32 nColumn );

	/// Returns the caption for the table.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    			getAccessibleCaption(  );

    /// Returns the summary description of the table.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    			getAccessibleSummary(  );

    /// Returns a boolean value indicating whether the accessible at a specified row and column is selected.
    virtual sal_Bool SAL_CALL
    			isAccessibleSelected( sal_Int32 nRow, sal_Int32 nColumn );

	///=====  XAccessibleExtendedTable  ========================================

    /// Returns the index of the cell on the given position.
    virtual sal_Int32 SAL_CALL
    			getAccessibleIndex( sal_Int32 nRow, sal_Int32 nColumn );

    /// Returns the row number of an index in the table.
    virtual sal_Int32 SAL_CALL
    			getAccessibleRow( sal_Int32 nChildIndex );

    /// Returns the column number of an index in the table.
    virtual sal_Int32 SAL_CALL
    			getAccessibleColumn( sal_Int32 nChildIndex );

	//=====  XAccessibleContext  ==============================================

    ///	Return the number of currently visible children.
    // is overloaded to calculate this on demand
    virtual sal_Int32 SAL_CALL
    	getAccessibleChildCount(void);

    ///	Return the specified child or NULL if index is invalid.
    // is overloaded to calculate this on demand
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleChild(sal_Int32 nIndex);
	virtual sal_Bool SAL_CALL selectRow( sal_Int32 row ) ;
    virtual sal_Bool SAL_CALL selectColumn( sal_Int32 column ) ;
    virtual sal_Bool SAL_CALL unselectRow( sal_Int32 row ) ;
    virtual sal_Bool SAL_CALL unselectColumn( sal_Int32 column ) ;

protected:
    ///	Return this object's description.
	virtual ::rtl::OUString SAL_CALL
    	createAccessibleDescription(void);

    ///	Return the object's current name.
	virtual ::rtl::OUString SAL_CALL
    	createAccessibleName(void);

public:
	///	Return NULL to indicate that an empty relation set.
	virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleRelationSet> SAL_CALL
    	getAccessibleRelationSet(void);

    ///	Return the set of current states.
    // perhaps sometimes to be implemented
	virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
    	getAccessibleStateSet(void);

	///=====  XAccessibleSelection  ===========================================

    virtual void SAL_CALL
		selectAccessibleChild( sal_Int32 nChildIndex );

    virtual sal_Bool SAL_CALL
		isAccessibleChildSelected( sal_Int32 nChildIndex );

    virtual void SAL_CALL
		clearAccessibleSelection(  );

    virtual void SAL_CALL
		selectAllAccessibleChildren(  );

    virtual sal_Int32 SAL_CALL
		getSelectedAccessibleChildCount(  );

    virtual ::com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessible > SAL_CALL
		getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );

    virtual void SAL_CALL
		deselectAccessibleChild( sal_Int32 nSelectedChildIndex );

	///=====  XServiceInfo  ===================================================

    /**	Returns an identifier for the implementation of this object.
    */
	virtual ::rtl::OUString SAL_CALL
    	getImplementationName(void);

	///=====  XTypeProvider  ===================================================

	/// returns the possible types
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL
		getTypes();

	/**	Returns a implementation id.
    */
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId(void);

protected:
	/// contains the range of the table, because it could be a subrange of the complete table
	ScRange maRange;

	ScDocument*	mpDoc;

	void CommitTableModelChange(sal_Int32 nStartRow, sal_Int32 nStartCol, sal_Int32 nEndRow, sal_Int32 nEndCol, sal_uInt16 nId);
};


#endif
