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


#ifndef _COM_SUN_STAR_SCRIPT_XTYPECONVERTER_HPP_
#include <com/sun/star/script/XTypeConverter.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#include <comphelper/processfactory.hxx>

#include <bf_svtools/ilstitem.hxx>

#define _SVSTDARR_ULONGS
#include <bf_svtools/svstdarr.hxx>

namespace binfilter
{

TYPEINIT1_AUTOFACTORY(SfxIntegerListItem, SfxPoolItem);

SfxIntegerListItem::SfxIntegerListItem()
{
}

SfxIntegerListItem::SfxIntegerListItem( const SfxIntegerListItem& rItem )
    : SfxPoolItem( rItem )
{
    m_aList = rItem.m_aList;
}

SfxIntegerListItem::~SfxIntegerListItem()
{
}

int SfxIntegerListItem::operator==( const SfxPoolItem& rPoolItem ) const
{
    if ( !rPoolItem.ISA( SfxIntegerListItem ) )
        return FALSE;

    const SfxIntegerListItem rItem = (const SfxIntegerListItem&) rPoolItem;
    return rItem.m_aList == m_aList;
}

SfxPoolItem* SfxIntegerListItem::Clone( SfxItemPool * ) const
{
    return new SfxIntegerListItem( *this );
}

BOOL SfxIntegerListItem::PutValue  ( const com::sun::star::uno::Any& rVal, BYTE )
{
    ::com::sun::star::uno::Reference < ::com::sun::star::script::XTypeConverter > xConverter
            ( ::comphelper::getProcessServiceFactory()->createInstance(::rtl::OUString::createFromAscii("com.sun.star.script.Converter")),
            ::com::sun::star::uno::UNO_QUERY );
    ::com::sun::star::uno::Any aNew;
    try { aNew = xConverter->convertTo( rVal, ::getCppuType((const ::com::sun::star::uno::Sequence < sal_Int32 >*)0) ); }
    catch (::com::sun::star::uno::Exception&)
    {
        return FALSE;
    }

    return ( aNew >>= m_aList );
}

BOOL SfxIntegerListItem::QueryValue( com::sun::star::uno::Any& rVal, BYTE ) const
{
    rVal <<= m_aList;
    return TRUE;
}

}
