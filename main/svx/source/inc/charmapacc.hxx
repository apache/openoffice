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



#include <vos/mutex.hxx>
#include <tools/list.hxx>
#include <tools/color.hxx>
#include <tools/string.hxx>
#ifndef _IMAGE_HXX
#include <vcl/image.hxx>
#endif
#include <rtl/uuid.h>
#include <comphelper/accessibleselectionhelper.hxx>
#include <com/sun/star/accessibility/XAccessibleTable.hpp>

#include <vector>
class SvxShowCharSet;

namespace svx
{
	typedef	::cppu::ImplHelper1	<	::com::sun::star::accessibility::XAccessible
								>	OAccessibleHelper_Base_2;

	class SvxShowCharSetAcc;
	/** The class SvxShowCharSetVirtualAcc is used as a virtual class which contains the table and the scrollbar.
		In the vcl control, the table and the scrollbar exists in one class. This is not feasible for the accessibility api.
	*/
	class SvxShowCharSetVirtualAcc : public ::comphelper::OAccessibleComponentHelper,
									 public OAccessibleHelper_Base_2
	{
		SvxShowCharSet*		mpParent; // the vcl control
		SvxShowCharSetAcc*	m_pTable; // the table, which holds the characters shown by the vcl control
		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > m_xAcc; // the ref to the table
	protected:
		virtual ~SvxShowCharSetVirtualAcc();

		virtual void SAL_CALL disposing();

		virtual ::com::sun::star::awt::Rectangle SAL_CALL implGetBounds(  );
	public:
		SvxShowCharSetVirtualAcc( SvxShowCharSet* pParent );

		// XInterface
		DECLARE_XINTERFACE( )
		DECLARE_XTYPEPROVIDER( )

		// XAccessibleComponent
		virtual void SAL_CALL grabFocus(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
		//OAccessibleContextHelper
		// XAccessibleContext - still waiting to be overwritten
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) { return this; }
		virtual sal_Int32 SAL_CALL getForeground(  );
		virtual sal_Int32 SAL_CALL getBackground(  );


		// call the fireEvent method from the table when it exists.
		void SAL_CALL fireEvent(
					const sal_Int16 _nEventId,
					const ::com::sun::star::uno::Any& _rOldValue,
					const ::com::sun::star::uno::Any& _rNewValue
				);

		// simple access methods
		inline SvxShowCharSetAcc*	getTable() const { return m_pTable; }
		inline SvxShowCharSet* getCharSetControl() const { return mpParent; }
	};


	class SvxShowCharSetItemAcc;
	// ----------------
	// - SvxShowCharSetItem -
	// ----------------
	/** Simple struct to hold some information about the single items of the table.
	*/
	struct SvxShowCharSetItem
	{
		SvxShowCharSet&				mrParent;
		sal_uInt16						mnId;
		XubString					maText;
		Rectangle					maRect;
		SvxShowCharSetItemAcc*		m_pItem;
		SvxShowCharSetAcc*			m_pParent;
		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > m_xAcc;

		SvxShowCharSetItem( SvxShowCharSet& rParent,SvxShowCharSetAcc*	_pParent,sal_uInt16 _nPos );
		~SvxShowCharSetItem();

		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >    GetAccessible();
		 void                                                                                       ClearAccessible();
	};

	// -----------------------------------------------------------------------------

	typedef	::cppu::ImplHelper2	<	::com::sun::star::accessibility::XAccessible,
									::com::sun::star::accessibility::XAccessibleTable
								>	OAccessibleHelper_Base;
	// ---------------
	// - SvxShowCharSetAcc -
	// ---------------
	/** The table implemtentation of the vcl control.
	*/

	class SvxShowCharSetAcc : public ::comphelper::OAccessibleSelectionHelper,
							  public OAccessibleHelper_Base
	{
		::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > > m_aChildren;
		SvxShowCharSetVirtualAcc* m_pParent; // the virtual parent
	protected:
		virtual void SAL_CALL disposing();
	public:
		SvxShowCharSetAcc( SvxShowCharSetVirtualAcc* _pParent );

		DECLARE_XINTERFACE( )
		DECLARE_XTYPEPROVIDER( )

		// XAccessibleComponent
		virtual void SAL_CALL grabFocus(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );

		//OAccessibleContextHelper
		// XAccessibleContext - still waiting to be overwritten
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) { return this; }
		virtual sal_Int32 SAL_CALL getForeground(  );
		virtual sal_Int32 SAL_CALL getBackground(  );

		// XAccessibleTable
		virtual sal_Int32 SAL_CALL getAccessibleRowCount(  );
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


		inline void SAL_CALL fireEvent(
					const sal_Int16 _nEventId,
					const ::com::sun::star::uno::Any& _rOldValue,
					const ::com::sun::star::uno::Any& _rNewValue
				)
		{
			NotifyAccessibleEvent(_nEventId,_rOldValue,_rNewValue);
		}
	protected:

		virtual ~SvxShowCharSetAcc();

		// OCommonAccessibleSelection
        // return if the specified child is visible => watch for special ChildIndexes (ACCESSIBLE_SELECTION_CHILD_xxx)
	    virtual sal_Bool
            implIsSelected( sal_Int32 nAccessibleChildIndex );

	    // select the specified child => watch for special ChildIndexes (ACCESSIBLE_SELECTION_CHILD_xxx)
        virtual void
            implSelect( sal_Int32 nAccessibleChildIndex, sal_Bool bSelect );

		// OCommonAccessibleComponent
		/// implements the calculation of the bounding rectangle - still waiting to be overwritten
		virtual ::com::sun::star::awt::Rectangle SAL_CALL implGetBounds(  );
	};

	// ----------------
	// - SvxShowCharSetItemAcc -
	// ----------------
	/** The child implementation of the table.
	*/
	class SvxShowCharSetItemAcc : public ::comphelper::OAccessibleComponentHelper,
								  public OAccessibleHelper_Base_2
	{
	private:
		SvxShowCharSetItem*	mpParent;
	protected:
		virtual ~SvxShowCharSetItemAcc();

		// OCommonAccessibleComponent
		/// implements the calculation of the bounding rectangle - still waiting to be overwritten
		virtual ::com::sun::star::awt::Rectangle SAL_CALL implGetBounds(  );
	public:

		// XInterface
		DECLARE_XINTERFACE( )
		DECLARE_XTYPEPROVIDER( )

		SvxShowCharSetItemAcc( SvxShowCharSetItem* pParent );

		void    ParentDestroyed();

		// XAccessibleComponent
		virtual void SAL_CALL grabFocus(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );

		//OAccessibleContextHelper
		// XAccessibleContext - still waiting to be overwritten
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) { return this; }

		virtual sal_Int32 SAL_CALL getForeground(  ) { return mpParent->m_pParent->getForeground(); }
		virtual sal_Int32 SAL_CALL getBackground(  ) { return mpParent->m_pParent->getBackground(); }

		inline void SAL_CALL fireEvent(
					const sal_Int16 _nEventId,
					const ::com::sun::star::uno::Any& _rOldValue,
					const ::com::sun::star::uno::Any& _rNewValue
				)
		{
			NotifyAccessibleEvent(_nEventId,_rOldValue,_rNewValue);
		}
	};
}
