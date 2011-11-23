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


#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XTYPECONVERTER_HPP_
#include <com/sun/star/script/XTypeConverter.hpp>
#endif

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <comphelper/processfactory.hxx>

#include <bf_svtools/globalnameitem.hxx>

// STATIC DATA -----------------------------------------------------------

namespace binfilter
{


// -----------------------------------------------------------------------

TYPEINIT1_AUTOFACTORY(SfxGlobalNameItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxGlobalNameItem::SfxGlobalNameItem()
{
}

// -----------------------------------------------------------------------

SfxGlobalNameItem::~SfxGlobalNameItem()
{
}

// -----------------------------------------------------------------------

int SfxGlobalNameItem::operator==( const SfxPoolItem& rItem ) const
{
	return ((SfxGlobalNameItem&)rItem).m_aName == m_aName;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxGlobalNameItem::Clone(SfxItemPool *) const
{
	return new SfxGlobalNameItem( *this );
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxGlobalNameItem::PutValue( const com::sun::star::uno::Any& rVal, BYTE )
{
    com::sun::star::uno::Reference < com::sun::star::script::XTypeConverter > xConverter
            ( ::comphelper::getProcessServiceFactory()->createInstance(::rtl::OUString::createFromAscii("com.sun.star.script.Converter")),
            com::sun::star::uno::UNO_QUERY );
	com::sun::star::uno::Sequence< sal_Int8 > aSeq;
    com::sun::star::uno::Any aNew;

    try { aNew = xConverter->convertTo( rVal, ::getCppuType((const com::sun::star::uno::Sequence < sal_Int8 >*)0) ); }
    catch (com::sun::star::uno::Exception&) {}
	aNew >>= aSeq;
	if ( aSeq.getLength() == 16 )
	{
		m_aName.MakeFromMemory( (void*) aSeq.getConstArray() );
		return TRUE;
	}

	DBG_ERROR( "SfxGlobalNameItem::PutValue - Wrong type!" );
	return FALSE;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxGlobalNameItem::QueryValue( ::com::sun::star::uno::Any& rVal, BYTE ) const
{
	::com::sun::star::uno::Sequence< sal_Int8 > aSeq( 16 );
	void* pData = ( void* ) &m_aName.GetCLSID();
	memcpy( aSeq.getArray(), pData, 16 );
	rVal <<= aSeq;
	return TRUE;
}

}
