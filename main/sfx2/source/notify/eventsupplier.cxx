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

//--------------------------------------------------------------------------------------------------------
#include <com/sun/star/beans/PropertyValue.hpp>

#ifndef  _COM_SUN_STAR_UTL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif

#ifndef  _COM_SUN_STAR_UTL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#include <tools/urlobj.hxx>
#include <tools/diagnose_ex.h>
#include <svl/macitem.hxx>
#include <sfx2/appuno.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/sfxbasemodel.hxx>
#include <sfx2/evntconf.hxx>
#include <unotools/eventcfg.hxx>

#include <unotools/securityoptions.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include "eventsupplier.hxx"

#include <sfx2/app.hxx>
#include "sfx2/sfxresid.hxx"

#include <sfx2/sfxsids.hrc>
#include "sfxlocal.hrc"
#include <sfx2/docfile.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/frame.hxx>

//--------------------------------------------------------------------------------------------------------

#define MACRO_PRFIX			"macro://"
#define MACRO_POSTFIX		"()"

//--------------------------------------------------------------------------------------------------------

#define PROPERTYVALUE		::com::sun::star::beans::PropertyValue
#define	UNO_QUERY			::com::sun::star::uno::UNO_QUERY

namespace css = ::com::sun::star;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::beans::PropertyValue;

//--------------------------------------------------------------------------------------------------------
	//  --- XNameReplace ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::replaceByName( const OUSTRING & aName, const ANY & rElement )
								throw( ILLEGALARGUMENTEXCEPTION, NOSUCHELEMENTEXCEPTION,
									   WRAPPEDTARGETEXCEPTION, RUNTIMEEXCEPTION )
{
	::osl::MutexGuard aGuard( maMutex );

	// find the event in the list and replace the data
	long nCount	= maEventNames.getLength();
	for ( long i=0; i<nCount; i++ )
	{
		if ( maEventNames[i] == aName )
		{
			// check for correct type of the element
            if ( !::comphelper::NamedValueCollection::canExtractFrom( rElement ) )
				throw ILLEGALARGUMENTEXCEPTION();
            ::comphelper::NamedValueCollection const aEventDescriptor( rElement );

            // create Configuration at first, creation might call this method also and that would overwrite everything
			// we might have stored before!
            if ( mpObjShell && !mpObjShell->IsLoading() )
                mpObjShell->SetModified( sal_True );

            ::comphelper::NamedValueCollection aNormalizedDescriptor;
	        NormalizeMacro( aEventDescriptor, aNormalizedDescriptor, mpObjShell );

            ::rtl::OUString sType;
            if  (   ( aNormalizedDescriptor.size() == 1 )
                &&  ( aNormalizedDescriptor.has( PROP_EVENT_TYPE ) == 0 )
                &&  ( aNormalizedDescriptor.get( PROP_EVENT_TYPE ) >>= sType )
                &&  ( sType.getLength() == 0 )
                )
            {
                // An empty event type means no binding. Therefore reset data
                // to reflect that state.
                // (that's for compatibility only. Nowadays, the Tools/Customize dialog should
                // set an empty sequence to indicate the request for resetting the assignment.)
                OSL_ENSURE( false, "legacy event assignment format detected" );
                aNormalizedDescriptor.clear();
            }

            if ( !aNormalizedDescriptor.empty() )
            {
                maEventData[i] <<= aNormalizedDescriptor.getPropertyValues();
            }
            else
            {
                maEventData[i].clear();
            }
			return;
		}
	}

	throw NOSUCHELEMENTEXCEPTION();
}

//--------------------------------------------------------------------------------------------------------
//  --- XNameAccess ---
//--------------------------------------------------------------------------------------------------------
ANY SAL_CALL SfxEvents_Impl::getByName( const OUSTRING& aName )
								throw( NOSUCHELEMENTEXCEPTION, WRAPPEDTARGETEXCEPTION,
									   RUNTIMEEXCEPTION )
{
	::osl::MutexGuard aGuard( maMutex );

	// find the event in the list and return the data

	long nCount	= maEventNames.getLength();

	for ( long i=0; i<nCount; i++ )
	{
		if ( maEventNames[i] == aName )
			return maEventData[i];
	}

	throw NOSUCHELEMENTEXCEPTION();
}

//--------------------------------------------------------------------------------------------------------
SEQUENCE< OUSTRING > SAL_CALL SfxEvents_Impl::getElementNames() throw ( RUNTIMEEXCEPTION )
{
	return maEventNames;
}

//--------------------------------------------------------------------------------------------------------
sal_Bool SAL_CALL SfxEvents_Impl::hasByName( const OUSTRING& aName ) throw ( RUNTIMEEXCEPTION )
{
	::osl::MutexGuard aGuard( maMutex );

	// find the event in the list and return the data

	long nCount	= maEventNames.getLength();

	for ( long i=0; i<nCount; i++ )
	{
		if ( maEventNames[i] == aName )
			return sal_True;
	}

	return sal_False;
}

//--------------------------------------------------------------------------------------------------------
//  --- XElementAccess ( parent of XNameAccess ) ---
//--------------------------------------------------------------------------------------------------------
UNOTYPE SAL_CALL SfxEvents_Impl::getElementType() throw ( RUNTIMEEXCEPTION )
{
	UNOTYPE aElementType = ::getCppuType( (const SEQUENCE < PROPERTYVALUE > *)0 );
	return aElementType;
}

//--------------------------------------------------------------------------------------------------------
sal_Bool SAL_CALL SfxEvents_Impl::hasElements() throw ( RUNTIMEEXCEPTION )
{
	::osl::MutexGuard aGuard( maMutex );

	if ( maEventNames.getLength() )
		return sal_True;
	else
		return sal_False;
}

static void Execute( ANY& aEventData, const css::document::DocumentEvent& aTrigger, SfxObjectShell* pDoc )
{
	SEQUENCE < PROPERTYVALUE > aProperties;
	if ( aEventData >>= aProperties )
	{
        OUSTRING        aPrefix = OUSTRING( RTL_CONSTASCII_USTRINGPARAM( MACRO_PRFIX ) );
		OUSTRING		aType;
		OUSTRING		aScript;
		OUSTRING		aLibrary;
		OUSTRING		aMacroName;

        sal_Int32 nCount = aProperties.getLength();

		if ( !nCount )
			return;

        sal_Int32 nIndex = 0;
		while ( nIndex < nCount )
		{
			if ( aProperties[ nIndex ].Name.compareToAscii( PROP_EVENT_TYPE ) == 0 )
				aProperties[ nIndex ].Value >>= aType;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_SCRIPT ) == 0 )
				aProperties[ nIndex ].Value >>= aScript;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_LIBRARY ) == 0 )
				aProperties[ nIndex ].Value >>= aLibrary;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_MACRO_NAME ) == 0 )
				aProperties[ nIndex ].Value >>= aMacroName;
			else {
				DBG_ERROR("Unknown property value!");
            }
			nIndex += 1;
		}

		if ( aType.compareToAscii( STAR_BASIC ) == 0 && aScript.getLength() )
		{
			com::sun::star::uno::Any aAny;
            SfxMacroLoader::loadMacro( aScript, aAny, pDoc );
		}
		else if ( aType.compareToAscii( "Service" ) == 0 ||
                  aType.compareToAscii( "Script" ) == 0 )
		{
			if ( aScript.getLength() )
			{
                SfxViewFrame* pView = pDoc ?
                    SfxViewFrame::GetFirst( pDoc ) :
					SfxViewFrame::Current();

				::com::sun::star::uno::Reference
					< ::com::sun::star::util::XURLTransformer > xTrans(
						::comphelper::getProcessServiceFactory()->createInstance(
							rtl::OUString::createFromAscii(
								"com.sun.star.util.URLTransformer" ) ),
						UNO_QUERY );

				::com::sun::star::util::URL aURL;
				aURL.Complete = aScript;
				xTrans->parseStrict( aURL );

				if ( aURL.Protocol.equals( ::rtl::OUString::createFromAscii( "vnd.sun.star.script:" ) ) )
				{
					::com::sun::star::uno::Reference
						< ::com::sun::star::frame::XDispatchProvider > xProv;

					if ( pView != NULL )
					{
						xProv = ::com::sun::star::uno::Reference
							< ::com::sun::star::frame::XDispatchProvider > (
								pView->GetFrame().GetFrameInterface(), UNO_QUERY );
					}
					else
					{
						xProv = ::com::sun::star::uno::Reference
							< ::com::sun::star::frame::XDispatchProvider > (
								::comphelper::getProcessServiceFactory()->createInstance(
									rtl::OUString::createFromAscii(
										"com.sun.star.frame.Desktop" ) ),
								UNO_QUERY );
					}

					::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatch > xDisp;
					if ( xProv.is() )
						xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );

					if ( xDisp.is() )
					{
						//::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue > aArgs(1);
						//aArgs[0].Name = rtl::OUString::createFromAscii("Referer");
						//aArs[0].Value <<= ::rtl::OUString( pDoc->GetMedium()->GetName() );
						//xDisp->dispatch( aURL, aArgs );

						css::beans::PropertyValue aEventParam;
						aEventParam.Value <<= aTrigger;
						css::uno::Sequence< css::beans::PropertyValue > aDispatchArgs( &aEventParam, 1 );
						xDisp->dispatch( aURL, aDispatchArgs );
					}
				}
			}
		}
        else if ( aType.getLength() == 0 )
        {
            // Empty type means no active binding for the event. Just ignore do nothing.
        }
        else
		{
			DBG_ERRORFILE( "notifyEvent(): Unsupported event type" );
		}
	}
}

//--------------------------------------------------------------------------------------------------------
// --- ::document::XEventListener ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::notifyEvent( const DOCEVENTOBJECT& aEvent ) throw( RUNTIMEEXCEPTION )
{
	::osl::ClearableMutexGuard aGuard( maMutex );

	// get the event name, find the coresponding data, execute the data

	OUSTRING	aName	= aEvent.EventName;
	long		nCount	= maEventNames.getLength();
	long		nIndex	= 0;
	sal_Bool	bFound	= sal_False;

	while ( !bFound && ( nIndex < nCount ) )
	{
		if ( maEventNames[nIndex] == aName )
			bFound = sal_True;
		else
			nIndex += 1;
	}

	if ( !bFound )
		return;

	ANY	aEventData = maEventData[ nIndex ];
    aGuard.clear();
    Execute( aEventData, css::document::DocumentEvent(aEvent.Source, aEvent.EventName, NULL, css::uno::Any()), mpObjShell );
}

//--------------------------------------------------------------------------------------------------------
// --- ::lang::XEventListener ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::disposing( const EVENTOBJECT& /*Source*/ ) throw( RUNTIMEEXCEPTION )
{
	::osl::MutexGuard aGuard( maMutex );

	if ( mxBroadcaster.is() )
	{
		mxBroadcaster->removeEventListener( this );
		mxBroadcaster = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------
SfxEvents_Impl::SfxEvents_Impl( SfxObjectShell* pShell,
							    REFERENCE< XEVENTBROADCASTER > xBroadcaster )
{
	// get the list of supported events and store it
	if ( pShell )
		maEventNames = pShell->GetEventNames();
	else
		maEventNames = GlobalEventConfig().getElementNames();

	maEventData = SEQUENCE < ANY > ( maEventNames.getLength() );

	mpObjShell		= pShell;
	mxBroadcaster	= xBroadcaster;

	if ( mxBroadcaster.is() )
		mxBroadcaster->addEventListener( this );
}

//--------------------------------------------------------------------------------------------------------
SfxEvents_Impl::~SfxEvents_Impl()
{
}

//--------------------------------------------------------------------------------------------------------
SvxMacro* SfxEvents_Impl::ConvertToMacro( const ANY& rElement, SfxObjectShell* pObjShell, sal_Bool bNormalizeMacro )
{
	SvxMacro* pMacro = NULL;
	SEQUENCE < PROPERTYVALUE > aProperties;
	ANY aAny;
	if ( bNormalizeMacro )
		NormalizeMacro( rElement, aAny, pObjShell );
	else
		aAny = rElement;

	if ( aAny >>= aProperties )
	{
		OUSTRING		aType;
		OUSTRING		aScriptURL;
		OUSTRING		aLibrary;
		OUSTRING		aMacroName;

		long nCount = aProperties.getLength();
		long nIndex = 0;

		if ( !nCount )
			return pMacro;

		while ( nIndex < nCount )
		{
			if ( aProperties[ nIndex ].Name.compareToAscii( PROP_EVENT_TYPE ) == 0 )
				aProperties[ nIndex ].Value >>= aType;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_SCRIPT ) == 0 )
				aProperties[ nIndex ].Value >>= aScriptURL;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_LIBRARY ) == 0 )
				aProperties[ nIndex ].Value >>= aLibrary;
			else if ( aProperties[ nIndex ].Name.compareToAscii( PROP_MACRO_NAME ) == 0 )
				aProperties[ nIndex ].Value >>= aMacroName;
			else {
				DBG_ERROR("Unknown propery value!");
            }
			nIndex += 1;
		}

		// Get the type
		ScriptType	eType( STARBASIC );
		if ( aType.compareToAscii( STAR_BASIC ) == COMPARE_EQUAL )
			eType = STARBASIC;
		else if ( aType.compareToAscii( "Script" ) == COMPARE_EQUAL && aScriptURL.getLength() )
			eType = EXTENDED_STYPE;
		else if ( aType.compareToAscii( SVX_MACRO_LANGUAGE_JAVASCRIPT ) == COMPARE_EQUAL )
			eType = JAVASCRIPT;
		else {
			DBG_ERRORFILE( "ConvertToMacro: Unknown macro type" );
        }

		if ( aMacroName.getLength() )
		{
			if ( aLibrary.compareToAscii("application") == 0 )
				aLibrary = SFX_APP()->GetName();
			else
				aLibrary = ::rtl::OUString();
			pMacro = new SvxMacro( aMacroName, aLibrary, eType );
		}
		else if ( eType == EXTENDED_STYPE )
			pMacro = new SvxMacro( aScriptURL, aType );
	}

	return pMacro;
}

void SfxEvents_Impl::NormalizeMacro( const ANY& rEvent, ANY& rRet, SfxObjectShell* pDoc )
{
    const ::comphelper::NamedValueCollection aEventDescriptor( rEvent );
    ::comphelper::NamedValueCollection aEventDescriptorOut;

    NormalizeMacro( aEventDescriptor, aEventDescriptorOut, pDoc );

    rRet <<= aEventDescriptorOut.getPropertyValues();
}

void SfxEvents_Impl::NormalizeMacro( const ::comphelper::NamedValueCollection& i_eventDescriptor,
        ::comphelper::NamedValueCollection& o_normalizedDescriptor, SfxObjectShell* i_document )
{
    SfxObjectShell* pDoc = i_document;
	if ( !pDoc )
		pDoc = SfxObjectShell::Current();

    ::rtl::OUString aType = i_eventDescriptor.getOrDefault( PROP_EVENT_TYPE, ::rtl::OUString() );
	::rtl::OUString aScript = i_eventDescriptor.getOrDefault( PROP_SCRIPT, ::rtl::OUString() );
	::rtl::OUString aLibrary = i_eventDescriptor.getOrDefault( PROP_LIBRARY, ::rtl::OUString() );
	::rtl::OUString aMacroName = i_eventDescriptor.getOrDefault( PROP_MACRO_NAME, ::rtl::OUString() );

    if ( aType.getLength() )
        o_normalizedDescriptor.put( PROP_EVENT_TYPE, aType );
    if ( aScript.getLength() )
        o_normalizedDescriptor.put( PROP_SCRIPT, aScript );

	if ( aType.compareToAscii( STAR_BASIC ) == 0 )
	{
		if ( aScript.getLength() )
		{
			if ( !aMacroName.getLength() || !aLibrary.getLength() )
			{
				sal_Int32 nHashPos = aScript.indexOf( '/', 8 );
				sal_Int32 nArgsPos = aScript.indexOf( '(' );
				if ( ( nHashPos != STRING_NOTFOUND ) && ( nHashPos < nArgsPos ) )
				{
					OUSTRING aBasMgrName( INetURLObject::decode( aScript.copy( 8, nHashPos-8 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET ) );
					if ( aBasMgrName.compareToAscii(".") == 0 )
						aLibrary = pDoc->GetTitle();
/*
					else if ( aBasMgrName.getLength() )
						aLibrary = aBasMgrName;
 */
					else
						aLibrary = SFX_APP()->GetName();

					// Get the macro name
					aMacroName = aScript.copy( nHashPos+1, nArgsPos - nHashPos - 1 );
				}
				else
				{
					DBG_ERRORFILE( "ConvertToMacro: Unknown macro url format" );
				}
			}
		}
		else if ( aMacroName.getLength() )
		{
            aScript = OUSTRING( RTL_CONSTASCII_USTRINGPARAM( MACRO_PRFIX ) );
			if ( aLibrary.compareTo( SFX_APP()->GetName() ) != 0 && aLibrary.compareToAscii("StarDesktop") != 0 && aLibrary.compareToAscii("application") != 0 )
				aScript += String('.');

			aScript += String('/');
			aScript += aMacroName;
			aScript += OUSTRING( RTL_CONSTASCII_USTRINGPARAM( MACRO_POSTFIX ) );
		}
		else
			// wrong properties
			return;

		if ( aLibrary.compareToAscii("document") != 0 )
		{
			if ( !aLibrary.getLength() || (pDoc && ( String(aLibrary) == pDoc->GetTitle( SFX_TITLE_APINAME ) || String(aLibrary) == pDoc->GetTitle() )) )
				aLibrary = String::CreateFromAscii("document");
			else
				aLibrary = String::CreateFromAscii("application");
		}

        o_normalizedDescriptor.put( PROP_SCRIPT, aScript );
        o_normalizedDescriptor.put( PROP_LIBRARY, aLibrary );
        o_normalizedDescriptor.put( PROP_MACRO_NAME, aMacroName );
    }
}

ModelCollectionEnumeration::ModelCollectionEnumeration(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ModelCollectionMutexBase(                 )
    , m_xSMGR                 (xSMGR            )
    , m_pEnumerationIt        (m_lModels.begin())
{
}

ModelCollectionEnumeration::~ModelCollectionEnumeration()
{
}

void ModelCollectionEnumeration::setModelList(const TModelList& rList)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    m_lModels        = rList;
    m_pEnumerationIt = m_lModels.begin();
    aLock.clear();
    // <- SAFE
}

sal_Bool SAL_CALL ModelCollectionEnumeration::hasMoreElements()
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    return (m_pEnumerationIt != m_lModels.end());
    // <- SAFE
}

css::uno::Any SAL_CALL ModelCollectionEnumeration::nextElement()
    throw(css::container::NoSuchElementException,
          css::lang::WrappedTargetException     ,
          css::uno::RuntimeException            )
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    if (m_pEnumerationIt == m_lModels.end())
        throw css::container::NoSuchElementException(
                    ::rtl::OUString::createFromAscii("End of model enumeration reached."),
                    static_cast< css::container::XEnumeration* >(this));
    css::uno::Reference< css::frame::XModel > xModel(*m_pEnumerationIt, UNO_QUERY);
    ++m_pEnumerationIt;
    aLock.clear();
    // <- SAFE

    return css::uno::makeAny(xModel);
}

SFX_IMPL_XSERVICEINFO( SfxGlobalEvents_Impl, "com.sun.star.frame.GlobalEventBroadcaster", "com.sun.star.comp.sfx2.GlobalEventBroadcaster" )
SFX_IMPL_ONEINSTANCEFACTORY( SfxGlobalEvents_Impl );

//-----------------------------------------------------------------------------
SfxGlobalEvents_Impl::SfxGlobalEvents_Impl( const com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xSMGR)
    : ModelCollectionMutexBase(       )
    , m_xSMGR                 (xSMGR  )
	, m_aLegacyListeners      (m_aLock)
    , m_aDocumentListeners    (m_aLock)
    , pImp                    (0      )
{
	m_refCount++;
    SFX_APP();
    pImp                   = new GlobalEventConfig();
	m_xEvents              = pImp;
    m_xJobExecutorListener = css::uno::Reference< css::document::XEventListener >(
                        xSMGR->createInstance(::rtl::OUString::createFromAscii("com.sun.star.task.JobExecutor")),
                        UNO_QUERY);
	m_refCount--;
}

//-----------------------------------------------------------------------------
SfxGlobalEvents_Impl::~SfxGlobalEvents_Impl()
{
}

//-----------------------------------------------------------------------------
css::uno::Reference< css::container::XNameReplace > SAL_CALL SfxGlobalEvents_Impl::getEvents()
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
	return m_xEvents;
    // <- SAFE
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::addEventListener(const css::uno::Reference< css::document::XEventListener >& xListener)
    throw(css::uno::RuntimeException)
{
    // container is threadsafe
	m_aLegacyListeners.addInterface(xListener);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::removeEventListener(const css::uno::Reference< css::document::XEventListener >& xListener)
    throw(css::uno::RuntimeException)
{
    // container is threadsafe
	m_aLegacyListeners.removeInterface(xListener);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::addDocumentEventListener( const css::uno::Reference< css::document::XDocumentEventListener >& _Listener )
    throw(css::uno::RuntimeException)
{
    m_aDocumentListeners.addInterface( _Listener );
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::removeDocumentEventListener( const css::uno::Reference< css::document::XDocumentEventListener >& _Listener )
    throw(css::uno::RuntimeException)
{
    m_aDocumentListeners.removeInterface( _Listener );
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::notifyDocumentEvent( const ::rtl::OUString& /*_EventName*/,
        const css::uno::Reference< css::frame::XController2 >& /*_ViewController*/, const css::uno::Any& /*_Supplement*/ )
        throw (css::lang::IllegalArgumentException, css::lang::NoSupportException, css::uno::RuntimeException)
{
    // we're a multiplexer only, no chance to generate artificial events here
    throw css::lang::NoSupportException(::rtl::OUString(), *this);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::notifyEvent(const css::document::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    css::document::DocumentEvent aDocEvent(aEvent.Source, aEvent.EventName, NULL, css::uno::Any());
    implts_notifyJobExecution(aEvent);
    implts_checkAndExecuteEventBindings(aDocEvent);
    implts_notifyListener(aDocEvent);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::documentEventOccured( const ::css::document::DocumentEvent& _Event )
    throw (::css::uno::RuntimeException)
{
    implts_notifyJobExecution(css::document::EventObject(_Event.Source, _Event.EventName));
    implts_checkAndExecuteEventBindings(_Event);
    implts_notifyListener(_Event);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XModel > xDoc(aEvent.Source, UNO_QUERY);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        m_lModels.erase(pIt);
    aLock.clear();
    // <- SAFE
}

//-----------------------------------------------------------------------------
sal_Bool SAL_CALL SfxGlobalEvents_Impl::has(const css::uno::Any& aElement)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;

    sal_Bool bHas = sal_False;

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        bHas = sal_True;
    aLock.clear();
    // <- SAFE

    return bHas;
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::insert( const css::uno::Any& aElement )
    throw (css::lang::IllegalArgumentException  ,
           css::container::ElementExistException,
           css::uno::RuntimeException           )
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;
    if (!xDoc.is())
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString::createFromAscii("Can't locate at least the model parameter."),
                static_cast< css::container::XSet* >(this),
                0);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        throw css::container::ElementExistException(
                ::rtl::OUString(),
                static_cast< css::container::XSet* >(this));
    m_lModels.push_back(xDoc);
    aLock.clear();
    // <- SAFE

    css::uno::Reference< css::document::XDocumentEventBroadcaster > xDocBroadcaster(xDoc, UNO_QUERY );
    if (xDocBroadcaster.is())
        xDocBroadcaster->addDocumentEventListener(this);
    else
    {
        // try the "legacy version" of XDocumentEventBroadcaster, which is XEventBroadcaster
        css::uno::Reference< css::document::XEventBroadcaster > xBroadcaster(xDoc, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->addEventListener(static_cast< css::document::XEventListener* >(this));
    }
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::remove( const css::uno::Any& aElement )
    throw (css::lang::IllegalArgumentException   ,
           css::container::NoSuchElementException,
           css::uno::RuntimeException            )
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;
    if (!xDoc.is())
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString::createFromAscii("Can't locate at least the model parameter."),
                static_cast< css::container::XSet* >(this),
                0);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt == m_lModels.end())
        throw css::container::NoSuchElementException(
                ::rtl::OUString(),
                static_cast< css::container::XSet* >(this));
    m_lModels.erase(pIt);
    aLock.clear();
    // <- SAFE

    css::uno::Reference< css::document::XDocumentEventBroadcaster > xDocBroadcaster(xDoc, UNO_QUERY );
    if (xDocBroadcaster.is())
        xDocBroadcaster->removeDocumentEventListener(this);
    else
    {
        // try the "legacy version" of XDocumentEventBroadcaster, which is XEventBroadcaster
        css::uno::Reference< css::document::XEventBroadcaster > xBroadcaster(xDoc, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->removeEventListener(static_cast< css::document::XEventListener* >(this));
    }
}

//-----------------------------------------------------------------------------
css::uno::Reference< css::container::XEnumeration > SAL_CALL SfxGlobalEvents_Impl::createEnumeration()
    throw (css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    ModelCollectionEnumeration* pEnum = new ModelCollectionEnumeration(m_xSMGR);
    pEnum->setModelList(m_lModels);
    css::uno::Reference< css::container::XEnumeration > xEnum(
        static_cast< css::container::XEnumeration* >(pEnum),
        UNO_QUERY);
    aLock.clear();
    // <- SAFE

    return xEnum;
}

//-----------------------------------------------------------------------------
css::uno::Type SAL_CALL SfxGlobalEvents_Impl::getElementType()
    throw (css::uno::RuntimeException)
{
    return ::getCppuType(static_cast< css::uno::Reference< css::frame::XModel >* >(NULL));
}

//-----------------------------------------------------------------------------
sal_Bool SAL_CALL SfxGlobalEvents_Impl::hasElements()
    throw (css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    return (m_lModels.size()>0);
    // <- SAFE
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_notifyJobExecution(const css::document::EventObject& aEvent)
{
    try
    {
        // SAFE ->
        ::osl::ResettableMutexGuard aLock(m_aLock);
        css::uno::Reference< css::document::XEventListener > xJobExecutor(m_xJobExecutorListener);
        aLock.clear();
        // <- SAFE
        if (xJobExecutor.is())
            xJobExecutor->notifyEvent(aEvent);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        {}
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_checkAndExecuteEventBindings(const css::document::DocumentEvent& aEvent)
{
    try
    {
        // SAFE ->
        ::osl::ResettableMutexGuard aLock(m_aLock);
        css::uno::Reference< css::container::XNameReplace > xEvents = m_xEvents;
        aLock.clear();
        // <- SAFE

        css::uno::Any aAny;
        if ( xEvents.is() && xEvents->hasByName( aEvent.EventName ) )
            aAny = xEvents->getByName(aEvent.EventName);
        Execute(aAny, aEvent, 0);
    }
    catch ( css::uno::RuntimeException const & )
    {
        throw;
    }
    catch ( css::uno::Exception const & )
    {
       DBG_UNHANDLED_EXCEPTION();
    }
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_notifyListener(const css::document::DocumentEvent& aEvent)
{
    // containers are threadsafe
    css::document::EventObject aLegacyEvent(aEvent.Source, aEvent.EventName);
    m_aLegacyListeners.notifyEach( &css::document::XEventListener::notifyEvent, aLegacyEvent );

    m_aDocumentListeners.notifyEach( &css::document::XDocumentEventListener::documentEventOccured, aEvent );
}

//-----------------------------------------------------------------------------
// not threadsafe ... must be locked from outside!
TModelList::iterator SfxGlobalEvents_Impl::impl_searchDoc(const css::uno::Reference< css::frame::XModel >& xModel)
{
    if (!xModel.is())
        return m_lModels.end();

    TModelList::iterator pIt;
    for (  pIt  = m_lModels.begin();
           pIt != m_lModels.end()  ;
         ++pIt                     )
    {
        css::uno::Reference< css::frame::XModel > xContainerDoc(*pIt, UNO_QUERY);
        if (xContainerDoc == xModel)
            break;
    }

    return pIt;
}

