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
#include "precompiled_sfx2.hxx"
#include <vcl/msgbox.hxx>
#include <tools/resary.hxx>
#include <svl/lstner.hxx>
#include <basic/basmgr.hxx>
#include <basic/sbmod.hxx>
#include <tools/urlobj.hxx>
#include <basic/sbx.hxx>
#include <sot/storage.hxx>
#include <unotools/securityoptions.hxx>

#include <rtl/ustring.h>
#include <com/sun/star/uno/Any.hxx>
#include <framework/eventsconfiguration.hxx>
#include <comphelper/processfactory.hxx>
#include <sfx2/evntconf.hxx>

#include <sfx2/docfile.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/dispatch.hxx>
#include "sfx2/sfxresid.hxx"
#include "eventsupplier.hxx"

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>

// -----------------------------------------------------------------------
TYPEINIT1(SfxEventHint, SfxHint);
TYPEINIT1(SfxEventNamesItem, SfxPoolItem);
TYPEINIT1(SfxViewEventHint, SfxEventHint);

using namespace com::sun::star;

SfxEventNamesList& SfxEventNamesList::operator=( const SfxEventNamesList& rTbl )
{
	DelDtor();
	for (sal_uInt16 n=0; n<rTbl.Count(); n++ )
	{
		SfxEventName* pTmp = ((SfxEventNamesList&)rTbl).GetObject(n);
		SfxEventName *pNew = new SfxEventName( *pTmp );
		Insert( pNew, n );
	}
	return *this;
}

void SfxEventNamesList::DelDtor()
{
	SfxEventName* pTmp = First();
	while( pTmp )
	{
		delete pTmp;
		pTmp = Next();
	}
	Clear();
}

int SfxEventNamesItem::operator==( const SfxPoolItem& rAttr ) const
{
	DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

	const SfxEventNamesList& rOwn = aEventsList;
	const SfxEventNamesList& rOther = ( (SfxEventNamesItem&) rAttr ).aEventsList;

	if ( rOwn.Count() != rOther.Count() )
		return sal_False;

	for ( sal_uInt16 nNo = 0; nNo < rOwn.Count(); ++nNo )
	{
		const SfxEventName *pOwn = rOwn.GetObject(nNo);
		const SfxEventName *pOther = rOther.GetObject(nNo);
		if ( 	pOwn->mnId != pOther->mnId ||
				pOwn->maEventName != pOther->maEventName ||
				pOwn->maUIName != pOther->maUIName )
			return sal_False;
	}

	return sal_True;

}

SfxItemPresentation SfxEventNamesItem::GetPresentation( SfxItemPresentation,
									SfxMapUnit,
									SfxMapUnit,
									XubString &rText,
                                    const IntlWrapper* ) const
{
	rText.Erase();
	return SFX_ITEM_PRESENTATION_NONE;
}

SfxPoolItem* SfxEventNamesItem::Clone( SfxItemPool *) const
{
	return new SfxEventNamesItem(*this);
}

SfxPoolItem* SfxEventNamesItem::Create(SvStream &, sal_uInt16) const
{
	DBG_ERROR("not streamable!");
	return new SfxEventNamesItem(Which());
}

SvStream& SfxEventNamesItem::Store(SvStream &rStream, sal_uInt16 ) const
{
	DBG_ERROR("not streamable!");
	return rStream;
}

sal_uInt16 SfxEventNamesItem::GetVersion( sal_uInt16 ) const
{
	DBG_ERROR("not streamable!");
	return 0;
}

void SfxEventNamesItem::AddEvent( const String& rName, const String& rUIName, sal_uInt16 nID )
{
	aEventsList.Insert( new SfxEventName( nID, rName, rUIName.Len() ? rUIName : rName ) );
}


//==========================================================================

//--------------------------------------------------------------------------
uno::Any CreateEventData_Impl( const SvxMacro *pMacro )
{
/*
    This function converts a SvxMacro into an Any containing three
    properties. These properties are EventType and Script. Possible
    values for EventType are StarBasic, JavaScript, ...
    The Script property should contain the URL to the macro and looks
    like "macro://./standard.module1.main()"

    If pMacro is NULL, we return an empty property sequence, so PropagateEvent_Impl
    can delete an event binding.
*/
	uno::Any aEventData;

    if ( pMacro )
    {
        if ( pMacro->GetScriptType() == STARBASIC )
        {
			uno::Sequence < beans::PropertyValue > aProperties(3);
            beans::PropertyValue *pValues = aProperties.getArray();

			::rtl::OUString aType = ::rtl::OUString::createFromAscii( STAR_BASIC );;
            ::rtl::OUString aLib  = pMacro->GetLibName();
            ::rtl::OUString aMacro = pMacro->GetMacName();

            pValues[ 0 ].Name = ::rtl::OUString::createFromAscii( PROP_EVENT_TYPE );
            pValues[ 0 ].Value <<= aType;

            pValues[ 1 ].Name = ::rtl::OUString::createFromAscii( PROP_LIBRARY );
            pValues[ 1 ].Value <<= aLib;

            pValues[ 2 ].Name = ::rtl::OUString::createFromAscii( PROP_MACRO_NAME );
            pValues[ 2 ].Value <<= aMacro;

            aEventData <<= aProperties;
        }
        else if ( pMacro->GetScriptType() == EXTENDED_STYPE )
        {
            uno::Sequence < beans::PropertyValue > aProperties(2);
            beans::PropertyValue *pValues = aProperties.getArray();

            ::rtl::OUString aLib   = pMacro->GetLibName();
            ::rtl::OUString aMacro = pMacro->GetMacName();

            pValues[ 0 ].Name = ::rtl::OUString::createFromAscii( PROP_EVENT_TYPE );
            pValues[ 0 ].Value <<= aLib;

            pValues[ 1 ].Name = ::rtl::OUString::createFromAscii( PROP_SCRIPT );
            pValues[ 1 ].Value <<= aMacro;

            aEventData <<= aProperties;
		}
        else if ( pMacro->GetScriptType() == JAVASCRIPT )
        {
            uno::Sequence < beans::PropertyValue > aProperties(2);
            beans::PropertyValue *pValues = aProperties.getArray();

            ::rtl::OUString aMacro  = pMacro->GetMacName();

            pValues[ 0 ].Name = ::rtl::OUString::createFromAscii( PROP_EVENT_TYPE );
            pValues[ 0 ].Value <<= ::rtl::OUString::createFromAscii(SVX_MACRO_LANGUAGE_JAVASCRIPT);

            pValues[ 1 ].Name = ::rtl::OUString::createFromAscii( PROP_MACRO_NAME );
            pValues[ 1 ].Value <<= aMacro;

            aEventData <<= aProperties;
		}
        else
        {
            DBG_ERRORFILE( "CreateEventData_Impl(): ScriptType not supported!");
        }
    }
    else
    {
        uno::Sequence < beans::PropertyValue > aProperties;
        aEventData <<= aProperties;
    }

    return aEventData;
}

//--------------------------------------------------------------------------
void PropagateEvent_Impl( SfxObjectShell *pDoc, rtl::OUString aEventName, const SvxMacro* pMacro )
{
	uno::Reference < document::XEventsSupplier > xSupplier;
	if ( pDoc )
	{
		xSupplier = uno::Reference < document::XEventsSupplier >( pDoc->GetModel(), uno::UNO_QUERY );
	}
	else
	{
		xSupplier = uno::Reference < document::XEventsSupplier >
                ( ::comphelper::getProcessServiceFactory()->createInstance(
				rtl::OUString::createFromAscii("com.sun.star.frame.GlobalEventBroadcaster" )), uno::UNO_QUERY );
	}

    if ( xSupplier.is() )
    {
		uno::Reference < container::XNameReplace > xEvents = xSupplier->getEvents();
        if ( aEventName.getLength() )
        {
			uno::Any aEventData = CreateEventData_Impl( pMacro );

            try
            {
                xEvents->replaceByName( aEventName, aEventData );
            }
            catch( ::com::sun::star::lang::IllegalArgumentException )
            { DBG_ERRORFILE( "PropagateEvents_Impl: caught IllegalArgumentException" ); }
            catch( ::com::sun::star::container::NoSuchElementException )
            { DBG_ERRORFILE( "PropagateEvents_Impl: caught NoSuchElementException" ); }
        }
        else {
            DBG_WARNING( "PropagateEvents_Impl: Got unknown event" );
        }
    }
}

//--------------------------------------------------------------------------------------------------------
void SfxEventConfiguration::ConfigureEvent( rtl::OUString aName, const SvxMacro& rMacro, SfxObjectShell *pDoc )
{
    SvxMacro *pMacro = NULL;
    if ( rMacro.GetMacName().Len() )
        pMacro = new SvxMacro( rMacro.GetMacName(), rMacro.GetLibName(), rMacro.GetScriptType() );
    if ( pDoc )
    {
        PropagateEvent_Impl( pDoc, aName, pMacro );
    }
    else
    {
        PropagateEvent_Impl( NULL, aName, pMacro );
    }
}

// -------------------------------------------------------------------------------------------------------
SvxMacro* SfxEventConfiguration::ConvertToMacro( const com::sun::star::uno::Any& rElement, SfxObjectShell* pDoc, sal_Bool bBlowUp )
{
	return SfxEvents_Impl::ConvertToMacro( rElement, pDoc, bBlowUp );
}
