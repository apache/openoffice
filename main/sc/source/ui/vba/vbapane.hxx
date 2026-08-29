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


#ifndef SC_VBA_PANE_HXX
#define SC_VBA_PANE_HXX

#include <com/sun/star/sheet/XViewPane.hpp>
#include <ooo/vba/excel/XPane.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include "excelvbahelper.hxx"

typedef InheritedHelperInterfaceImpl1< ov::excel::XPane > ScVbaPane_BASE;

class ScVbaPane : public ScVbaPane_BASE
{
public:
    ScVbaPane(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::frame::XModel >& xModel,
        const css::uno::Reference< css::sheet::XViewPane > xViewPane );

    css::uno::Reference< css::sheet::XViewPane > getViewPane() { return m_xViewPane; }

    // XPane attributes
    virtual sal_Int32 SAL_CALL getScrollColumn();
    virtual void SAL_CALL setScrollColumn( sal_Int32 _scrollcolumn );
    virtual sal_Int32 SAL_CALL getScrollRow();
    virtual void SAL_CALL setScrollRow( sal_Int32 _scrollrow );
 	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getVisibleRange();

    // XPane methods
    virtual void SAL_CALL SmallScroll( const css::uno::Any& Down, const css::uno::Any& Up, const css::uno::Any& ToRight, const css::uno::Any& ToLeft );
    virtual void SAL_CALL LargeScroll( const css::uno::Any& Down, const css::uno::Any& Up, const css::uno::Any& ToRight, const css::uno::Any& ToLeft );

	// XHelperInterface
    VBAHELPER_DECL_XHELPERINTERFACE

protected:
	css::uno::Reference< css::frame::XModel > m_xModel;
    css::uno::Reference< css::sheet::XViewPane > m_xViewPane;
};

#endif //SC_VBA_PANE_HXX
