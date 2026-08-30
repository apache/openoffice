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


#ifndef SW_VBA_BOOKMARK_HXX
#define SW_VBA_BOOKMARK_HXX

#include <ooo/vba/word/XBookmark.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XBookmarksSupplier.hpp>
#include <com/sun/star/text/XTextContent.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XBookmark > SwVbaBookmark_BASE;

class SwVbaBookmark : public SwVbaBookmark_BASE
{
private:
    css::uno::Reference< css::frame::XModel > mxModel;
    css::uno::Reference< css::text::XTextContent > mxBookmark;
    rtl::OUString maName;
    sal_Bool mbValid;

private:
    void checkVality();

public:
	SwVbaBookmark( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext,
        const css::uno::Reference< css::frame::XModel >& rModel, const rtl::OUString& rName );
	virtual ~SwVbaBookmark();

   // Methods
    virtual rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const rtl::OUString& );
    virtual void SAL_CALL Delete();
    virtual void SAL_CALL Select();
    virtual css::uno::Any SAL_CALL Range();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_BOOKMARK_HXX */
