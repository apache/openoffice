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
#include "precompiled_comphelper.hxx"
#include <comphelper/accessiblecomponenthelper.hxx>

//.........................................................................
namespace comphelper
{
//.........................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::awt;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::accessibility;

	//=====================================================================
	//= OCommonAccessibleComponent
	//=====================================================================
	//---------------------------------------------------------------------
	OCommonAccessibleComponent::OCommonAccessibleComponent( )
	{
	}

	//---------------------------------------------------------------------
	OCommonAccessibleComponent::OCommonAccessibleComponent( IMutex* _pExternalLock )
		:OAccessibleContextHelper( _pExternalLock )
	{
	}

	//---------------------------------------------------------------------
	OCommonAccessibleComponent::~OCommonAccessibleComponent( )
	{
		forgetExternalLock();
			// this ensures that the lock, which may be already destroyed as part of the derivee,
			// is not used anymore
	}

	//--------------------------------------------------------------------
	sal_Bool SAL_CALL OCommonAccessibleComponent::containsPoint( const Point& _rPoint )
	{
		OExternalLockGuard aGuard( this );
		Rectangle aBounds( implGetBounds() );
		return	( _rPoint.X >= 0 )
			&&	( _rPoint.Y >= 0 )
			&&	( _rPoint.X < aBounds.Width )
			&&	( _rPoint.Y < aBounds.Height );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OCommonAccessibleComponent::getLocation(  )
	{
		OExternalLockGuard aGuard( this );
		Rectangle aBounds( implGetBounds() );
		return Point( aBounds.X, aBounds.Y );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OCommonAccessibleComponent::getLocationOnScreen(  )
	{
		OExternalLockGuard aGuard( this );
		Rectangle aBounds( implGetBounds() );

		Point aScreenLoc( 0, 0 );

		Reference< XAccessibleComponent > xParentComponent( implGetParentContext(), UNO_QUERY );
		OSL_ENSURE( xParentComponent.is(), "OCommonAccessibleComponent::getLocationOnScreen: no parent component!" );
		if ( xParentComponent.is() )
		{
			Point aParentScreenLoc( xParentComponent->getLocationOnScreen() );
			Point aOwnRelativeLoc( getLocation() );
			aScreenLoc.X = aParentScreenLoc.X + aOwnRelativeLoc.X;
			aScreenLoc.Y = aParentScreenLoc.Y + aOwnRelativeLoc.Y;
		}

		return aScreenLoc;
	}

	//--------------------------------------------------------------------
	Size SAL_CALL OCommonAccessibleComponent::getSize(  )
	{
		OExternalLockGuard aGuard( this );
		Rectangle aBounds( implGetBounds() );
		return Size( aBounds.Width, aBounds.Height );
	}

	//--------------------------------------------------------------------
	Rectangle SAL_CALL OCommonAccessibleComponent::getBounds(  )
	{
		OExternalLockGuard aGuard( this );
		return implGetBounds();
	}

	//=====================================================================
	//= OAccessibleComponentHelper
	//=====================================================================
	//---------------------------------------------------------------------
	OAccessibleComponentHelper::OAccessibleComponentHelper( )
	{
	}

	//---------------------------------------------------------------------
	OAccessibleComponentHelper::OAccessibleComponentHelper( IMutex* _pExternalLock )
		:OCommonAccessibleComponent( _pExternalLock )
	{
	}

	//--------------------------------------------------------------------
	IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
	IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
		// (order matters: the first is the class name, the second is the class doing the ref counting)

	//--------------------------------------------------------------------
	sal_Bool SAL_CALL OAccessibleComponentHelper::containsPoint( const Point& _rPoint )
	{
		return OCommonAccessibleComponent::containsPoint( _rPoint );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OAccessibleComponentHelper::getLocation(  )
	{
		return OCommonAccessibleComponent::getLocation( );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OAccessibleComponentHelper::getLocationOnScreen(  )
	{
		return OCommonAccessibleComponent::getLocationOnScreen( );
	}

	//--------------------------------------------------------------------
	Size SAL_CALL OAccessibleComponentHelper::getSize(  )
	{
		return OCommonAccessibleComponent::getSize( );
	}

	//--------------------------------------------------------------------
	Rectangle SAL_CALL OAccessibleComponentHelper::getBounds(  )
	{
		return OCommonAccessibleComponent::getBounds( );
	}

	//=====================================================================
	//= OAccessibleExtendedComponentHelper
	//=====================================================================
	//---------------------------------------------------------------------
	OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( )
	{
	}

	//---------------------------------------------------------------------
	OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( IMutex* _pExternalLock )
		:OCommonAccessibleComponent( _pExternalLock )
	{
	}

	//--------------------------------------------------------------------
	IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
	IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
		// (order matters: the first is the class name, the second is the class doing the ref counting)

	//--------------------------------------------------------------------
	sal_Bool SAL_CALL OAccessibleExtendedComponentHelper::containsPoint( const Point& _rPoint )
	{
		return OCommonAccessibleComponent::containsPoint( _rPoint );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OAccessibleExtendedComponentHelper::getLocation(  )
	{
		return OCommonAccessibleComponent::getLocation( );
	}

	//--------------------------------------------------------------------
	Point SAL_CALL OAccessibleExtendedComponentHelper::getLocationOnScreen(  )
	{
		return OCommonAccessibleComponent::getLocationOnScreen( );
	}

	//--------------------------------------------------------------------
	Size SAL_CALL OAccessibleExtendedComponentHelper::getSize(  )
	{
		return OCommonAccessibleComponent::getSize( );
	}

	//--------------------------------------------------------------------
	Rectangle SAL_CALL OAccessibleExtendedComponentHelper::getBounds(  )
	{
		return OCommonAccessibleComponent::getBounds( );
	}

//.........................................................................
}	// namespace comphelper
//.........................................................................
