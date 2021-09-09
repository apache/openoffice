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
#include "accessibility/extended/accessiblebrowseboxcell.hxx"
#include <svtools/accessibletableprovider.hxx>

// .................................................................................
namespace accessibility
{
// .................................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::awt;
	using namespace ::com::sun::star::accessibility;
	using namespace ::svt;

	// =============================================================================
	// = AccessibleBrowseBoxCell
	// =============================================================================
	DBG_NAME( svt_AccessibleBrowseBoxCell )
	// -----------------------------------------------------------------------------
	AccessibleBrowseBoxCell::AccessibleBrowseBoxCell(
			const Reference< XAccessible >& _rxParent, IAccessibleTableProvider& _rBrowseBox,
			const Reference< XWindow >& _xFocusWindow,
			sal_Int32 _nRowPos, sal_uInt16 _nColPos, AccessibleBrowseBoxObjType _eType )
		:AccessibleBrowseBoxBase( _rxParent, _rBrowseBox, _xFocusWindow, _eType )
		,m_nRowPos( _nRowPos )
		,m_nColPos( _nColPos )
	{
		DBG_CTOR( svt_AccessibleBrowseBoxCell, NULL );
		// set accessible name here, because for that we need the position of the cell
		// and so the base class isn't capable of doing this
		sal_Int32 nPos = _nRowPos * _rBrowseBox.GetColumnCount() + _nColPos;
		::rtl::OUString aAccName = _rBrowseBox.GetAccessibleObjectName( BBTYPE_TABLECELL, nPos );
		implSetName( aAccName );
	}

	// -----------------------------------------------------------------------------
	AccessibleBrowseBoxCell::~AccessibleBrowseBoxCell()
	{
		DBG_DTOR( svt_AccessibleBrowseBoxCell, NULL );
	}

	// -----------------------------------------------------------------------------
	void SAL_CALL AccessibleBrowseBoxCell::grabFocus() throw ( RuntimeException )
	{
		SolarMethodGuard aGuard( *this );
		mpBrowseBox->GoToCell( m_nRowPos, m_nColPos );
	}
	// -----------------------------------------------------------------------------
	::Rectangle AccessibleBrowseBoxCell::implGetBoundingBox()
	{
		return mpBrowseBox->GetFieldRectPixelAbs( m_nRowPos, m_nColPos, sal_False, sal_False );
	}

	// -----------------------------------------------------------------------------
	::Rectangle AccessibleBrowseBoxCell::implGetBoundingBoxOnScreen()
	{
		return mpBrowseBox->GetFieldRectPixelAbs( m_nRowPos, m_nColPos, sal_False );
	}

// .................................................................................
}	// namespace accessibility
// .................................................................................
