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


#ifndef ACCESSIBILITY_EXT_ACCESSIBILEGRIDCONTROLTABLECELL_HXX
#define ACCESSIBILITY_EXT_ACCESSIBILEGRIDCONTROLTABLECELL_HXX

#include <comphelper/accessibletexthelper.hxx>
#include <cppuhelper/implbase2.hxx>
#include "accessibility/extended/AccessibleGridControlBase.hxx"
#include <svtools/accessibletable.hxx>

namespace accessibility
{
	class AccessibleGridControlCell : public AccessibleGridControlBase
	{
	private:
		sal_Int32				m_nRowPos;		// the row number of the table cell
		sal_Int32				m_nColPos;		// the column id of the table cell

	protected:
		// attribute access
		inline sal_Int32	getRowPos( ) const { return m_nRowPos; }
		inline sal_Int32	getColumnPos( ) const { return m_nColPos; }

		// XAccessibleComponent
		virtual void SAL_CALL grabFocus();

	protected:
		AccessibleGridControlCell(
			const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxParent,
			::svt::table::IAccessibleTable& _rTable,
			sal_Int32 _nRowPos,
			sal_uInt16 _nColPos,
			::svt::table::AccessibleTableControlObjType _eType
		);

		virtual ~AccessibleGridControlCell();

	private:
		AccessibleGridControlCell();													// never implemented
		AccessibleGridControlCell( const AccessibleGridControlCell& );				// never implemented
		AccessibleGridControlCell& operator=( const AccessibleGridControlCell& );	// never implemented
	};

	typedef ::cppu::ImplHelper2	<	::com::sun::star::accessibility::XAccessibleText
								,	::com::sun::star::accessibility::XAccessible
								>	AccessibleTextHelper_BASE;
	// implementation of a table cell of GridControl
	class AccessibleGridControlTableCell	:public AccessibleGridControlCell
										,public AccessibleTextHelper_BASE
										,public ::comphelper::OCommonAccessibleText
	{
	protected:
		// OCommonAccessibleText
		virtual ::rtl::OUString					implGetText();
		virtual ::com::sun::star::lang::Locale	implGetLocale();
		virtual void							implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex );
		virtual Rectangle implGetBoundingBox();
		virtual Rectangle implGetBoundingBoxOnScreen();

	public:
        AccessibleGridControlTableCell( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxParent,
									::svt::table::IAccessibleTable& _rTable,
									sal_Int32 _nRowId,
									sal_uInt16 _nColId,
									svt::table::AccessibleTableControlObjType  eObjType);

		// XInterface -------------------------------------------------------------

		/** Queries for a new interface. */
		::com::sun::star::uno::Any SAL_CALL queryInterface(
				const ::com::sun::star::uno::Type& rType );

		/** Aquires the object (calls acquire() on base class). */
		virtual void SAL_CALL acquire() throw ();

		/** Releases the object (calls release() on base class). */
		virtual void SAL_CALL release() throw ();

		/** @return  The index of this object among the parent's children. */
		virtual sal_Int32 SAL_CALL getAccessibleIndexInParent();

		/** @return
				The name of this class.
		*/
		virtual ::rtl::OUString SAL_CALL getImplementationName();

		/** @return
				The count of visible children.
		*/
		virtual sal_Int32 SAL_CALL getAccessibleChildCount();

		/** @return
				The XAccessible interface of the specified child.
		*/
		virtual ::com::sun::star::uno::Reference<
			::com::sun::star::accessibility::XAccessible > SAL_CALL
			getAccessibleChild( sal_Int32 nChildIndex );

		/** Creates a new AccessibleStateSetHelper and fills it with states of the
			current object.
			@return
				A filled AccessibleStateSetHelper.
		*/
		::utl::AccessibleStateSetHelper* implCreateStateSetHelper();

		// XAccessible ------------------------------------------------------------

		/** @return  The XAccessibleContext interface of this object. */
		virtual ::com::sun::star::uno::Reference<
			::com::sun::star::accessibility::XAccessibleContext > SAL_CALL
		getAccessibleContext();

		// XAccessibleText
		virtual sal_Int32 SAL_CALL getCaretPosition();
		virtual sal_Bool SAL_CALL setCaretPosition( sal_Int32 nIndex );
		virtual sal_Unicode SAL_CALL getCharacter( sal_Int32 nIndex );
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRequestedAttributes );
		virtual ::com::sun::star::awt::Rectangle SAL_CALL getCharacterBounds( sal_Int32 nIndex );
		virtual sal_Int32 SAL_CALL getCharacterCount();
		virtual sal_Int32 SAL_CALL getIndexAtPoint( const ::com::sun::star::awt::Point& aPoint );
		virtual ::rtl::OUString SAL_CALL getSelectedText();
		virtual sal_Int32 SAL_CALL getSelectionStart();
		virtual sal_Int32 SAL_CALL getSelectionEnd();
		virtual sal_Bool SAL_CALL setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
		virtual ::rtl::OUString SAL_CALL getText();
		virtual ::rtl::OUString SAL_CALL getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType );
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType );
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType );
		virtual sal_Bool SAL_CALL copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
	};
}
#endif // ACCESSIBILITY_EXT_ACCESSIBILEGRIDCONTROLTABLECELL_HXX
