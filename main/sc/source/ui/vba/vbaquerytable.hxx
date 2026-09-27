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

#ifndef SC_VBA_QUERYTABLE_HXX
#define SC_VBA_QUERYTABLE_HXX

#include <ooo/vba/excel/XQueryTable.hpp>
#include <vbahelper/vbahelperinterface.hxx>

namespace css = ::com::sun::star;

typedef InheritedHelperInterfaceImpl1< ooo::vba::excel::XQueryTable > ScVbaQueryTableImpl_BASE;

class ScDocument;
class ScVbaRange;

class ScVbaQueryTable : public ScVbaQueryTableImpl_BASE
{
private:
    ScDocument* m_pDocument;
    ScVbaRange* m_pParent;
public:
    ScVbaQueryTable( const css::uno::Reference< ov::XHelperInterface >& xParent,
            const css::uno::Reference< css::uno::XComponentContext >& xContext,
            ScDocument* pDocument = NULL,
            ScVbaRange* pParent = NULL );
    virtual ~ScVbaQueryTable();

    virtual sal_Bool SAL_CALL Refresh( const css::uno::Any& aBackgroundQuery )
        throw ( css::uno::RuntimeException );
    virtual void SAL_CALL Delete() throw ( css::uno::RuntimeException );

    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};

#endif