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
#include "precompiled_unotools.hxx"
#ifndef GCC
#endif

//_________________________________________________________________________________________________________________
// includes
//_________________________________________________________________________________________________________________

#include <unotools/historyoptions.hxx>
#include <unotools/historyoptions_const.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#ifndef __SGI_STL_DEQUE
#include <deque>
#endif

#ifndef __SGI_STL_ALGORITHM
#include <algorithm>
#endif

#include <rtl/logfile.hxx>
#include "itemholder1.hxx"

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

//_________________________________________________________________________________________________________________
// namespaces
//_________________________________________________________________________________________________________________

using namespace ::std     ;
using namespace ::utl     ;
using namespace ::rtl     ;
using namespace ::osl     ;
using namespace ::com::sun::star::uno ;
using namespace ::com::sun::star::beans ;

namespace css = ::com::sun::star;

//_________________________________________________________________________________________________________________
// const
//_________________________________________________________________________________________________________________

namespace {
	static const ::sal_Int32 s_nOffsetURL               = 0;
	static const ::sal_Int32 s_nOffsetFilter            = 1;
	static const ::sal_Int32 s_nOffsetTitle             = 2;
	static const ::sal_Int32 s_nOffsetPassword          = 3;
}

//_________________________________________________________________________________________________________________
// private declarations!
//_________________________________________________________________________________________________________________

struct IMPL_THistoryItem
{
	IMPL_THistoryItem()
	{
	}

	IMPL_THistoryItem( const OUString& sNewURL   ,
		const OUString& sNewFilter  ,
		const OUString& sNewTitle  ,
		const OUString& sNewPassword )
	{
		sURL  = sNewURL  ;
		sFilter  = sNewFilter ;
		sTitle  = sNewTitle  ;
		sPassword = sNewPassword ;
	}

	sal_Bool operator==( const OUString& sSearchedURL )
	{
		return( sURL == sSearchedURL );
	}

	OUString sURL  ;
	OUString sFilter  ;
	OUString sTitle  ;
	OUString sPassword ;
};

//*****************************************************************************************************************
//  class SvtHistoryOptions_Impl
//  redesigned
//*****************************************************************************************************************
class SvtHistoryOptions_Impl
{
public:
	SvtHistoryOptions_Impl();
	~SvtHistoryOptions_Impl();

	sal_uInt32 GetSize( EHistoryType eHistory );
	void SetSize( EHistoryType eHistory, sal_uInt32 nSize );
	void Clear( EHistoryType eHistory );
	Sequence< Sequence< PropertyValue > > GetList( EHistoryType eHistory );
	void                                  AppendItem(       EHistoryType eHistory ,
		const OUString&    sURL     ,
		const OUString&    sFilter  ,
		const OUString&    sTitle   ,
		const OUString&    sPassword );

private:
    void impl_truncateList (EHistoryType eHistory, sal_uInt32 nSize);

private:
	css::uno::Reference< css::container::XNameAccess > m_xCfg;
	css::uno::Reference< css::container::XNameAccess > m_xCommonXCU;
};

//*****************************************************************************************************************
//  constructor
//*****************************************************************************************************************
SvtHistoryOptions_Impl::SvtHistoryOptions_Impl()
{
	try
	{
		m_xCfg = Reference< css::container::XNameAccess > (
			::comphelper::ConfigurationHelper::openConfig(
			utl::getProcessServiceFactory(),
			s_sHistories,
			::comphelper::ConfigurationHelper::E_STANDARD),
			css::uno::UNO_QUERY );

		m_xCommonXCU = Reference< css::container::XNameAccess > (
			::comphelper::ConfigurationHelper::openConfig(
			utl::getProcessServiceFactory(),
			s_sCommonHistory,
			::comphelper::ConfigurationHelper::E_STANDARD),
			css::uno::UNO_QUERY );
	}
	catch(const css::uno::Exception& ex)
	{
		m_xCfg.clear();
		m_xCommonXCU.clear();

		LogHelper::logIt(ex);
	}
}

//*****************************************************************************************************************
//  destructor
//*****************************************************************************************************************
SvtHistoryOptions_Impl::~SvtHistoryOptions_Impl()
{
}

//*****************************************************************************************************************
//  public method
//  Attention: We return the max. size of our internal lists - That is the capacity not the size!
//*****************************************************************************************************************
sal_uInt32 SvtHistoryOptions_Impl::GetSize( EHistoryType eHistory )
{
	sal_uInt32                                       nSize = 0  ;
	css::uno::Reference< css::beans::XPropertySet >  xListAccess(m_xCommonXCU, css::uno::UNO_QUERY);

	try
	{
		switch( eHistory )
		{
		case ePICKLIST:
			xListAccess->getPropertyValue(s_sPickListSize) >>= nSize;
			break;

		case eHISTORY:
			xListAccess->getPropertyValue(s_sURLHistorySize) >>= nSize;
			break;

		case eHELPBOOKMARKS:
			xListAccess->getPropertyValue(s_sHelpBookmarksSize) >>= nSize;
			break;

		default:
			break;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return nSize;
}

//*****************************************************************************************************************
//  public method
//  Attention: We return the max. size of our internal lists - That is the capacity not the size!
//*****************************************************************************************************************
void SvtHistoryOptions_Impl::SetSize( EHistoryType eHistory, sal_uInt32 nSize )
{
	css::uno::Reference< css::beans::XPropertySet > xListAccess(m_xCommonXCU, css::uno::UNO_QUERY);
    if (! xListAccess.is ())
        return;

	try
	{
		switch( eHistory )
		{
		case ePICKLIST:
			if(nSize!=GetSize(ePICKLIST))
			{
				xListAccess->setPropertyValue(s_sPickListSize, css::uno::makeAny(nSize));
				::comphelper::ConfigurationHelper::flush(m_xCommonXCU);
			}
			break;

		case eHISTORY:
			if(nSize!=GetSize(eHISTORY))
			{
				xListAccess->setPropertyValue(s_sURLHistorySize, css::uno::makeAny(nSize));
				::comphelper::ConfigurationHelper::flush(m_xCommonXCU);
			}
			break;

		case eHELPBOOKMARKS:
			if(nSize!=GetSize(eHELPBOOKMARKS))
			{
				xListAccess->setPropertyValue(s_sHelpBookmarksSize, css::uno::makeAny(nSize));
				::comphelper::ConfigurationHelper::flush(m_xCommonXCU);
			}
			break;

		default:
			break;
		}

        impl_truncateList (eHistory, nSize);
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

//*****************************************************************************************************************
void SvtHistoryOptions_Impl::impl_truncateList ( EHistoryType eHistory, sal_uInt32 nSize )
{
	css::uno::Reference< css::container::XNameAccess >    xList;
	css::uno::Reference< css::container::XNameContainer > xItemList;
	css::uno::Reference< css::container::XNameContainer > xOrderList;
	css::uno::Reference< css::beans::XPropertySet >       xSet;

	try
	{
		switch( eHistory )
		{
		case ePICKLIST:
            m_xCfg->getByName(s_sPickList) >>= xList;
			break;

		case eHISTORY:
            m_xCfg->getByName(s_sURLHistory) >>= xList;
			break;

		case eHELPBOOKMARKS:
            m_xCfg->getByName(s_sHelpBookmarks) >>= xList;
			break;

		default:
			break;
		}

		// If too much items in current list ...
		// truncate the oldest items BEFORE you set the new one.
		if ( ! xList.is())
            return;

        xList->getByName(s_sOrderList) >>= xOrderList;
        xList->getByName(s_sItemList)  >>= xItemList;

        const sal_uInt32 nLength = xOrderList->getElementNames().getLength();
        if (nSize < nLength)
        {
            for (sal_uInt32 i=nLength-1; i>=nSize; --i)
            {
                ::rtl::OUString sTmp;
                const ::rtl::OUString sRemove = ::rtl::OUString::valueOf((sal_Int32)i);
                xOrderList->getByName(sRemove) >>= xSet;
                xSet->getPropertyValue(s_sHistoryItemRef) >>= sTmp;
                xItemList->removeByName(sTmp);
                xOrderList->removeByName(sRemove);
            }

            ::comphelper::ConfigurationHelper::flush(m_xCfg);
        }
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

//*****************************************************************************************************************
//  public method
//  Clear specified history list
//*****************************************************************************************************************
void SvtHistoryOptions_Impl::Clear( EHistoryType eHistory )
{
	css::uno::Reference< css::container::XNameAccess >    xListAccess;
	css::uno::Reference< css::container::XNameContainer > xNode;
	Sequence< ::rtl::OUString >                           lOrders;

	try
	{
		switch( eHistory )
		{
		case ePICKLIST:
			{
				m_xCfg->getByName(s_sPickList) >>= xListAccess;
				break;
			}

		case eHISTORY:
			{
				m_xCfg->getByName(s_sURLHistory) >>= xListAccess;
				break;
			}

		case eHELPBOOKMARKS:
			{
				m_xCfg->getByName(s_sHelpBookmarks) >>= xListAccess;
				break;
			}

		default:
			break;
		}

		if (xListAccess.is())
		{
			// clear ItemList
			xListAccess->getByName(s_sItemList)  >>= xNode  ;
			lOrders = xNode->getElementNames();
			const sal_Int32 nLength = lOrders.getLength();
			for(sal_Int32 i=0; i<nLength; ++i)
				xNode->removeByName(lOrders[i]);

			// clear OrderList
			xListAccess->getByName(s_sOrderList) >>= xNode ;
			lOrders = xNode->getElementNames();
			for(sal_Int32 j=0; j<nLength; ++j)
				xNode->removeByName(lOrders[j]);

			::comphelper::ConfigurationHelper::flush(m_xCfg);
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

//*****************************************************************************************************************
//  public method
//  get a sequence list from the items
//*****************************************************************************************************************
Sequence< Sequence< PropertyValue > > SvtHistoryOptions_Impl::GetList( EHistoryType eHistory )
{
    impl_truncateList (eHistory, GetSize (eHistory));

	Sequence< Sequence< PropertyValue > > seqReturn; // Set default return value.
	Sequence< PropertyValue >             seqProperties( 4 );
	Sequence< ::rtl::OUString >           lOrders;

	css::uno::Reference< css::container::XNameAccess > xListAccess;
	css::uno::Reference< css::container::XNameAccess > xItemList;
	css::uno::Reference< css::container::XNameAccess > xOrderList;
	css::uno::Reference< css::beans::XPropertySet >    xSet;

	seqProperties[s_nOffsetURL       ].Name = HISTORY_PROPERTYNAME_URL;
	seqProperties[s_nOffsetFilter    ].Name = HISTORY_PROPERTYNAME_FILTER;
	seqProperties[s_nOffsetTitle     ].Name = HISTORY_PROPERTYNAME_TITLE;
	seqProperties[s_nOffsetPassword  ].Name = HISTORY_PROPERTYNAME_PASSWORD;

	try
	{
		switch( eHistory )
		{
		case ePICKLIST:
			{
				m_xCfg->getByName(s_sPickList) >>= xListAccess;
				break;
			}

		case eHISTORY:
			{
				m_xCfg->getByName(s_sURLHistory) >>= xListAccess;
				break;
			}

		case eHELPBOOKMARKS:
			{
				m_xCfg->getByName(s_sHelpBookmarks) >>= xListAccess;
				break;
			}

		default:
			break;
		}

		if (xListAccess.is())
		{
			xListAccess->getByName(s_sItemList)  >>= xItemList;
			xListAccess->getByName(s_sOrderList) >>= xOrderList;

			const sal_Int32 nLength = xOrderList->getElementNames().getLength();
			Sequence< Sequence< PropertyValue > > aRet(nLength);

			for(sal_Int32 nItem=0; nItem<nLength; ++nItem)
			{
				::rtl::OUString sUrl;
				xOrderList->getByName(::rtl::OUString::valueOf(nItem)) >>= xSet;
				xSet->getPropertyValue(s_sHistoryItemRef) >>= sUrl;

				xItemList->getByName(sUrl) >>= xSet;
				seqProperties[s_nOffsetURL  ].Value <<= sUrl;
				xSet->getPropertyValue(s_sFilter)   >>= seqProperties[s_nOffsetFilter   ].Value;
				xSet->getPropertyValue(s_sTitle)    >>= seqProperties[s_nOffsetTitle    ].Value;
				xSet->getPropertyValue(s_sPassword) >>= seqProperties[s_nOffsetPassword ].Value;
				aRet[nItem] = seqProperties;
			}
			seqReturn = aRet;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return seqReturn;
}

//*****************************************************************************************************************
//  public method
//  implements a deque in XML
//*****************************************************************************************************************
void SvtHistoryOptions_Impl::AppendItem(       EHistoryType eHistory ,
										const OUString& sURL        ,
										const OUString& sFilter     ,
										const OUString& sTitle      ,
										const OUString& sPassword   )
{
    impl_truncateList (eHistory, GetSize (eHistory));

	css::uno::Reference< css::container::XNameAccess > xListAccess;
	sal_Int32             nMaxSize = 0;

	switch(eHistory)
	{
	case ePICKLIST:
		{
			m_xCfg->getByName(s_sPickList) >>= xListAccess;
			nMaxSize = GetSize(ePICKLIST);
		}
		break;
	case eHISTORY:
		{
			m_xCfg->getByName(s_sURLHistory) >>= xListAccess;
			nMaxSize = GetSize(eHISTORY);
		}
		break;
	case eHELPBOOKMARKS:
		{
			m_xCfg->getByName(s_sHelpBookmarks) >>= xListAccess;
			nMaxSize = GetSize(eHELPBOOKMARKS);
		}
		break;
	default:
		break;
	}

	if (nMaxSize==0)
		return;

	css::uno::Reference< css::container::XNameContainer > xItemList;
	css::uno::Reference< css::container::XNameContainer > xOrderList;
	css::uno::Reference< css::beans::XPropertySet >       xSet;

	try
	{
		xListAccess->getByName(s_sItemList)  >>= xItemList;
		xListAccess->getByName(s_sOrderList) >>= xOrderList;
		sal_Int32 nLength = xOrderList->getElementNames().getLength();

		// The item to be appended is already existing!
		if (xItemList->hasByName(sURL))
		{
			for (sal_Int32 i=0; i<nLength; ++i)
			{
				::rtl::OUString sTmp;
				xOrderList->getByName(::rtl::OUString::valueOf(i)) >>= xSet;
				xSet->getPropertyValue(s_sHistoryItemRef) >>= sTmp;

				if(sURL == sTmp)
				{
					::rtl::OUString sFind;
					xOrderList->getByName( ::rtl::OUString::valueOf(i) ) >>= xSet;
					xSet->getPropertyValue(s_sHistoryItemRef) >>= sFind;
					for (sal_Int32 j=i-1; j>=0; --j)
					{
						css::uno::Reference< css::beans::XPropertySet > xPrevSet;
						css::uno::Reference< css::beans::XPropertySet > xNextSet;
						xOrderList->getByName( ::rtl::OUString::valueOf(j+1) )   >>= xPrevSet;
						xOrderList->getByName( ::rtl::OUString::valueOf(j) )     >>= xNextSet;

						::rtl::OUString sTemp;
						xNextSet->getPropertyValue(s_sHistoryItemRef) >>= sTemp;
						xPrevSet->setPropertyValue(s_sHistoryItemRef, css::uno::makeAny(sTemp));
					}
					xOrderList->getByName( ::rtl::OUString::valueOf((sal_Int32)0) ) >>= xSet;
					xSet->setPropertyValue(s_sHistoryItemRef, css::uno::makeAny(sFind));

					::comphelper::ConfigurationHelper::flush(m_xCfg);
					break;
				}
			}
		}

		// The item to be appended is not existing!
		else
		{
			css::uno::Reference< css::lang::XSingleServiceFactory > xFac;
			css::uno::Reference< css::uno::XInterface >             xInst;
			css::uno::Reference< css::beans::XPropertySet > xPrevSet;
			css::uno::Reference< css::beans::XPropertySet > xNextSet;

			// Append new item to OrderList.
			if ( nLength == nMaxSize )
			{
				::rtl::OUString sRemove;
				xOrderList->getByName(::rtl::OUString::valueOf(nLength-1)) >>= xSet;
				xSet->getPropertyValue(s_sHistoryItemRef) >>= sRemove;
				xItemList->removeByName(sRemove);
			}
			if ( nLength != nMaxSize )
			{
				xFac = css::uno::Reference< css::lang::XSingleServiceFactory >(xOrderList, css::uno::UNO_QUERY);
				xInst = xFac->createInstance();
				::rtl::OUString sPush = ::rtl::OUString::valueOf(nLength++);
				xOrderList->insertByName(sPush, css::uno::makeAny(xInst));
			}
			for (sal_Int32 j=nLength-1; j>0; --j)
			{
				xOrderList->getByName( ::rtl::OUString::valueOf(j) )   >>= xPrevSet;
				xOrderList->getByName( ::rtl::OUString::valueOf(j-1) ) >>= xNextSet;
				::rtl::OUString sTemp;
				xNextSet->getPropertyValue(s_sHistoryItemRef) >>= sTemp;
				xPrevSet->setPropertyValue(s_sHistoryItemRef, css::uno::makeAny(sTemp));
			}
			xOrderList->getByName( ::rtl::OUString::valueOf((sal_Int32)0) ) >>= xSet;
			xSet->setPropertyValue(s_sHistoryItemRef, css::uno::makeAny(sURL));

			// Append the item to ItemList.
			xFac = css::uno::Reference< css::lang::XSingleServiceFactory >(xItemList, css::uno::UNO_QUERY);
			xInst = xFac->createInstance();
			xItemList->insertByName(sURL, css::uno::makeAny(xInst));
			xSet = css::uno::Reference< css::beans::XPropertySet >(xInst, css::uno::UNO_QUERY);
			xSet->setPropertyValue(s_sFilter, css::uno::makeAny(sFilter));
			xSet->setPropertyValue(s_sTitle, css::uno::makeAny(sTitle));
			xSet->setPropertyValue(s_sPassword, css::uno::makeAny(sPassword));

			::comphelper::ConfigurationHelper::flush(m_xCfg);
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

//*****************************************************************************************************************
// initialize static member
// DON'T DO IT IN YOUR HEADER!
// see definition for further informations
//*****************************************************************************************************************
SvtHistoryOptions_Impl*  SvtHistoryOptions::m_pDataContainer = NULL ;
sal_Int32     SvtHistoryOptions::m_nRefCount  = 0  ;

//*****************************************************************************************************************
// constructor
//*****************************************************************************************************************
SvtHistoryOptions::SvtHistoryOptions()
{
	// Global access, must be guarded (multithreading!).
	MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already exist!
	if( m_pDataContainer == NULL )
	{
		RTL_LOGFILE_CONTEXT(aLog, "unotools ( ??? ) ::SvtHistoryOptions_Impl::ctor()");
		m_pDataContainer = new SvtHistoryOptions_Impl;

		ItemHolder1::holdConfigItem(E_HISTORYOPTIONS);
	}
}

//*****************************************************************************************************************
// destructor
//*****************************************************************************************************************
SvtHistoryOptions::~SvtHistoryOptions()
{
	// Global access, must be guarded (multithreading!)
	MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
	if( m_nRefCount <= 0 )
	{
		delete m_pDataContainer;
		m_pDataContainer = NULL;
	}
}

//*****************************************************************************************************************
// public method
//*****************************************************************************************************************
sal_uInt32 SvtHistoryOptions::GetSize( EHistoryType eHistory ) const
{
	MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetSize( eHistory );
}

//*****************************************************************************************************************
// public method
//*****************************************************************************************************************
void SvtHistoryOptions::SetSize( EHistoryType eHistory, sal_uInt32 nSize )
{
	MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetSize( eHistory, nSize );
}

//*****************************************************************************************************************
// public method
//*****************************************************************************************************************
void SvtHistoryOptions::Clear( EHistoryType eHistory )
{
	MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->Clear( eHistory );
}

//*****************************************************************************************************************
// public method
//*****************************************************************************************************************
Sequence< Sequence< PropertyValue > > SvtHistoryOptions::GetList( EHistoryType eHistory ) const
{
	MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetList( eHistory );
}

//*****************************************************************************************************************
// public method
//*****************************************************************************************************************
void SvtHistoryOptions::AppendItem(   EHistoryType eHistory ,
								   const OUString&  sURL  ,
								   const OUString&  sFilter  ,
								   const OUString&  sTitle  ,
								   const OUString&  sPassword )
{
	MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->AppendItem( eHistory, sURL, sFilter, sTitle, sPassword );
}

//*****************************************************************************************************************
// private method
//*****************************************************************************************************************
Mutex& SvtHistoryOptions::GetOwnStaticMutex()
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
