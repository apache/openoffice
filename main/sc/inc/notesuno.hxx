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



#ifndef SC_NOTESUNO_HXX
#define SC_NOTESUNO_HXX

#include <com/sun/star/sheet/XSheetAnnotation.hpp>
#include <com/sun/star/sheet/XSheetAnnotationShapeSupplier.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/text/XSimpleText.hpp>
#include <cppuhelper/implbase5.hxx>
#include <svl/lstner.hxx>
#include "address.hxx"

class ScDocShell;
class SvxUnoText;
class ScPostIt;


class ScAnnotationObj : public cppu::WeakImplHelper5<
							com::sun::star::container::XChild,
							com::sun::star::text::XSimpleText,
							com::sun::star::sheet::XSheetAnnotation,
                            com::sun::star::sheet::XSheetAnnotationShapeSupplier,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
public:
							ScAnnotationObj(ScDocShell* pDocSh, const ScAddress& rPos);
	virtual					~ScAnnotationObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XChild
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
							getParent();
	virtual void SAL_CALL	setParent( const ::com::sun::star::uno::Reference<
										::com::sun::star::uno::XInterface >& Parent );

                            // XSimpleText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursorByRange( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& aTextPosition );
	virtual void SAL_CALL	insertString( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										const ::rtl::OUString& aString, sal_Bool bAbsorb );
	virtual void SAL_CALL	insertControlCharacter( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										sal_Int16 nControlCharacter, sal_Bool bAbsorb );

							// XTextRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > SAL_CALL
							getText();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getStart();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getEnd();
	virtual ::rtl::OUString SAL_CALL getString();
	virtual void SAL_CALL	setString( const ::rtl::OUString& aString );

							// XSheetAnnotation
	virtual ::com::sun::star::table::CellAddress SAL_CALL getPosition();
	virtual ::rtl::OUString SAL_CALL getAuthor();
	virtual ::rtl::OUString SAL_CALL getDate();
	virtual sal_Bool SAL_CALL getIsVisible();
	virtual void SAL_CALL setIsVisible( sal_Bool bIsVisible );

                            // XSheetAnnotationShapeSupplier
    virtual ::com::sun::star::uno::Reference < ::com::sun::star::drawing::XShape > SAL_CALL
                            getAnnotationShape();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
	SvxUnoText&         GetUnoText();

    const ScPostIt*     ImplGetNote() const;

private:
	ScDocShell*				pDocShell;
	ScAddress				aCellPos;
	SvxUnoText*				pUnoText;
};

#endif
