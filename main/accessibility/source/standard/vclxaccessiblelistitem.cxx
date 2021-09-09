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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_accessibility.hxx"
#include <accessibility/standard/vclxaccessiblelistitem.hxx>
#include <toolkit/helper/convert.hxx>
#include <accessibility/helper/listboxhelper.hxx>
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/awt/Size.hpp>

#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboard.hpp>
#include <com/sun/star/datatransfer/clipboard/XFlushableClipboard.hpp>
#include <tools/debug.hxx>
#include <vcl/svapp.hxx>
#include <vcl/controllayout.hxx>
#include <vcl/unohelp2.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <unotools/accessiblestatesethelper.hxx>
#include <unotools/accessiblerelationsethelper.hxx>
#include <cppuhelper/typeprovider.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/accessibleeventnotifier.hxx>

namespace
{
	void checkIndex_Impl( sal_Int32 _nIndex, const ::rtl::OUString& _sText ) throw (::com::sun::star::lang::IndexOutOfBoundsException)
	{
		if ( _nIndex < 0 || _nIndex > _sText.getLength() )
			throw ::com::sun::star::lang::IndexOutOfBoundsException();
	}
}

// class VCLXAccessibleListItem ------------------------------------------

using namespace ::com::sun::star::accessibility;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star;

DBG_NAME(VCLXAccessibleListItem)

// -----------------------------------------------------------------------------
// Ctor() and Dtor()
// -----------------------------------------------------------------------------
VCLXAccessibleListItem::VCLXAccessibleListItem( ::accessibility::IComboListBoxHelper* _pListBoxHelper, sal_Int32 _nIndexInParent, const Reference< XAccessible >& _xParent ) :

	VCLXAccessibleListItem_BASE	( m_aMutex ),

	m_nIndexInParent( _nIndexInParent ),
	m_bSelected		( sal_False ),
	m_bVisible		( sal_False ),
	m_nClientId		( 0 ),
	m_pListBoxHelper( _pListBoxHelper ),
	m_xParent		( _xParent )

{
	DBG_CTOR( VCLXAccessibleListItem, NULL );

	if ( m_xParent.is() )
		m_xParentContext = m_xParent->getAccessibleContext();

	if ( m_pListBoxHelper )
		m_sEntryText = m_pListBoxHelper->GetEntry( (sal_uInt16)_nIndexInParent );
}
// -----------------------------------------------------------------------------
VCLXAccessibleListItem::~VCLXAccessibleListItem()
{
	DBG_DTOR( VCLXAccessibleListItem, NULL );
}
// -----------------------------------------------------------------------------
void VCLXAccessibleListItem::SetSelected( sal_Bool _bSelected )
{
	if ( m_bSelected != _bSelected )
	{
		Any aOldValue;
		Any aNewValue;
		if ( m_bSelected )
			aOldValue <<= AccessibleStateType::SELECTED;
		else
			aNewValue <<= AccessibleStateType::SELECTED;
		m_bSelected = _bSelected;
		NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
	}
}
// -----------------------------------------------------------------------------
void VCLXAccessibleListItem::SetVisible( sal_Bool _bVisible )
{
	if ( m_bVisible != _bVisible )
	{
		Any aOldValue, aNewValue;
		m_bVisible = _bVisible;
		(_bVisible ? aNewValue : aOldValue ) <<= AccessibleStateType::VISIBLE;
		NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
		(_bVisible ? aNewValue : aOldValue ) <<= AccessibleStateType::SHOWING;
		NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
	}
}
// -----------------------------------------------------------------------------
void VCLXAccessibleListItem::NotifyAccessibleEvent( sal_Int16 _nEventId,
													const ::com::sun::star::uno::Any& _aOldValue,
													const ::com::sun::star::uno::Any& _aNewValue )
{
	AccessibleEventObject aEvt;
	aEvt.Source = *this;
	aEvt.EventId = _nEventId;
	aEvt.OldValue = _aOldValue;
	aEvt.NewValue = _aNewValue;

	if (m_nClientId)
		comphelper::AccessibleEventNotifier::addEvent( m_nClientId, aEvt );
}
// -----------------------------------------------------------------------------
// OCommonAccessibleText
// -----------------------------------------------------------------------------
::rtl::OUString VCLXAccessibleListItem::implGetText()
{
	return m_sEntryText;
}
// -----------------------------------------------------------------------------
Locale VCLXAccessibleListItem::implGetLocale()
{
	return Application::GetSettings().GetLocale();
}
// -----------------------------------------------------------------------------
void VCLXAccessibleListItem::implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex )
{
	nStartIndex = 0;
	nEndIndex = 0;
}
// -----------------------------------------------------------------------------
// XInterface
// -----------------------------------------------------------------------------
Any SAL_CALL VCLXAccessibleListItem::queryInterface( Type const & rType ) throw (RuntimeException)
{
	return VCLXAccessibleListItem_BASE::queryInterface( rType );
}
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::acquire() throw ()
{
	VCLXAccessibleListItem_BASE::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::release() throw ()
{
	VCLXAccessibleListItem_BASE::release();
}
// -----------------------------------------------------------------------------
// XTypeProvider
// -----------------------------------------------------------------------------
Sequence< Type > SAL_CALL VCLXAccessibleListItem::getTypes(  ) throw (RuntimeException)
{
	return VCLXAccessibleListItem_BASE::getTypes();
}
// -----------------------------------------------------------------------------
Sequence< sal_Int8 > VCLXAccessibleListItem::getImplementationId() throw (RuntimeException)
{
	static ::cppu::OImplementationId* pId = NULL;

	if ( !pId )
	{
		::osl::Guard< ::osl::Mutex > aGuard( m_aMutex );

		if ( !pId )
		{
			static ::cppu::OImplementationId aId;
			pId = &aId;
		}
	}
	return pId->getImplementationId();
}
// -----------------------------------------------------------------------------
// XComponent
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::disposing()
{
	comphelper::AccessibleEventNotifier::TClientId nId( 0 );
	Reference< XInterface > xEventSource;
	{
		::osl::MutexGuard aGuard( m_aMutex );

		VCLXAccessibleListItem_BASE::disposing();
		m_sEntryText		= ::rtl::OUString();
		m_pListBoxHelper	= NULL;
		m_xParent			= NULL;
		m_xParentContext	= NULL;

		nId = m_nClientId;
		m_nClientId = 0;
		if ( nId )
			xEventSource = *this;
	}

	// Send a disposing to all listeners.
	if ( nId )
		comphelper::AccessibleEventNotifier::revokeClientNotifyDisposing( nId, *this );
}
// -----------------------------------------------------------------------------
// XServiceInfo
// -----------------------------------------------------------------------------
::rtl::OUString VCLXAccessibleListItem::getImplementationName() throw (RuntimeException)
{
	return ::rtl::OUString::createFromAscii( "com.sun.star.comp.toolkit.AccessibleListItem" );
}
// -----------------------------------------------------------------------------
sal_Bool VCLXAccessibleListItem::supportsService( const ::rtl::OUString& rServiceName ) throw (RuntimeException)
{
	Sequence< ::rtl::OUString > aNames( getSupportedServiceNames() );
	const ::rtl::OUString* pNames = aNames.getConstArray();
	const ::rtl::OUString* pEnd = pNames + aNames.getLength();
	for ( ; pNames != pEnd && !pNames->equals( rServiceName ); ++pNames )
		;

	return pNames != pEnd;
}
// -----------------------------------------------------------------------------
Sequence< ::rtl::OUString > VCLXAccessibleListItem::getSupportedServiceNames() throw (RuntimeException)
{
	Sequence< ::rtl::OUString > aNames(3);
	aNames[0] = ::rtl::OUString::createFromAscii( "com.sun.star.accessibility.AccessibleContext" );
	aNames[1] = ::rtl::OUString::createFromAscii( "com.sun.star.accessibility.AccessibleComponent" );
	aNames[2] = ::rtl::OUString::createFromAscii( "com.sun.star.accessibility.AccessibleListItem" );
	return aNames;
}
// -----------------------------------------------------------------------------
// XAccessible
// -----------------------------------------------------------------------------
Reference< XAccessibleContext > SAL_CALL VCLXAccessibleListItem::getAccessibleContext(  ) throw (RuntimeException)
{
	return this;
}
// -----------------------------------------------------------------------------
// XAccessibleContext
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getAccessibleChildCount(  ) throw (RuntimeException)
{
	return 0;
}
// -----------------------------------------------------------------------------
Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleChild( sal_Int32 ) throw (RuntimeException)
{
	return Reference< XAccessible >();
}
// -----------------------------------------------------------------------------
Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleParent(  ) throw (RuntimeException)
{
	::osl::MutexGuard aGuard( m_aMutex );

	return m_xParent;
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getAccessibleIndexInParent(  ) throw (RuntimeException)
{
	::osl::MutexGuard aGuard( m_aMutex );
	return m_nIndexInParent;
}
// -----------------------------------------------------------------------------
sal_Int16 SAL_CALL VCLXAccessibleListItem::getAccessibleRole(  ) throw (RuntimeException)
{
	return AccessibleRole::LIST_ITEM;
	//	return AccessibleRole::LABEL;
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL VCLXAccessibleListItem::getAccessibleDescription(  ) throw (RuntimeException)
{
	// no description for every item
	return ::rtl::OUString();
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL VCLXAccessibleListItem::getAccessibleName(  ) throw (RuntimeException)
{
	::osl::MutexGuard aGuard( m_aMutex );

	// entry text == accessible name
	return implGetText();
}
// -----------------------------------------------------------------------------
Reference< XAccessibleRelationSet > SAL_CALL VCLXAccessibleListItem::getAccessibleRelationSet(  ) throw (RuntimeException)
{
	utl::AccessibleRelationSetHelper* pRelationSetHelper = new utl::AccessibleRelationSetHelper;
	Reference< XAccessibleRelationSet > xSet = pRelationSetHelper;
	return xSet;
}
// -----------------------------------------------------------------------------
Reference< XAccessibleStateSet > SAL_CALL VCLXAccessibleListItem::getAccessibleStateSet(  ) throw (RuntimeException)
{
	::osl::MutexGuard aGuard( m_aMutex );

	utl::AccessibleStateSetHelper* pStateSetHelper = new utl::AccessibleStateSetHelper;
	Reference< XAccessibleStateSet > xStateSet = pStateSetHelper;

	if ( !rBHelper.bDisposed && !rBHelper.bInDispose )
	{
		pStateSetHelper->AddState( AccessibleStateType::TRANSIENT );

		if(m_pListBoxHelper->IsEnabled())
		{
			pStateSetHelper->AddState( AccessibleStateType::SELECTABLE );
			pStateSetHelper->AddState( AccessibleStateType::ENABLED );
			pStateSetHelper->AddState( AccessibleStateType::SENSITIVE );
		}

		if ( m_bSelected )
			pStateSetHelper->AddState( AccessibleStateType::SELECTED );
		if ( m_bVisible )
		{
			pStateSetHelper->AddState( AccessibleStateType::VISIBLE );
			pStateSetHelper->AddState( AccessibleStateType::SHOWING );
		}
	}
	else
		pStateSetHelper->AddState( AccessibleStateType::DEFUNC );

	return xStateSet;
}
// -----------------------------------------------------------------------------
Locale SAL_CALL VCLXAccessibleListItem::getLocale(  ) throw (IllegalAccessibleComponentStateException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return implGetLocale();
}
// -----------------------------------------------------------------------------
// XAccessibleComponent
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL VCLXAccessibleListItem::containsPoint( const awt::Point& _aPoint ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	sal_Bool bInside = sal_False;
	if ( m_pListBoxHelper )
	{
		Rectangle aRect( m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent ) );
		aRect.Move(-aRect.TopLeft().X(),-aRect.TopLeft().Y());
		bInside = aRect.IsInside( VCLPoint( _aPoint ) );
	}
	return bInside;
}
// -----------------------------------------------------------------------------
Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleAtPoint( const awt::Point& ) throw (RuntimeException)
{
	return Reference< XAccessible >();
}
// -----------------------------------------------------------------------------
awt::Rectangle SAL_CALL VCLXAccessibleListItem::getBounds(  ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	awt::Rectangle aRect;
	if ( m_pListBoxHelper )
		aRect = AWTRectangle( m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent ) );

	return aRect;
}
// -----------------------------------------------------------------------------
awt::Point SAL_CALL VCLXAccessibleListItem::getLocation(  ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	Point aPoint(0,0);
	if ( m_pListBoxHelper )
	{
		Rectangle aRect = m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent );
		aPoint = aRect.TopLeft();
	}
	return AWTPoint( aPoint );
}
// -----------------------------------------------------------------------------
awt::Point SAL_CALL VCLXAccessibleListItem::getLocationOnScreen(  ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	Point aPoint(0,0);
	if ( m_pListBoxHelper )
	{
		Rectangle aRect = m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent );
		aPoint = aRect.TopLeft();
		aPoint += m_pListBoxHelper->GetWindowExtentsRelative( NULL ).TopLeft();
	}
	return AWTPoint( aPoint );
}
// -----------------------------------------------------------------------------
awt::Size SAL_CALL VCLXAccessibleListItem::getSize(  ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	Size aSize;
	if ( m_pListBoxHelper )
		aSize = m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent ).GetSize();

	return AWTSize( aSize );
}
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::grabFocus(  ) throw (RuntimeException)
{
	// no focus for each item
}
// -----------------------------------------------------------------------------
// XAccessibleText
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getCaretPosition() throw (RuntimeException)
{
	return -1;
}
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL VCLXAccessibleListItem::setCaretPosition( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	if ( !implIsValidRange( nIndex, nIndex, implGetText().getLength() ) )
		throw IndexOutOfBoundsException();

	return sal_False;
}
// -----------------------------------------------------------------------------
sal_Unicode SAL_CALL VCLXAccessibleListItem::getCharacter( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getCharacter( nIndex );
}
// -----------------------------------------------------------------------------
Sequence< PropertyValue > SAL_CALL VCLXAccessibleListItem::getCharacterAttributes( sal_Int32 nIndex, const Sequence< ::rtl::OUString >& ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	::rtl::OUString sText( implGetText() );
	if ( !implIsValidIndex( nIndex, sText.getLength() ) )
		throw IndexOutOfBoundsException();

	return Sequence< PropertyValue >();
}
// -----------------------------------------------------------------------------
awt::Rectangle SAL_CALL VCLXAccessibleListItem::getCharacterBounds( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	::rtl::OUString sText( implGetText() );
	if ( !implIsValidIndex( nIndex, sText.getLength() ) )
		throw IndexOutOfBoundsException();

	awt::Rectangle aBounds( 0, 0, 0, 0 );
	if ( m_pListBoxHelper )
	{
		Rectangle aCharRect = m_pListBoxHelper->GetEntryCharacterBounds( m_nIndexInParent, nIndex );
		Rectangle aItemRect = m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent );
		aCharRect.Move( -aItemRect.Left(), -aItemRect.Top() );
		aBounds = AWTRectangle( aCharRect );
	}

	return aBounds;
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getCharacterCount() throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getCharacterCount();
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getIndexAtPoint( const awt::Point& aPoint ) throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	sal_Int32 nIndex = -1;
	if ( m_pListBoxHelper )
	{
		sal_uInt16 nPos = LISTBOX_ENTRY_NOTFOUND;
		Rectangle aItemRect = m_pListBoxHelper->GetBoundingRectangle( (sal_uInt16)m_nIndexInParent );
		Point aPnt( VCLPoint( aPoint ) );
		aPnt += aItemRect.TopLeft();
		sal_Int32 nI = m_pListBoxHelper->GetIndexForPoint( aPnt, nPos );
		if ( nI != -1 && (sal_uInt16)m_nIndexInParent == nPos )
			nIndex = nI;
	}
	return nIndex;
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL VCLXAccessibleListItem::getSelectedText() throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getSelectedText();
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getSelectionStart() throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getSelectionStart();
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL VCLXAccessibleListItem::getSelectionEnd() throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getSelectionEnd();
}
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL VCLXAccessibleListItem::setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	if ( !implIsValidRange( nStartIndex, nEndIndex, implGetText().getLength() ) )
		throw IndexOutOfBoundsException();

	return sal_False;
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL VCLXAccessibleListItem::getText() throw (RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getText();
}
// -----------------------------------------------------------------------------
::rtl::OUString SAL_CALL VCLXAccessibleListItem::getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getTextRange( nStartIndex, nEndIndex );
}
// -----------------------------------------------------------------------------
::com::sun::star::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getTextAtIndex( nIndex, aTextType );
}
// -----------------------------------------------------------------------------
::com::sun::star::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getTextBeforeIndex( nIndex, aTextType );
}
// -----------------------------------------------------------------------------
::com::sun::star::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	return OCommonAccessibleText::getTextBehindIndex( nIndex, aTextType );
}
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL VCLXAccessibleListItem::copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
	vos::OGuard aSolarGuard( Application::GetSolarMutex() );
	::osl::MutexGuard aGuard( m_aMutex );

	checkIndex_Impl( nStartIndex, m_sEntryText );
	checkIndex_Impl( nEndIndex, m_sEntryText );

	sal_Bool bRet = sal_False;
	if ( m_pListBoxHelper )
	{
		Reference< datatransfer::clipboard::XClipboard > xClipboard = m_pListBoxHelper->GetClipboard();
		if ( xClipboard.is() )
		{
			::rtl::OUString sText( getTextRange( nStartIndex, nEndIndex ) );
			::vcl::unohelper::TextDataObject* pDataObj = new ::vcl::unohelper::TextDataObject( sText );

			const sal_uInt32 nRef = Application::ReleaseSolarMutex();
			xClipboard->setContents( pDataObj, NULL );
			Reference< datatransfer::clipboard::XFlushableClipboard > xFlushableClipboard( xClipboard, uno::UNO_QUERY );
			if( xFlushableClipboard.is() )
				xFlushableClipboard->flushClipboard();
			Application::AcquireSolarMutex( nRef );

			bRet = sal_True;
		}
	}

	return bRet;
}
// -----------------------------------------------------------------------------
// XAccessibleEventBroadcaster
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::addEventListener( const Reference< XAccessibleEventListener >& xListener ) throw (RuntimeException)
{
	if (xListener.is())
	{
		if (!m_nClientId)
			m_nClientId = comphelper::AccessibleEventNotifier::registerClient( );
		comphelper::AccessibleEventNotifier::addEventListener( m_nClientId, xListener );
	}
}
// -----------------------------------------------------------------------------
void SAL_CALL VCLXAccessibleListItem::removeEventListener( const Reference< XAccessibleEventListener >& xListener ) throw (RuntimeException)
{
	if ( xListener.is() && m_nClientId )
	{
		sal_Int32 nListenerCount = comphelper::AccessibleEventNotifier::removeEventListener( m_nClientId, xListener );
		if ( !nListenerCount )
		{
			// no listeners anymore
			// -> revoke ourself. This may lead to the notifier thread dying (if we were the last client),
			// and at least to us not firing any events anymore, in case somebody calls
			// NotifyAccessibleEvent, again
			if ( m_nClientId )
			{
				comphelper::AccessibleEventNotifier::TClientId nId( m_nClientId );
				m_nClientId = 0;
				comphelper::AccessibleEventNotifier::revokeClient( nId );
			}
		}
	}
}
// -----------------------------------------------------------------------------



// AF (Oct. 29 2002): Return black as constant foreground color. This is an
// initial implementation and has to be substituted by code that determines
// the color that is actually used.
sal_Int32 SAL_CALL VCLXAccessibleListItem::getForeground (void)
	throw (::com::sun::star::uno::RuntimeException)
{
	return COL_BLACK;
}

// AF (Oct. 29 2002): Return white as constant background color. This is an
// initial implementation and has to be substituted by code that determines
// the color that is actually used.
sal_Int32 SAL_CALL VCLXAccessibleListItem::getBackground (void)
	throw (::com::sun::star::uno::RuntimeException)
{
	return COL_WHITE;
}
// -----------------------------------------------------------------------------
