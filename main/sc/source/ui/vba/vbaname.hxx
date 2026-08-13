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


#ifndef SC_VBA_NAME_HXX
#define SC_VBA_NAME_HXX

#include <ooo/vba/excel/XName.hpp>
#include <com/sun/star/sheet/XNamedRange.hpp>
#include <com/sun/star/sheet/XNamedRanges.hpp>

#include <vbahelper/vbahelperinterface.hxx>

class ScDocument;

typedef InheritedHelperInterfaceImpl1< ov::excel::XName > NameImpl_BASE;

class ScVbaName : public NameImpl_BASE
{
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference< css::sheet::XNamedRange > mxNamedRange;
	css::uno::Reference< css::sheet::XNamedRanges > mxNames;

	ScDocument * m_pDoc;

protected:
	virtual css::uno::Reference< css::frame::XModel >  getModel() { return mxModel; }
	virtual css::uno::Reference< ov::excel::XWorksheet > getWorkSheet();

public:
	ScVbaName( const css::uno::Reference< ov::XHelperInterface >& xParent,  const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::sheet::XNamedRange >& xName , const css::uno::Reference< css::sheet::XNamedRanges >& xNames , const css::uno::Reference< css::frame::XModel >& xModel );
	virtual ~ScVbaName();

	// Attributes
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL setName( const ::rtl::OUString &rName );
	virtual ::rtl::OUString SAL_CALL getNameLocal();
	virtual void SAL_CALL setNameLocal( const ::rtl::OUString &rName );
	virtual sal_Bool SAL_CALL getVisible();
	virtual void SAL_CALL setVisible( sal_Bool bVisible );
	virtual ::rtl::OUString SAL_CALL getValue();
	virtual void SAL_CALL setValue( const ::rtl::OUString &rValue );
	virtual ::rtl::OUString SAL_CALL getRefersTo();
	virtual void SAL_CALL setRefersTo( const ::rtl::OUString &rRefersTo );
	virtual ::rtl::OUString SAL_CALL getRefersToLocal();
	virtual void SAL_CALL setRefersToLocal( const ::rtl::OUString &rRefersTo );
	virtual ::rtl::OUString SAL_CALL getRefersToR1C1();
	virtual void SAL_CALL setRefersToR1C1( const ::rtl::OUString &rRefersTo );
	virtual ::rtl::OUString SAL_CALL getRefersToR1C1Local();
	virtual void SAL_CALL setRefersToR1C1Local( const ::rtl::OUString &rRefersTo );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getRefersToRange();
	virtual void SAL_CALL setRefersToRange( const css::uno::Reference< ov::excel::XRange > xRange );

	// Methods
	virtual void SAL_CALL Delete();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SC_VBA_NAME_HXX */
