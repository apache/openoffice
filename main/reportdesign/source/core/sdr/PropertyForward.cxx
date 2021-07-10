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



#include "precompiled_reportdesign.hxx"
#include "PropertyForward.hxx"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <comphelper/property.hxx>
#include <com/sun/star/sdbcx/XAppend.hpp>
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>
#include "corestrings.hrc"

//........................................................................
namespace rptui
{
//........................................................................
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::beans;
	using namespace reportdesign;

DBG_NAME( rpt_OPropertyMediator )
OPropertyMediator::OPropertyMediator(const Reference< XPropertySet>& _xSource
									 ,const Reference< XPropertySet>& _xDest
									 ,const TPropertyNamePair& _aNameMap
									 ,sal_Bool _bReverse)
								: OPropertyForward_Base(m_aMutex)
								,m_aNameMap(_aNameMap)
								,m_xSource(_xSource)
								,m_xDest(_xDest)
								,m_bInChange(sal_False)
{
	DBG_CTOR( rpt_OPropertyMediator,NULL);
	osl_incrementInterlockedCount(&m_refCount);
	OSL_ENSURE(m_xDest.is(),"Dest is NULL!");
	OSL_ENSURE(m_xSource.is(),"Source is NULL!");
	if ( m_xDest.is() && m_xSource.is() )
	{
		try
		{
			m_xDestInfo = m_xDest->getPropertySetInfo();
			m_xSourceInfo = m_xSource->getPropertySetInfo();
			if ( _bReverse )
			{
				::comphelper::copyProperties(m_xDest,m_xSource);
				TPropertyNamePair::iterator aIter = m_aNameMap.begin();
				TPropertyNamePair::iterator aEnd = m_aNameMap.end();
				for (; aIter != aEnd; ++aIter)
				{
					Property aProp = m_xSourceInfo->getPropertyByName(aIter->first);
					if (0 == (aProp.Attributes & PropertyAttribute::READONLY))
					{
						Any aValue = _xDest->getPropertyValue(aIter->second.first);
						if ( 0 != (aProp.Attributes & PropertyAttribute::MAYBEVOID) || aValue.hasValue() )
							_xSource->setPropertyValue(aIter->first,aIter->second.second->operator()(aIter->second.first,aValue));
					}
				}
			}
			else
			{
				::comphelper::copyProperties(m_xSource,m_xDest);
				TPropertyNamePair::iterator aIter = m_aNameMap.begin();
				TPropertyNamePair::iterator aEnd = m_aNameMap.end();
				for (; aIter != aEnd; ++aIter)
					_xDest->setPropertyValue(aIter->second.first,aIter->second.second->operator()(aIter->second.first,_xSource->getPropertyValue(aIter->first)));
			}
			startListening();
		}
		catch(Exception& e)
		{
			DBG_UNHANDLED_EXCEPTION();
			(void)e;
		}
	} // if ( m_xDest.is() && m_xSource.is() )
	osl_decrementInterlockedCount(&m_refCount);
}
// -----------------------------------------------------------------------------
OPropertyMediator::~OPropertyMediator()
{
	DBG_DTOR( rpt_OPropertyMediator,NULL);
}
// -----------------------------------------------------------------------------
void SAL_CALL OPropertyMediator::propertyChange( const PropertyChangeEvent& evt ) throw(RuntimeException)
{
	::osl::MutexGuard aGuard(m_aMutex);
	if ( !m_bInChange )
	{
		m_bInChange = sal_True;
		try
		{
			sal_Bool bDest = (evt.Source == m_xDest);
			Reference<XPropertySet> xProp = bDest ? m_xSource : m_xDest;
			Reference<XPropertySetInfo> xPropInfo = bDest ? m_xSourceInfo : m_xDestInfo;
			if ( xProp.is() )
			{
				if ( xPropInfo.is() )
				{
					if ( xPropInfo->hasPropertyByName(evt.PropertyName) )
						xProp->setPropertyValue(evt.PropertyName,evt.NewValue);
					else
					{
						TPropertyNamePair::iterator aFind = m_aNameMap.find(evt.PropertyName);
						::rtl::OUString sPropName;
						if ( aFind != m_aNameMap.end() )
							sPropName = aFind->second.first;
						else
						{
							aFind = ::std::find_if(
								m_aNameMap.begin(),
								m_aNameMap.end(),
								::std::compose1(
								::std::bind2nd(::std::equal_to< ::rtl::OUString >(), evt.PropertyName),
									::std::compose1(::std::select1st<TPropertyConverter>(),::std::select2nd<TPropertyNamePair::value_type>())
								)
							);
							if ( aFind != m_aNameMap.end() )
								sPropName = aFind->first;
						}
						if ( sPropName.getLength() && xPropInfo->hasPropertyByName(sPropName) )
							xProp->setPropertyValue(sPropName,aFind->second.second->operator()(sPropName,evt.NewValue));
						else if (  evt.PropertyName == PROPERTY_CHARFONTNAME
								|| evt.PropertyName == PROPERTY_CHARFONTSTYLENAME
								|| evt.PropertyName == PROPERTY_CHARSTRIKEOUT
								|| evt.PropertyName == PROPERTY_CHARWORDMODE
								|| evt.PropertyName == PROPERTY_CHARROTATION
								|| evt.PropertyName == PROPERTY_CHARSCALEWIDTH
								|| evt.PropertyName == PROPERTY_CHARFONTFAMILY
								|| evt.PropertyName == PROPERTY_CHARFONTCHARSET
								|| evt.PropertyName == PROPERTY_CHARFONTPITCH
								|| evt.PropertyName == PROPERTY_CHARHEIGHT
								|| evt.PropertyName == PROPERTY_CHARUNDERLINE
								|| evt.PropertyName == PROPERTY_CHARWEIGHT
								|| evt.PropertyName == PROPERTY_CHARPOSTURE)
						{
							xProp->setPropertyValue(PROPERTY_FONTDESCRIPTOR,m_xSource->getPropertyValue(PROPERTY_FONTDESCRIPTOR));
						}
					}
				}
			}
		}
		catch(Exception&)
		{
			OSL_ENSURE(0,"Exception caught!");
		}
		m_bInChange = sal_False;
	}
}
// -----------------------------------------------------------------------------
void SAL_CALL OPropertyMediator::disposing( const ::com::sun::star::lang::EventObject& /*_rSource*/ ) throw (RuntimeException)
{
	::osl::MutexGuard aGuard(m_aMutex);
	disposing();
}
// -----------------------------------------------------------------------------
void SAL_CALL OPropertyMediator::disposing()
{
	stopListening();
	m_xSource.clear();
	m_xSourceInfo.clear();
	m_xDest.clear();
	m_xDestInfo.clear();
}
// -----------------------------------------------------------------------------
void OPropertyMediator::stopListening()
{
	if ( m_xSource.is() )
		m_xSource->removePropertyChangeListener(::rtl::OUString(), this);
	if ( m_xDest.is() )
		m_xDest->removePropertyChangeListener(::rtl::OUString(), this);
}
// -----------------------------------------------------------------------------
void OPropertyMediator::startListening()
{
	if ( m_xSource.is() )
		m_xSource->addPropertyChangeListener(::rtl::OUString(), this);
	if ( m_xDest.is() )
		m_xDest->addPropertyChangeListener(::rtl::OUString(), this);
}
// -----------------------------------------------------------------------------
//........................................................................
}	// namespace dbaccess
//........................................................................

