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



#ifndef PDFDIALOG_HXX
#define PDFDIALOG_HXX

#include "pdffilter.hxx"
#include <svtools/genericunodialog.hxx>

#include <cppuhelper/implbase2.hxx>

// -------------
// - PDFDialog -
// -------------

class Window;
class ResMgr;

typedef ::svt::OGenericUnoDialog             PDFDialog_DialogBase;
typedef ::cppu::ImplInheritanceHelper2  <
                                             PDFDialog_DialogBase,
                                             XPropertyAccess,
                                             XExporter
                                        >    PDFDialog_Base;

class PDFDialog : public PDFDialog_Base,
                  public ::comphelper::OPropertyArrayUsageHelper< PDFDialog >
{
private:
    Sequence< PropertyValue >   maMediaDescriptor;
    Sequence< PropertyValue >   maFilterData;
    Reference< XComponent >     mxSrcDoc;

protected:
	// OGenericUnoDialog
    virtual Sequence< sal_Int8 > SAL_CALL getImplementationId();
    virtual OUString SAL_CALL getImplementationName();
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames();
    virtual Dialog*	createDialog( Window* pParent );
    virtual void executedDialog( sal_Int16 nExecutionResult );
	virtual Reference< XPropertySetInfo>  SAL_CALL getPropertySetInfo();
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
	virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;

    // XPropertyAccess
    using OPropertySetHelper::getPropertyValues;
    virtual Sequence< PropertyValue > SAL_CALL getPropertyValues(  );
    using OPropertySetHelper::setPropertyValues;
    virtual void SAL_CALL setPropertyValues( const Sequence< PropertyValue >& aProps );

   	// XExporter
    virtual void SAL_CALL setSourceDocument( const Reference< XComponent >& xDoc );

public:

				PDFDialog( const Reference< XMultiServiceFactory >& rxMSF );
	virtual		~PDFDialog();
};

// -----------------------------------------------------------------------------

OUString PDFDialog_getImplementationName ();
Sequence< sal_Int8 > SAL_CALL PDFDialog_getImplementationId();
Sequence< OUString > SAL_CALL PDFDialog_getSupportedServiceNames();
Reference< XInterface > SAL_CALL PDFDialog_createInstance( const Reference< XMultiServiceFactory > & rSMgr);

#endif // PDFDIALOG_HXX
