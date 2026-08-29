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



#ifndef ACCESSIBILITY_EXT_ACCESSIBLEICONCHOICECTRLENTRY_HXX_
#define ACCESSIBILITY_EXT_ACCESSIBLEICONCHOICECTRLENTRY_HXX_

#include <deque>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleStateSet.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/accessibility/XAccessibleAction.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/compbase8.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/accessibletexthelper.hxx>
#include <tools/gen.hxx>

// forward ---------------------------------------------------------------

class SvxIconChoiceCtrlEntry;
class SvtIconChoiceCtrl;

//........................................................................
namespace accessibility
{
//........................................................................

// class AccessibleIconChoiceCtrlEntry ------------------------------------------

    typedef ::cppu::WeakAggComponentImplHelper8< ::com::sun::star::accessibility::XAccessible
												, ::com::sun::star::accessibility::XAccessibleContext
												, ::com::sun::star::accessibility::XAccessibleComponent
												, ::com::sun::star::accessibility::XAccessibleEventBroadcaster
												, ::com::sun::star::accessibility::XAccessibleText
                                                , ::com::sun::star::accessibility::XAccessibleAction
										    	, ::com::sun::star::lang::XServiceInfo
												, ::com::sun::star::lang::XEventListener > AccessibleIconChoiceCtrlEntry_BASE;

	/** the class AccessibleListBoxEntry represents the class for an accessible object of a listbox entry */
	class AccessibleIconChoiceCtrlEntry :	public ::comphelper::OBaseMutex,
							   		public AccessibleIconChoiceCtrlEntry_BASE,
									public ::comphelper::OCommonAccessibleText
	{
	private:
		/** The treelistbox control */
		SvtIconChoiceCtrl*					m_pIconCtrl;
		sal_Int32							m_nIndex;

	protected:
        /// client id in the AccessibleEventNotifier queue
        sal_uInt32                          m_nClientId;

		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > m_xParent;

	private:
    #ifdef ACCESSIBLE_EVENT_NOTIFICATION_ENABLED
    // (the following method is unused currently. If you need it, simply remove the #ifdef thing here and
    // in the cxx)
		/** notifies all listeners that this object has changed
			@param	_nEventId
				is the event id
			@param	_aOldValue
				is the old value
			@param	_aNewValue
				is the new value
		*/
		void	NotifyAccessibleEvent( sal_Int16 _nEventId,
								   	const ::com::sun::star::uno::Any& _aOldValue,
								   	const ::com::sun::star::uno::Any& _aNewValue );
    #endif

		Rectangle				GetBoundingBox_Impl() const;
		Rectangle				GetBoundingBoxOnScreen_Impl() const;
		sal_Bool				IsAlive_Impl() const;
		sal_Bool				IsShowing_Impl() const;

		Rectangle				GetBoundingBox();
		Rectangle				GetBoundingBoxOnScreen();
    	void 					EnsureIsAlive() const;

	protected:
		virtual ~AccessibleIconChoiceCtrlEntry();
		/** this function is called upon disposing the component
		*/
		virtual void SAL_CALL					disposing();

		// OCommonAccessibleText
		virtual ::rtl::OUString					implGetText();
		virtual ::com::sun::star::lang::Locale	implGetLocale();
		virtual void							implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex );

	public:
		/** Ctor()
			@param	_rListBox
				the view control
			@param	_pEntry
				the entry
			@param	_xParent
				is our parent accessible object
		*/
		AccessibleIconChoiceCtrlEntry( SvtIconChoiceCtrl& _rIconCtrl,
									   sal_uLong _nPos,
									   const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _xParent );

		// XTypeProvider
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

		// XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName();
		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

		// XServiceInfo - static methods
		static com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		static ::rtl::OUString getImplementationName_Static(void);

		// XEventListener
	    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

		// XAccessible
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  );

		// XAccessibleContext
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );
		virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

		// XAccessibleComponent
	    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
	    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
	    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  );
	    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  );
	    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  );
	    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
	    virtual void SAL_CALL grabFocus(  );
		virtual sal_Int32 SAL_CALL getForeground(  );
		virtual sal_Int32 SAL_CALL getBackground(  );

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

    	// XAccessibleEventBroadcaster
        using cppu::WeakAggComponentImplHelperBase::addEventListener;
		virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );
        using cppu::WeakAggComponentImplHelperBase::removeEventListener;
    	virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

        // XAccessibleAction
        virtual sal_Int32 SAL_CALL getAccessibleActionCount(  );
        virtual sal_Bool SAL_CALL doAccessibleAction( sal_Int32 nIndex );
        virtual ::rtl::OUString SAL_CALL getAccessibleActionDescription( sal_Int32 nIndex );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleKeyBinding > SAL_CALL getAccessibleActionKeyBinding( sal_Int32 nIndex );
	};

//........................................................................
}// namespace accessibility
//........................................................................

#endif // ACCESSIBILITY_EXT_ACCESSIBLELISTBOXENTRY_HXX_
