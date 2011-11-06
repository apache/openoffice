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



#include <vector>

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif
#include <rtl/uuid.h>
#include <rtl/memory.h>

#include "attrlist.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

struct SvXMLTagAttribute_Impl
{
	SvXMLTagAttribute_Impl(){}
	SvXMLTagAttribute_Impl( const OUString &rName,
						 const OUString &rValue )
		: sName(rName),
		sValue(rValue)
	{
	}

	OUString sName;
	OUString sValue;
};

struct SvXMLAttributeList_Impl
{
	SvXMLAttributeList_Impl()
	{
		// performance improvement during adding
		vecAttribute.reserve(20);
	}
	::std::vector<struct SvXMLTagAttribute_Impl> vecAttribute;
};



sal_Int16 SAL_CALL SvXMLAttributeList::getLength(void) throw( ::com::sun::star::uno::RuntimeException )
{
	return m_pImpl->vecAttribute.size();
}


SvXMLAttributeList::SvXMLAttributeList( const SvXMLAttributeList &r )
{
	m_pImpl = new SvXMLAttributeList_Impl;
	*m_pImpl = *(r.m_pImpl);
}

SvXMLAttributeList::SvXMLAttributeList( const uno::Reference< 
		xml::sax::XAttributeList> & rAttrList )
    : sType( GetXMLToken(XML_CDATA) )
{
	m_pImpl = new SvXMLAttributeList_Impl;

	SvXMLAttributeList* pImpl =
		SvXMLAttributeList::getImplementation( rAttrList );

	if( pImpl )
		*m_pImpl = *(pImpl->m_pImpl);
	else
		AppendAttributeList( rAttrList );
}

OUString SAL_CALL SvXMLAttributeList::getNameByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException )
{
	if( i < m_pImpl->vecAttribute.size() ) {
		return m_pImpl->vecAttribute[i].sName;
	}
	return OUString();
}


OUString SAL_CALL SvXMLAttributeList::getTypeByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException )
{
    return sType;
}

OUString SAL_CALL  SvXMLAttributeList::getValueByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException )
{
	if( i < m_pImpl->vecAttribute.size() ) {
		return m_pImpl->vecAttribute[i].sValue;
	}
	return OUString();

}

OUString SAL_CALL SvXMLAttributeList::getTypeByName( const OUString& sName ) throw( ::com::sun::star::uno::RuntimeException )
{
    return sType;
}

OUString SAL_CALL SvXMLAttributeList::getValueByName(const OUString& sName) throw( ::com::sun::star::uno::RuntimeException )
{
	::std::vector<struct SvXMLTagAttribute_Impl>::iterator ii = m_pImpl->vecAttribute.begin();

	for( ; ii != m_pImpl->vecAttribute.end() ; ii ++ ) {
		if( (*ii).sName == sName ) {
			return (*ii).sValue;
		}
	}
	return OUString();
}


uno::Reference< ::com::sun::star::util::XCloneable >  SvXMLAttributeList::createClone() throw( ::com::sun::star::uno::RuntimeException )
{
	uno::Reference< ::com::sun::star::util::XCloneable >  r = new SvXMLAttributeList( *this );
	return r;
}


SvXMLAttributeList::SvXMLAttributeList()
    : sType( GetXMLToken(XML_CDATA) )
{
	m_pImpl = new SvXMLAttributeList_Impl;
}



SvXMLAttributeList::~SvXMLAttributeList()
{
	delete m_pImpl;
}


void SvXMLAttributeList::AddAttribute( 	const OUString &sName ,
										const OUString &sValue )
{
	m_pImpl->vecAttribute.push_back( SvXMLTagAttribute_Impl( sName , sValue ) );
}

void SvXMLAttributeList::Clear()
{
	m_pImpl->vecAttribute.clear();

	OSL_ASSERT( ! getLength() );
}

void SvXMLAttributeList::RemoveAttribute( const OUString sName )
{
	::std::vector<struct SvXMLTagAttribute_Impl>::iterator ii = m_pImpl->vecAttribute.begin();

	for( ; ii != m_pImpl->vecAttribute.end() ; ii ++ ) {
		if( (*ii).sName == sName ) {
			m_pImpl->vecAttribute.erase( ii );
			break;
		}
	}
}

void SvXMLAttributeList::AppendAttributeList( const uno::Reference< ::com::sun::star::xml::sax::XAttributeList >  &r )
{
	OSL_ASSERT( r.is() );

	sal_Int32 nMax = r->getLength();
	sal_Int32 nTotalSize = m_pImpl->vecAttribute.size() + nMax;
	m_pImpl->vecAttribute.reserve( nTotalSize );

	for( sal_Int32 i = 0 ; i < nMax ; i ++ ) {
		m_pImpl->vecAttribute.push_back( SvXMLTagAttribute_Impl(
			r->getNameByIndex( i ) ,
			r->getValueByIndex( i )));
	}

	OSL_ASSERT( nTotalSize == getLength());
}

// XUnoTunnel & co
const uno::Sequence< sal_Int8 > & SvXMLAttributeList::getUnoTunnelId() throw()
{
	static uno::Sequence< sal_Int8 > * pSeq = 0;
	if( !pSeq )
	{
		Guard< Mutex > aGuard( Mutex::getGlobalMutex() );
		if( !pSeq )
		{
			static uno::Sequence< sal_Int8 > aSeq( 16 );
			rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
			pSeq = &aSeq;
		}
	}
	return *pSeq;
}

SvXMLAttributeList* SvXMLAttributeList::getImplementation( uno::Reference< uno::XInterface > xInt ) throw()
{
	uno::Reference< lang::XUnoTunnel > xUT( xInt, uno::UNO_QUERY );
	if( xUT.is() )
		return (SvXMLAttributeList*)xUT->getSomething( SvXMLAttributeList::getUnoTunnelId() );
	else
		return NULL;
}

// XUnoTunnel
sal_Int64 SAL_CALL SvXMLAttributeList::getSomething( const uno::Sequence< sal_Int8 >& rId )
	throw( uno::RuntimeException )
{
	if( rId.getLength() == 16 && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
														 rId.getConstArray(), 16 ) )
	{
		return (sal_Int64)this;
	}
	return 0;
}


}//end of namespace binfilter
