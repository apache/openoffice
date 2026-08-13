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



#ifndef SC_EVENTUNO_HXX
#define SC_EVENTUNO_HXX

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <cppuhelper/implbase2.hxx>
#include <svl/lstner.hxx>

#include "global.hxx"

class ScDocShell;

class ScSheetEventsObj : public cppu::WeakImplHelper2<
                                        com::sun::star::container::XNameReplace,
                                        com::sun::star::lang::XServiceInfo>,
                                    public SfxListener
{
private:
    ScDocShell*             mpDocShell;
    SCTAB                   mnTab;

public:
                            ScSheetEventsObj(ScDocShell* pDocSh, SCTAB nT);
    virtual                 ~ScSheetEventsObj();

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

                            // XNameReplace
    virtual void SAL_CALL   replaceByName( const ::rtl::OUString& aName,
                                const ::com::sun::star::uno::Any& aElement );

                            // XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

                            // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

                            // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

#endif
