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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_filter.hxx"

#include "pdfdialog.hxx"
#include "impdialog.hxx"
#include <vcl/svapp.hxx>
#include <vcl/dialog.hxx>
#include <svl/solar.hrc>
#include <com/sun/star/view/XRenderable.hpp>

using namespace ::rtl;
using namespace ::vcl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;

// -----------------------
// - PDFDialog functions -
// -----------------------

#define SERVICE_NAME "com.sun.star.document.PDFDialog"

// -----------------------------------------------------------------------------

OUString PDFDialog_getImplementationName ()
{
	return OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.comp.PDF.PDFDialog" ) );
}

// -----------------------------------------------------------------------------

Sequence< sal_Int8 > SAL_CALL PDFDialog_getImplementationId()
{
    static ::cppu::OImplementationId aId;
    return aId.getImplementationId();
}

// -----------------------------------------------------------------------------

Sequence< OUString > SAL_CALL PDFDialog_getSupportedServiceNames()
{
	Sequence < OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] =  OUString ( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
    return aRet;
}

// -----------------------------------------------------------------------------

Reference< XInterface > SAL_CALL PDFDialog_createInstance( const Reference< XMultiServiceFactory > & rSMgr)
{
	return (cppu::OWeakObject*) new PDFDialog( rSMgr );
}

// -----------------------------------------------------------------------------

#undef SERVICE_NAME

// -------------
// - PDFDialog -
// -------------

PDFDialog::PDFDialog( const Reference< XMultiServiceFactory > &rxMSF )
: PDFDialog_Base( rxMSF )
{
}

// -----------------------------------------------------------------------------

PDFDialog::~PDFDialog()
{
}

// -----------------------------------------------------------------------------

Sequence< sal_Int8 > SAL_CALL PDFDialog::getImplementationId()
{
    return PDFDialog_getImplementationId();
}

// -----------------------------------------------------------------------------

OUString SAL_CALL PDFDialog::getImplementationName()
{
	return PDFDialog_getImplementationName();
}

// -----------------------------------------------------------------------------

Sequence< OUString > SAL_CALL PDFDialog::getSupportedServiceNames()
{
    return PDFDialog_getSupportedServiceNames();
}

// -----------------------------------------------------------------------------

Dialog*	PDFDialog::createDialog( Window* pParent )
{
    Dialog* pRet = NULL;

    if( mxSrcDoc.is() )
    {
        ImpPDFTabDialog* pDlg = new ImpPDFTabDialog( pParent, maFilterData, mxSrcDoc, m_aContext.getLegacyServiceFactory() );
        pRet = pDlg;
    }

    return pRet;
}

// -----------------------------------------------------------------------------

void PDFDialog::executedDialog( sal_Int16 nExecutionResult )
{
    if( nExecutionResult && m_pDialog )
        maFilterData = static_cast< ImpPDFTabDialog* >( m_pDialog )->GetFilterData();
    destroyDialog();
}

// -----------------------------------------------------------------------------

Reference< XPropertySetInfo > SAL_CALL PDFDialog::getPropertySetInfo()
{
	Reference< XPropertySetInfo >  xInfo( createPropertySetInfo( getInfoHelper() ) );
	return xInfo;
}

//-------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& PDFDialog::getInfoHelper()
{
    return *const_cast<PDFDialog*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper* PDFDialog::createArrayHelper() const
{
	Sequence< Property > aProps;
	describeProperties(aProps);
	return new ::cppu::OPropertyArrayHelper( aProps );
}

// -----------------------------------------------------------------------------

Sequence< PropertyValue > SAL_CALL PDFDialog::getPropertyValues()
{
    sal_Int32 i, nCount;

    for( i = 0, nCount = maMediaDescriptor.getLength(); i < nCount; i++ )
	{
		if( maMediaDescriptor[ i ].Name.equalsAscii( "FilterData" ) )
			break;
	}

    if( i == nCount )
		maMediaDescriptor.realloc( ++nCount );

	maMediaDescriptor[ i ].Name = String( RTL_CONSTASCII_USTRINGPARAM( "FilterData" ) );
	maMediaDescriptor[ i ].Value <<= maFilterData;

    return maMediaDescriptor;
}

// -----------------------------------------------------------------------------

void SAL_CALL PDFDialog::setPropertyValues( const Sequence< PropertyValue >& rProps )
{
    maMediaDescriptor = rProps;

	for( sal_Int32 i = 0, nCount = maMediaDescriptor.getLength(); i < nCount; i++ )
	{
		if( maMediaDescriptor[ i ].Name.equalsAscii( "FilterData" ) )
		{
			maMediaDescriptor[ i ].Value >>= maFilterData;
			break;
		}
	}
}

// -----------------------------------------------------------------------------

void SAL_CALL PDFDialog::setSourceDocument( const Reference< XComponent >& xDoc )
{
    mxSrcDoc = xDoc;
}
