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


#ifndef VBA_DOCUMENTBASE_HXX
#define VBA_DOCUMENTBASE_HXX

#include <com/sun/star/frame/XModel.hpp>
#include <ooo/vba/XDocumentBase.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ooo::vba::XDocumentBase > VbaDocumentBase_BASE;

class VBAHELPER_DLLPUBLIC VbaDocumentBase : public VbaDocumentBase_BASE
{
protected:
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference< css::uno::XInterface > mxVBProject;
protected:
	virtual css::uno::Reference< css::frame::XModel >  getModel() { return mxModel; }
	VbaDocumentBase( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext);
public:
	VbaDocumentBase( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,
			css::uno::Reference< css::frame::XModel > xModel );
	VbaDocumentBase( 	css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );
	virtual ~VbaDocumentBase() {}

    // Attributes
	virtual ::rtl::OUString SAL_CALL getName();
	virtual ::rtl::OUString SAL_CALL getPath();
	virtual ::rtl::OUString SAL_CALL getFullName();
	virtual sal_Bool SAL_CALL getSaved();
	virtual void SAL_CALL setSaved( sal_Bool bSave );
	virtual css::uno::Any SAL_CALL getVBProject();

	// Methods
	virtual void SAL_CALL Close( const css::uno::Any &bSaveChanges,
								 const css::uno::Any &aFileName,
								 const css::uno::Any &bRouteWorkbook );
	virtual void SAL_CALL Protect( const css::uno::Any & aPassword );
	virtual void SAL_CALL Unprotect( const css::uno::Any &aPassword );
	virtual void SAL_CALL Save();
	virtual void SAL_CALL Activate();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* VBA_DOCUMENTBASE_HXX */
