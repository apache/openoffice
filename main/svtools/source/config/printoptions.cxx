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
#include "precompiled_svtools.hxx"

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <svtools/printoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <vcl/print.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif

#ifndef _SVT_LOGHELPER_HXX
#include <unotools/loghelper.hxx>
#endif

#include <itemholder2.hxx>


// -----------
// - statics -
// -----------

static sal_uInt16 aDPIArray[] = { 72, 96, 150, 200, 300, 600 };

#define DPI_COUNT (sizeof(aDPIArray)/sizeof(aDPIArray[0 ]))

// -----------
// - Defines -
// -----------

#define	ROOTNODE_START			   		                OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Print/Option"))
#define ROOTNODE_PRINTOPTION                            OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Office.Common/Print/Option"))

#define	PROPERTYNAME_REDUCETRANSPARENCY	                OUString(RTL_CONSTASCII_USTRINGPARAM("ReduceTransparency"))
#define	PROPERTYNAME_REDUCEDTRANSPARENCYMODE	        OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedTransparencyMode"))
#define	PROPERTYNAME_REDUCEGRADIENTS		            OUString(RTL_CONSTASCII_USTRINGPARAM("ReduceGradients"))
#define	PROPERTYNAME_REDUCEDGRADIENTMODE		        OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedGradientMode"))
#define PROPERTYNAME_REDUCEDGRADIENTSTEPCOUNT           OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedGradientStepCount"))
#define PROPERTYNAME_REDUCEBITMAPS                      OUString(RTL_CONSTASCII_USTRINGPARAM("ReduceBitmaps"))
#define PROPERTYNAME_REDUCEDBITMAPMODE                  OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedBitmapMode"))
#define PROPERTYNAME_REDUCEDBITMAPRESOLUTION            OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedBitmapResolution"))
#define PROPERTYNAME_REDUCEDBITMAPINCLUDESTRANSPARENCY  OUString(RTL_CONSTASCII_USTRINGPARAM("ReducedBitmapIncludesTransparency"))
#define PROPERTYNAME_CONVERTTOGREYSCALES                OUString(RTL_CONSTASCII_USTRINGPARAM("ConvertToGreyscales"))

// --------------
// - Namespaces -
// --------------

using namespace ::utl;
using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star::uno;
namespace css = com::sun::star;

// -----------
// - statics -
// -----------

static SvtPrintOptions_Impl*   pPrinterOptionsDataContainer = NULL;
static SvtPrintOptions_Impl*   pPrintFileOptionsDataContainer = NULL;

SvtPrintOptions_Impl*   SvtPrinterOptions::m_pStaticDataContainer = NULL;
sal_Int32				SvtPrinterOptions::m_nRefCount = 0;

SvtPrintOptions_Impl*   SvtPrintFileOptions::m_pStaticDataContainer = NULL;
sal_Int32				SvtPrintFileOptions::m_nRefCount = 0;

// ------------------------
// - SvtPrintOptions_Impl -
// ------------------------

class SvtPrintOptions_Impl
{
public:

//---------------------------------------------------------------------------------------------------------
//	constructor / destructor
//---------------------------------------------------------------------------------------------------------

	 SvtPrintOptions_Impl( const OUString& rConfigRoot );
	~SvtPrintOptions_Impl();

//---------------------------------------------------------------------------------------------------------
//	public interface
//---------------------------------------------------------------------------------------------------------

	sal_Bool	IsReduceTransparency() const ;
	sal_Int16   GetReducedTransparencyMode() const ;
	sal_Bool	IsReduceGradients() const ;
	sal_Int16   GetReducedGradientMode() const ;
	sal_Int16   GetReducedGradientStepCount() const ;
	sal_Bool	IsReduceBitmaps() const ;
	sal_Int16   GetReducedBitmapMode() const ;
	sal_Int16   GetReducedBitmapResolution() const ;
	sal_Bool	IsReducedBitmapIncludesTransparency() const ;
   	sal_Bool	IsConvertToGreyscales() const;

	void        SetReduceTransparency( sal_Bool	bState ) ;
	void        SetReducedTransparencyMode( sal_Int16 nMode ) ;
	void        SetReduceGradients( sal_Bool bState ) ;
	void        SetReducedGradientMode( sal_Int16 nMode ) ;
	void        SetReducedGradientStepCount( sal_Int16 nStepCount ) ;
	void        SetReduceBitmaps( sal_Bool bState ) ;
	void        SetReducedBitmapMode( sal_Int16 nMode ) ;
	void        SetReducedBitmapResolution( sal_Int16 nResolution ) ;
	void        SetReducedBitmapIncludesTransparency( sal_Bool bState ) ;
   	void        SetConvertToGreyscales( sal_Bool bState ) ;

//-------------------------------------------------------------------------------------------------------------
//	private API
//-------------------------------------------------------------------------------------------------------------

private:
    void impl_setValue (const ::rtl::OUString& sProp,
                              ::sal_Bool       bNew );
    void impl_setValue (const ::rtl::OUString& sProp,
                              ::sal_Int16      nNew );

//-------------------------------------------------------------------------------------------------------------
//	private member
//-------------------------------------------------------------------------------------------------------------

private:
   	css::uno::Reference< css::container::XNameAccess > m_xCfg;
	css::uno::Reference< css::container::XNameAccess > m_xNode;
};

SvtPrintOptions_Impl::SvtPrintOptions_Impl(const OUString& rConfigRoot)
{
	try
	{
		m_xCfg = css::uno::Reference< css::container::XNameAccess >(
			::comphelper::ConfigurationHelper::openConfig(
			utl::getProcessServiceFactory(),
			ROOTNODE_PRINTOPTION,
			::comphelper::ConfigurationHelper::E_STANDARD),
			css::uno::UNO_QUERY);

		if (m_xCfg.is())
		{
			UniString  sTmp = UniString(rConfigRoot);
			xub_StrLen nTokenCount = sTmp.GetTokenCount('/');
			sTmp = sTmp.GetToken(nTokenCount - 1, '/');
			m_xCfg->getByName(OUString(sTmp.GetBuffer())) >>= m_xNode;
		}
	}
	catch (const css::uno::Exception& ex)
	{
		m_xNode.clear();
		m_xCfg.clear();
		LogHelper::logIt(ex);
	}
}

sal_Bool SvtPrintOptions_Impl::IsReduceTransparency() const
{
	sal_Bool bRet = sal_False;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference< css::beans::XPropertySet > xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
				xSet->getPropertyValue(PROPERTYNAME_REDUCETRANSPARENCY) >>= bRet;
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

    return bRet;
}

sal_Int16 SvtPrintOptions_Impl::GetReducedTransparencyMode() const
{
	sal_Int16 nRet = 0;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference< css::beans::XPropertySet > xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDTRANSPARENCYMODE) >>= nRet;
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return  nRet;
}

sal_Bool SvtPrintOptions_Impl::IsReduceGradients() const
{
	sal_Bool bRet = sal_False;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEGRADIENTS) >>= bRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Int16 SvtPrintOptions_Impl::GetReducedGradientMode() const
{
	sal_Int16 nRet = 0;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDGRADIENTMODE) >>= nRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return nRet;
}

sal_Int16 SvtPrintOptions_Impl::GetReducedGradientStepCount() const
{
	sal_Int16 nRet = 64;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDGRADIENTSTEPCOUNT) >>= nRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return nRet;
}

sal_Bool SvtPrintOptions_Impl::IsReduceBitmaps() const
{
	sal_Bool bRet = sal_False;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEBITMAPS) >>= bRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Int16 SvtPrintOptions_Impl::GetReducedBitmapMode() const
{
	sal_Int16 nRet = 1;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDBITMAPMODE) >>= nRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return nRet;
}

sal_Int16 SvtPrintOptions_Impl::GetReducedBitmapResolution() const
{
	sal_Int16 nRet = 3;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDBITMAPRESOLUTION) >>= nRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return  nRet;
}

sal_Bool SvtPrintOptions_Impl::IsReducedBitmapIncludesTransparency() const
{
	sal_Bool bRet = sal_True;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_REDUCEDBITMAPINCLUDESTRANSPARENCY) >>= bRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return  bRet;
}

sal_Bool SvtPrintOptions_Impl::IsConvertToGreyscales() const
{
	sal_Bool bRet = sal_False;
	try
	{
		if (m_xNode.is())
		{
			css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
			if (xSet.is())
			{
				xSet->getPropertyValue(PROPERTYNAME_CONVERTTOGREYSCALES) >>= bRet;
			}
		}
	}
	catch (const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return  bRet;

}

void SvtPrintOptions_Impl::SetReduceTransparency(sal_Bool bState)
{
    impl_setValue(PROPERTYNAME_REDUCETRANSPARENCY, bState);
}

void SvtPrintOptions_Impl::SetReducedTransparencyMode(sal_Int16 nMode)
{
    impl_setValue(PROPERTYNAME_REDUCEDTRANSPARENCYMODE, nMode);
}

void SvtPrintOptions_Impl::SetReduceGradients(sal_Bool bState)
{
    impl_setValue(PROPERTYNAME_REDUCEGRADIENTS, bState);
}

void SvtPrintOptions_Impl::SetReducedGradientMode(sal_Int16 nMode)
{
    impl_setValue(PROPERTYNAME_REDUCEDGRADIENTMODE, nMode);
}

void SvtPrintOptions_Impl::SetReducedGradientStepCount(sal_Int16 nStepCount )
{
    impl_setValue(PROPERTYNAME_REDUCEDGRADIENTSTEPCOUNT, nStepCount);
}

void SvtPrintOptions_Impl::SetReduceBitmaps(sal_Bool bState )
{
    impl_setValue(PROPERTYNAME_REDUCEBITMAPS, bState);
}

void SvtPrintOptions_Impl::SetReducedBitmapMode(sal_Int16 nMode )
{
    impl_setValue(PROPERTYNAME_REDUCEDBITMAPMODE, nMode);
}

void SvtPrintOptions_Impl::SetReducedBitmapResolution(sal_Int16 nResolution )
{
    impl_setValue(PROPERTYNAME_REDUCEDBITMAPRESOLUTION, nResolution);
}

void SvtPrintOptions_Impl::SetReducedBitmapIncludesTransparency(sal_Bool bState )
{
    impl_setValue(PROPERTYNAME_REDUCEDBITMAPINCLUDESTRANSPARENCY, bState);
}

void SvtPrintOptions_Impl::SetConvertToGreyscales(sal_Bool bState)
{
    impl_setValue(PROPERTYNAME_CONVERTTOGREYSCALES, bState);
}

SvtPrintOptions_Impl::~SvtPrintOptions_Impl()
{
    m_xNode.clear();
    m_xCfg.clear();
}

void SvtPrintOptions_Impl::impl_setValue (const ::rtl::OUString& sProp,
                                                ::sal_Bool       bNew )
{
	try
	{
		if ( ! m_xNode.is())
            return;

        css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
        if ( ! xSet.is())
            return;

        ::sal_Bool bOld = ! bNew;
        if ( ! (xSet->getPropertyValue(sProp) >>= bOld))
            return;

        if (bOld != bNew)
        {
            xSet->setPropertyValue(sProp, css::uno::makeAny(bNew));
            ::comphelper::ConfigurationHelper::flush(m_xCfg);
        }
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtPrintOptions_Impl::impl_setValue (const ::rtl::OUString& sProp,
                                                ::sal_Int16      nNew )
{
	try
	{
		if ( ! m_xNode.is())
            return;

        css::uno::Reference<css::beans::XPropertySet> xSet(m_xNode, css::uno::UNO_QUERY);
        if ( ! xSet.is())
            return;

        ::sal_Int16 nOld = nNew+1;
        if ( ! (xSet->getPropertyValue(sProp) >>= nOld))
            return;

        if (nOld != nNew)
        {
            xSet->setPropertyValue(sProp, css::uno::makeAny(nNew));
            ::comphelper::ConfigurationHelper::flush(m_xCfg);
        }
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

// -----------------------------------------------------------------------------


// -----------------------
// - SvtBasePrintOptions -
// -----------------------

SvtBasePrintOptions::SvtBasePrintOptions()
{
}

// -----------------------------------------------------------------------------

SvtBasePrintOptions::~SvtBasePrintOptions()
{
}

// -----------------------------------------------------------------------------

Mutex& SvtBasePrintOptions::GetOwnStaticMutex()
{
	// Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
	// If these method first called (Mutex not already exist!) ...
    if( pMutex == NULL )
    {
		// ... we must create a new one. Protect follow code with the global mutex -
		// It must be - we create a static variable!
        MutexGuard aGuard( Mutex::getGlobalMutex() );
		// We must check our pointer again - because it can be that another instance of our class will be faster than these!
        if( pMutex == NULL )
        {
			// Create the new mutex and set it for return on static variable.
            static Mutex aMutex;
            pMutex = &aMutex;
        }
    }
	// Return new created or already existing mutex object.
    return *pMutex;
}

// -----------------------------------------------------------------------------

sal_Bool SvtBasePrintOptions::IsReduceTransparency() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsReduceTransparency();
}

// -----------------------------------------------------------------------------

sal_Int16 SvtBasePrintOptions::GetReducedTransparencyMode() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetReducedTransparencyMode();
}

// -----------------------------------------------------------------------------

sal_Bool SvtBasePrintOptions::IsReduceGradients() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsReduceGradients();
}

// -----------------------------------------------------------------------------

sal_Int16 SvtBasePrintOptions::GetReducedGradientMode() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetReducedGradientMode();
}

// -----------------------------------------------------------------------------

sal_Int16 SvtBasePrintOptions::GetReducedGradientStepCount() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetReducedGradientStepCount();
}

// -----------------------------------------------------------------------------

sal_Bool SvtBasePrintOptions::IsReduceBitmaps() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsReduceBitmaps();
}

// -----------------------------------------------------------------------------

sal_Int16 SvtBasePrintOptions::GetReducedBitmapMode() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetReducedBitmapMode();
}

// -----------------------------------------------------------------------------

sal_Int16 SvtBasePrintOptions::GetReducedBitmapResolution() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetReducedBitmapResolution();
}

// -----------------------------------------------------------------------------

sal_Bool SvtBasePrintOptions::IsReducedBitmapIncludesTransparency() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsReducedBitmapIncludesTransparency();
}

// -----------------------------------------------------------------------------

sal_Bool SvtBasePrintOptions::IsConvertToGreyscales() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsConvertToGreyscales();
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReduceTransparency( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReduceTransparency( bState ) ;
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedTransparencyMode( sal_Int16 nMode )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedTransparencyMode( nMode );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReduceGradients( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReduceGradients( bState );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedGradientMode( sal_Int16 nMode )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedGradientMode( nMode );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedGradientStepCount( sal_Int16 nStepCount )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedGradientStepCount( nStepCount );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReduceBitmaps( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReduceBitmaps( bState );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedBitmapMode( sal_Int16 nMode )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedBitmapMode( nMode );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedBitmapResolution( sal_Int16 nResolution )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedBitmapResolution( nResolution );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetReducedBitmapIncludesTransparency( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetReducedBitmapIncludesTransparency( bState );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetConvertToGreyscales( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetConvertToGreyscales( bState );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::GetPrinterOptions( PrinterOptions& rOptions ) const
{
    rOptions.SetReduceTransparency( IsReduceTransparency() );
    rOptions.SetReducedTransparencyMode( (PrinterTransparencyMode) GetReducedTransparencyMode() );
    rOptions.SetReduceGradients( IsReduceGradients() );
    rOptions.SetReducedGradientMode( (PrinterGradientMode) GetReducedGradientMode() );
    rOptions.SetReducedGradientStepCount( GetReducedGradientStepCount() );
    rOptions.SetReduceBitmaps( IsReduceBitmaps() );
    rOptions.SetReducedBitmapMode( (PrinterBitmapMode) GetReducedBitmapMode() );
    rOptions.SetReducedBitmapResolution( aDPIArray[ Min( (sal_uInt16) GetReducedBitmapResolution(), (sal_uInt16)( DPI_COUNT - 1 ) ) ] );
    rOptions.SetReducedBitmapIncludesTransparency( IsReducedBitmapIncludesTransparency() );
    rOptions.SetConvertToGreyscales( IsConvertToGreyscales() );
}

// -----------------------------------------------------------------------------

void SvtBasePrintOptions::SetPrinterOptions( const PrinterOptions& rOptions )
{
    SetReduceTransparency( rOptions.IsReduceTransparency() );
    SetReducedTransparencyMode(
        sal::static_int_cast< sal_Int16 >(
            rOptions.GetReducedTransparencyMode()) );
    SetReduceGradients( rOptions.IsReduceGradients() );
    SetReducedGradientMode(
        sal::static_int_cast< sal_Int16 >(rOptions.GetReducedGradientMode()) );
    SetReducedGradientStepCount( rOptions.GetReducedGradientStepCount() );
    SetReduceBitmaps( rOptions.IsReduceBitmaps() );
    SetReducedBitmapMode(
        sal::static_int_cast< sal_Int16 >(rOptions.GetReducedBitmapMode()) );
    SetReducedBitmapIncludesTransparency( rOptions.IsReducedBitmapIncludesTransparency() );
    SetConvertToGreyscales( rOptions.IsConvertToGreyscales() );

    const sal_uInt16 nDPI = rOptions.GetReducedBitmapResolution();

    if( nDPI < aDPIArray[ 0 ] )
        SetReducedBitmapResolution( 0 );
    else
    {
        for( long i = ( DPI_COUNT - 1 ); i >= 0; i-- )
        {
            if( nDPI >= aDPIArray[ i ] )
            {
                SetReducedBitmapResolution( (sal_Int16) i );
                i = -1;
            }
        }
    }
}

// ---------------------
// - SvtPrinterOptions -
// ---------------------

SvtPrinterOptions::SvtPrinterOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already!
    if( m_pStaticDataContainer == NULL )
	{
        OUString aRootPath( ROOTNODE_START );
        m_pStaticDataContainer = new SvtPrintOptions_Impl( aRootPath += OUString( RTL_CONSTASCII_USTRINGPARAM( "/Printer" ) ) );
        pPrinterOptionsDataContainer = m_pStaticDataContainer;
		ItemHolder2::holdConfigItem(E_PRINTOPTIONS);
	}

    SetDataContainer( m_pStaticDataContainer );
}

// -----------------------------------------------------------------------------

SvtPrinterOptions::~SvtPrinterOptions()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
    if( m_nRefCount <= 0 )
	{
		delete m_pStaticDataContainer;
		m_pStaticDataContainer = NULL;
        pPrinterOptionsDataContainer = NULL;
	}
}

// ---------------------
// - SvtPrintFileOptions -
// ---------------------

SvtPrintFileOptions::SvtPrintFileOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already!
    if( m_pStaticDataContainer == NULL )
	{
        OUString aRootPath( ROOTNODE_START );
        m_pStaticDataContainer = new SvtPrintOptions_Impl( aRootPath += OUString( RTL_CONSTASCII_USTRINGPARAM( "/File" ) ) );
        pPrintFileOptionsDataContainer = m_pStaticDataContainer;

		ItemHolder2::holdConfigItem(E_PRINTFILEOPTIONS);
	}

    SetDataContainer( m_pStaticDataContainer );
}

// -----------------------------------------------------------------------------

SvtPrintFileOptions::~SvtPrintFileOptions()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
    if( m_nRefCount <= 0 )
	{
		delete m_pStaticDataContainer;
		m_pStaticDataContainer = NULL;
        pPrintFileOptionsDataContainer = NULL;
	}
}
