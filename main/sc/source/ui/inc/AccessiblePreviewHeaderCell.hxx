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




#ifndef _SC_ACCESSIBLEPREVIEWHEADERCELL_HXX
#define _SC_ACCESSIBLEPREVIEWHEADERCELL_HXX

#include "AccessibleContextBase.hxx"
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include <tools/gen.hxx>
#include "global.hxx"
#include "address.hxx"
#include <cppuhelper/implbase1.hxx>

class ScPreviewShell;
class ScPreviewTableInfo;
namespace accessibility {
    class AccessibleTextHelper;
}

typedef cppu::ImplHelper1< ::com::sun::star::accessibility::XAccessibleValue>
					ScAccessiblePreviewHeaderCellImpl;

class ScAccessiblePreviewHeaderCell :
		public ScAccessibleContextBase,
		public ScAccessiblePreviewHeaderCellImpl
{
public:
	ScAccessiblePreviewHeaderCell( const ::com::sun::star::uno::Reference<
						        ::com::sun::star::accessibility::XAccessible>& rxParent,
							ScPreviewShell* pViewShell,
							const ScAddress& rCellPos, sal_Bool bIsColHdr, sal_Bool bIsRowHdr,
							sal_Int32 nIndex );

protected:
	virtual ~ScAccessiblePreviewHeaderCell();

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

	//=====  XAccessibleValue  ================================================

    virtual ::com::sun::star::uno::Any SAL_CALL getCurrentValue();
    virtual sal_Bool SAL_CALL setCurrentValue( const ::com::sun::star::uno::Any& aNumber );
    virtual ::com::sun::star::uno::Any SAL_CALL getMaximumValue();
    virtual ::com::sun::star::uno::Any SAL_CALL getMinimumValue();

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

	///=====  XTypeProvider  ===================================================

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL
		getTypes();

	/**	Returns a implementation id.
    */
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId(void);

protected:
	virtual ::rtl::OUString SAL_CALL createAccessibleDescription(void);
	virtual ::rtl::OUString SAL_CALL createAccessibleName(void);

	virtual Rectangle GetBoundingBoxOnScreen(void) const;
	virtual Rectangle GetBoundingBox(void) const;

private:
	ScPreviewShell*		mpViewShell;
	accessibility::AccessibleTextHelper* mpTextHelper;
	sal_Int32			mnIndex;
	ScAddress			maCellPos;
	sal_Bool			mbColumnHeader;
	sal_Bool			mbRowHeader;
	mutable ScPreviewTableInfo*	mpTableInfo;

	sal_Bool IsDefunc(
		const com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessibleStateSet>& rxParentStates);

	void CreateTextHelper();
	void	FillTableInfo() const;
};


#endif
