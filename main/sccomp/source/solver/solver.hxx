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



#ifndef SOLVER_HXX
#define SOLVER_HXX

#include <com/sun/star/sheet/XSolver.hpp>
#include <com/sun/star/sheet/XSolverDescription.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/implbase3.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/propertycontainer.hxx>
#include <comphelper/proparrhlp.hxx>

typedef cppu::WeakImplHelper3<
                com::sun::star::sheet::XSolver,
                com::sun::star::sheet::XSolverDescription,
                com::sun::star::lang::XServiceInfo >
        SolverComponent_Base;

class SolverComponent : public comphelper::OMutexAndBroadcastHelper,
                        public comphelper::OPropertyContainer,
                        public comphelper::OPropertyArrayUsageHelper< SolverComponent >,
                        public SolverComponent_Base
{
    // settings
    com::sun::star::uno::Reference< com::sun::star::sheet::XSpreadsheetDocument > mxDoc;
    com::sun::star::table::CellAddress                                            maObjective;
    com::sun::star::uno::Sequence< com::sun::star::table::CellAddress >           maVariables;
    com::sun::star::uno::Sequence< com::sun::star::sheet::SolverConstraint >      maConstraints;
    sal_Bool                                                                      mbMaximize;
    // set via XPropertySet
    sal_Bool                                                                      mbNonNegative;
    sal_Bool                                                                      mbInteger;
    sal_Int32                                                                     mnTimeout;
    sal_Int32                                                                     mnEpsilonLevel;
    sal_Bool                                                                      mbLimitBBDepth;
    sal_Bool                                                                      mbNonLinearTest;
    // results
    sal_Bool                                                                      mbSuccess;
    double                                                                        mfResultValue;
    com::sun::star::uno::Sequence< double >                                       maSolution;
    rtl::OUString                                                                 maStatus;

public:
                            SolverComponent( const com::sun::star::uno::Reference<
                                    com::sun::star::uno::XComponentContext >& rxMSF );
    virtual                 ~SolverComponent();

    DECLARE_XINTERFACE()
    DECLARE_XTYPEPROVIDER()

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();     // from OPropertySetHelper
    virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;    // from OPropertyArrayUsageHelper

                            // XSolver
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheetDocument > SAL_CALL getDocument();
    virtual void SAL_CALL   setDocument( const ::com::sun::star::uno::Reference<
                                    ::com::sun::star::sheet::XSpreadsheetDocument >& _document );
    virtual ::com::sun::star::table::CellAddress SAL_CALL getObjective();
    virtual void SAL_CALL   setObjective( const ::com::sun::star::table::CellAddress& _objective );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellAddress > SAL_CALL getVariables();
    virtual void SAL_CALL   setVariables( const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::table::CellAddress >& _variables );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::SolverConstraint > SAL_CALL getConstraints();
    virtual void SAL_CALL   setConstraints( const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::sheet::SolverConstraint >& _constraints );
    virtual ::sal_Bool SAL_CALL getMaximize();
    virtual void SAL_CALL   setMaximize( ::sal_Bool _maximize );

    virtual ::sal_Bool SAL_CALL getSuccess();
    virtual double SAL_CALL getResultValue();
    virtual ::com::sun::star::uno::Sequence< double > SAL_CALL getSolution();

    virtual void SAL_CALL solve();

                            // XSolverDescription
    virtual ::rtl::OUString SAL_CALL getComponentDescription();
    virtual ::rtl::OUString SAL_CALL getStatusDescription();
    virtual ::rtl::OUString SAL_CALL getPropertyDescription( const ::rtl::OUString& aPropertyName );

                            // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

#endif
