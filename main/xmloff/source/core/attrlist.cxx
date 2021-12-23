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
#include "precompiled_xmloff.hxx"

#include <vector>
#include <osl/mutex.hxx>
#include <xmloff/xmltoken.hxx>
#include <rtl/uuid.h>
#include <rtl/memory.h>

#include <xmloff/attrlist.hxx>

using ::rtl::OUString;

using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::xmloff::token;

struct SvXMLTagAttribute_Impl
{
	SvXMLTagAttribute_Impl(){}
	SvXMLTagAttribute_Impl( const OUString &rName,
						 const OUString &rValue )
		: sName(rName),
		sValue(rValue)
	{
	}

	SvXMLTagAttribute_Impl( const SvXMLTagAttribute_Impl& r ) :
		sName(r.sName),
		sValue(r.sValue)
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

	SvXMLAttributeList_Impl( const SvXMLAttributeList_Impl& r ) :
			vecAttribute( r.vecAttribute )
	{
	}

	::std::vector<struct SvXMLTagAttribute_Impl> vecAttribute;
	typedef ::std::vector<struct SvXMLTagAttribute_Impl>::size_type size_type;
};



sal_Int16 SAL_CALL SvXMLAttributeList::getLength(void) throw( ::com::sun::star::uno::RuntimeException )
{
	return sal::static_int_cast< sal_Int16 >(m_pImpl->vecAttribute.size());
}


SvXMLAttributeList::SvXMLAttributeList( const SvXMLAttributeList &r ) :
    cppu::WeakImplHelper3<com::sun::star::xml::sax::XAttributeList, com::sun::star::util::XCloneable, com::sun::star::lang::XUnoTunnel>(r),
	m_pImpl( new SvXMLAttributeList_Impl( *r.m_pImpl ) )
{
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
    return ( static_cast< SvXMLAttributeList_Impl::size_type >( i ) < m_pImpl->vecAttribute.size() ) ? m_pImpl->vecAttribute[i].sName : OUString();
}


OUString SAL_CALL SvXMLAttributeList::getTypeByIndex(sal_Int16) throw( ::com::sun::star::uno::RuntimeException )
{
    return sType;
}

OUString SAL_CALL  SvXMLAttributeList::getValueByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException )
{
    return ( static_cast< SvXMLAttributeList_Impl::size_type >( i ) < m_pImpl->vecAttribute.size() ) ? m_pImpl->vecAttribute[i].sValue : OUString();
}

OUString SAL_CALL SvXMLAttributeList::getTypeByName( const OUString& ) throw( ::com::sun::star::uno::RuntimeException )
{
    return sType;
}

OUString SAL_CALL SvXMLAttributeList::getValueByName(const OUString& sName) throw( ::com::sun::star::uno::RuntimeException )
{
	::std::vector<struct SvXMLTagAttribute_Impl>::iterator ii = m_pImpl->vecAttribute.begin();

	for( ; ii != m_pImpl->vecAttribute.end() ; ++ii ) {
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
    if (GetIndexByName(sName) == -1) {
		m_pImpl->vecAttribute.push_back( SvXMLTagAttribute_Impl( sName , sValue ) );
    } else {
        throw com::sun::star::uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("attempt to insert duplicate XML tag attribute: ")) + sName, NULL);
    }
}

void SvXMLAttributeList::Clear()
{
	m_pImpl->vecAttribute.clear();

	OSL_ASSERT( ! getLength() );
}

void SvXMLAttributeList::RemoveAttribute( const OUString sName )
{
	::std::vector<struct SvXMLTagAttribute_Impl>::iterator ii = m_pImpl->vecAttribute.begin();

	for( ; ii != m_pImpl->vecAttribute.end() ; ++ii ) {
		if( (*ii).sName == sName ) {
			m_pImpl->vecAttribute.erase( ii );
			break;
		}
	}
}


void SvXMLAttributeList::SetAttributeList( const uno::Reference< ::com::sun::star::xml::sax::XAttributeList >  &r )
{
	Clear();
	AppendAttributeList( r );
}

void SvXMLAttributeList::AppendAttributeList( const uno::Reference< ::com::sun::star::xml::sax::XAttributeList >  &r )
{
	OSL_ASSERT( r.is() );

	sal_Int16 nMax = r->getLength();
	SvXMLAttributeList_Impl::size_type nTotalSize = 
		m_pImpl->vecAttribute.size() + nMax;
	m_pImpl->vecAttribute.reserve( nTotalSize );

    OUString sName;
	for( sal_Int16 i = 0 ; i < nMax ; ++i ) {
        sName = r->getNameByIndex(i);
        if (GetIndexByName(sName) == -1) {
            m_pImpl->vecAttribute.push_back( SvXMLTagAttribute_Impl(
                sName ,
			    r->getValueByIndex( i )));
        } else {
            throw com::sun::star::uno::RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("attempt to insert duplicate XML tag attribute: ")) + sName, NULL);
        }
	}

	OSL_ASSERT( nTotalSize == (SvXMLAttributeList_Impl::size_type)getLength());
}

void SvXMLAttributeList::SetValueByIndex( sal_Int16 i, 
		const ::rtl::OUString& rValue )
{
	if( static_cast< SvXMLAttributeList_Impl::size_type >( i ) 
			< m_pImpl->vecAttribute.size() )
	{
		m_pImpl->vecAttribute[i].sValue = rValue;
	}
}

void SvXMLAttributeList::RemoveAttributeByIndex( sal_Int16 i )
{
    if( static_cast< SvXMLAttributeList_Impl::size_type >( i ) 
			< m_pImpl->vecAttribute.size() )
		m_pImpl->vecAttribute.erase( m_pImpl->vecAttribute.begin() + i );
}

void SvXMLAttributeList::RenameAttributeByIndex( sal_Int16 i, 
												 const OUString& rNewName )
{
	if( static_cast< SvXMLAttributeList_Impl::size_type >( i ) 
			< m_pImpl->vecAttribute.size() )
	{
		m_pImpl->vecAttribute[i].sName = rNewName;
	}
}

sal_Int16 SvXMLAttributeList::GetIndexByName( const OUString& rName ) const
{
	::std::vector<struct SvXMLTagAttribute_Impl>::iterator ii = 
		m_pImpl->vecAttribute.begin();
	
	for( sal_Int16 nIndex=0; ii!=m_pImpl->vecAttribute.end(); ++ii, ++nIndex ) 
	{
		if( (*ii).sName == rName )
		{
			return nIndex;
		}
	}
	return -1;
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
	{
		return 
			reinterpret_cast<SvXMLAttributeList*>(
				sal::static_int_cast<sal_IntPtr>(
					xUT->getSomething( SvXMLAttributeList::getUnoTunnelId())));
	}
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
		return sal::static_int_cast<sal_Int64>(reinterpret_cast<sal_uIntPtr>(this));
	}
	return 0;
}


