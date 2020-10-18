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
//	includes
//_________________________________________________________________________________________________________________

#include <deque>

#include <unotools/internaloptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>

#include <rtl/logfile.hxx>
#include "itemholder1.hxx"

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::utl					;
using namespace ::rtl					;
using namespace ::osl					;
using namespace ::std					;
using namespace ::com::sun::star::uno	;
using namespace ::com::sun::star::beans	;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define	ROOTNODE_INTERNAL					OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Internal"	))
#define	DEFAULT_SLOTCFG						sal_False
#define	DEFAULT_SENDCRASHMAIL				sal_False
#define DEFAULT_USEMAILUI                   sal_True
#define DEFAULT_CURRENTTEMPURL				OUString(RTL_CONSTASCII_USTRINGPARAM(""))

#define	FIXPROPERTYNAME_SLOTCFG				OUString(RTL_CONSTASCII_USTRINGPARAM("Slot"						))
#define	FIXPROPERTYNAME_SENDCRASHMAIL		OUString(RTL_CONSTASCII_USTRINGPARAM("SendCrashMail"			))
#define FIXPROPERTYNAME_USEMAILUI           OUString(RTL_CONSTASCII_USTRINGPARAM("UseMailUI"                ))
#define FIXPROPERTYNAME_CURRENTTEMPURL		OUString(RTL_CONSTASCII_USTRINGPARAM("CurrentTempURL"			))
//#define FIXPROPERTYNAME_REMOVEMENUENTRYCLOSE           OUString(RTL_CONSTASCII_USTRINGPARAM("RemoveMenuEntryClose"))
//#define FIXPROPERTYNAME_REMOVEMENUENTRYBACKTOWEBTOP    OUString(RTL_CONSTASCII_USTRINGPARAM("RemoveMenuEntryBackToWebtop"))
//#define FIXPROPERTYNAME_REMOVEMENUENTRYNEWWEBTOP       OUString(RTL_CONSTASCII_USTRINGPARAM("RemoveMenuEntryNewWebtop"))
//#define FIXPROPERTYNAME_REMOVEMENUENTRYLOGOUT          OUString(RTL_CONSTASCII_USTRINGPARAM("RemoveMenuEntryLogout"))

#define	FIXPROPERTYHANDLE_SLOTCFG			0
#define	FIXPROPERTYHANDLE_SENDCRASHMAIL		1
#define FIXPROPERTYHANDLE_USEMAILUI         2
#define FIXPROPERTYHANDLE_CURRENTTEMPURL	3
//#define FIXPROPERTYHANDLE_REMOVEMENUENTRYCLOSE   3
//#define FIXPROPERTYHANDLE_REMOVEMENUENTRYBACKTOWEBTOP         4
//#define FIXPROPERTYHANDLE_REMOVEMENUENTRYNEWWEBTOP         5
//#define FIXPROPERTYHANDLE_REMOVEMENUENTRYLOGOUT         6

#define FIXPROPERTYCOUNT                    4
/*
#define	PROPERTYNAME_RECOVERYLIST			OUString(RTL_CONSTASCII_USTRINGPARAM("RecoveryList"				))
#define	PROPERTYNAME_URL					OUString(RTL_CONSTASCII_USTRINGPARAM("OrgURL"					))
#define	PROPERTYNAME_FILTER					OUString(RTL_CONSTASCII_USTRINGPARAM("FilterName"  			    ))
#define	PROPERTYNAME_TEMPNAME				OUString(RTL_CONSTASCII_USTRINGPARAM("TempURL"					))

#define	OFFSET_URL							0
#define	OFFSET_FILTER						1
#define	OFFSET_TEMPNAME						2
*/
#define	PATHDELIMITER						OUString(RTL_CONSTASCII_USTRINGPARAM("/"						))
#define	FIXR								OUString(RTL_CONSTASCII_USTRINGPARAM("r"						))

//_________________________________________________________________________________________________________________
//	private declarations!
//_________________________________________________________________________________________________________________
/*
struct tIMPL_RecoveryEntry
{
	OUString	sURL		;
	OUString	sFilter		;
	OUString	sTempName	;

	tIMPL_RecoveryEntry()
	{
		sURL		=	OUString();
		sFilter		=	OUString();
		sTempName	=	OUString();
	}

	tIMPL_RecoveryEntry(	const	OUString&	sNewURL			,
							const	OUString&	sNewFilter		,
							const	OUString&	sNewTempName	)
	{
		sURL		=	sNewURL			;
		sFilter		=	sNewFilter		;
		sTempName	=	sNewTempName	;
	}
};

typedef deque< tIMPL_RecoveryEntry > tIMPL_RecoveryStack;
*/
class SvtInternalOptions_Impl : public ConfigItem
{
	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:

        sal_Bool                m_bRemoveMenuEntryClose;
        sal_Bool                m_bRemoveMenuEntryBackToWebtop;
        sal_Bool                m_bRemoveMenuEntryNewWebtop;
        sal_Bool                m_bRemoveMenuEntryLogout;
        sal_Bool                m_bSlotCFG          ;   /// cache "Slot" of Internal section
		sal_Bool				m_bSendCrashMail	;	/// cache "SendCrashMail" of Internal section
        sal_Bool                m_bUseMailUI;
		OUString				m_aCurrentTempURL	;
	//	tIMPL_RecoveryStack		m_aRecoveryList		;	/// cache "RecoveryList" of Internal section
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		 SvtInternalOptions_Impl();
		~SvtInternalOptions_Impl();

		//---------------------------------------------------------------------------------------------------------
		//	overloaded methods of baseclass
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		called for notify of configmanager
			@descr		These method is called from the ConfigManager before application ends or from the
			 			PropertyChangeListener if the sub tree broadcasts changes. You must update your
						internal values.

			@seealso	baseclass ConfigItem

			@param		"seqPropertyNames" is the list of properties which should be updated.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void Notify( const Sequence< OUString >& )
		{
			DBG_ASSERT( sal_False, "SvtInternalOptions::Notify()\nNot used yet ... but called!?\n" );
		}

		/*-****************************************************************************************************//**
			@short		write changes to configuration
			@descr		These method writes the changed values into the sub tree
						and should always called in our destructor to guarantee consistency of config data.

			@seealso	baseclass ConfigItem

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void Commit();

		//---------------------------------------------------------------------------------------------------------
		//	public interface
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		access method to get internal values
			@descr		This method gives us a chance to regulate access to our internal values.
						It's not used in the moment - but it's possible for the future!

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        sal_Bool    IsRemoveMenuEntryClose() const { return m_bRemoveMenuEntryClose; }
        sal_Bool    IsRemoveMenuEntryBackToWebtop() const { return m_bRemoveMenuEntryBackToWebtop; }
        sal_Bool    IsRemoveMenuEntryNewWebtop() const { return m_bRemoveMenuEntryNewWebtop; }
        sal_Bool    IsRemoveMenuEntryLogout() const { return m_bRemoveMenuEntryLogout; }
        sal_Bool    SlotCFGEnabled      () const { return m_bSlotCFG; }
        sal_Bool    CrashMailEnabled    () const { return m_bSendCrashMail; }
        sal_Bool    MailUIEnabled       () const { return m_bUseMailUI; }

		OUString	GetCurrentTempURL() const { return m_aCurrentTempURL; }
		void		SetCurrentTempURL( const OUString& aNewCurrentTempURL );
/*
		void		PushRecoveryItem	(	const	OUString&	sURL		,
											const	OUString&	sFilter		,
					 						const	OUString&	sTempName	);
		void		PopRecoveryItem		(			OUString&	sURL		,
													OUString&	sFilter		,
													OUString&	sTempName	);
		sal_Bool	IsRecoveryListEmpty	(									) const;
*/
	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		return list of fix key names of our configuration management which represent our module tree
			@descr		These methods return a static const list of key names. We need it to get needed values from our
						configuration management. We return well known key names only - because the "UserData" node
						is handled in a special way!

			@seealso	-

			@param		-
			@return		A list of needed configuration keys is returned.

			@onerror	-
		*//*-*****************************************************************************************************/

		Sequence< OUString > impl_GetPropertyNames();
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtInternalOptions_Impl::SvtInternalOptions_Impl()
	// Init baseclasses first
    :   ConfigItem          ( ROOTNODE_INTERNAL, CONFIG_MODE_IMMEDIATE_UPDATE )
	// Init member then.
    , m_bRemoveMenuEntryClose ( sal_False )
    , m_bRemoveMenuEntryBackToWebtop ( sal_False )
    , m_bRemoveMenuEntryNewWebtop ( sal_False )
    , m_bRemoveMenuEntryLogout ( sal_False )
	,	m_bSlotCFG			( DEFAULT_SLOTCFG			)
	,	m_bSendCrashMail	( DEFAULT_SENDCRASHMAIL		)
    ,   m_bUseMailUI        ( DEFAULT_USEMAILUI			)
	,	m_aCurrentTempURL	( DEFAULT_CURRENTTEMPURL	)
{
	// Use our list of configuration keys to get his values.
	// structure of internal section: (first 2 entries are fixed - all other are member of a set!)
	//		"Slot"
	//		"SendCrashMail"
	//		"RecoveryList/r1/URL"
	//		"RecoveryList/r1/Filter"
	//		"RecoveryList/r1/TempName"
	//		"RecoveryList/r2/URL"
	//		"RecoveryList/r2/Filter"
	//		"RecoveryList/r2/TempName"
	//		"RecoveryList/.."
	Sequence< OUString >	seqNames	= impl_GetPropertyNames()	;
	Sequence< Any >			seqValues	= GetProperties( seqNames )	;

	// Safe impossible cases.
	// We need values from ALL configuration keys.
	// Follow assignment use order of values in relation to our list of key names!
	DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtInternalOptions_Impl::SvtInternalOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Read fixed values first!
	DBG_ASSERT(!(seqValues[FIXPROPERTYHANDLE_SLOTCFG].getValueTypeClass()!=TypeClass_BOOLEAN)		, "SvtInternalOptions_Impl::SvtInternalOptions_Impl()\nWho has changed the value type of \"Office.Common\\Internal\\Slot\"?"			);
	DBG_ASSERT(!(seqValues[FIXPROPERTYHANDLE_SENDCRASHMAIL].getValueTypeClass()!=TypeClass_BOOLEAN)	, "SvtInternalOptions_Impl::SvtInternalOptions_Impl()\nWho has changed the value type of \"Office.Common\\Internal\\SendCrashMail\"?"	);
	seqValues[FIXPROPERTYHANDLE_SLOTCFG			] >>= m_bSlotCFG		;
	seqValues[FIXPROPERTYHANDLE_SENDCRASHMAIL	] >>= m_bSendCrashMail	;
    seqValues[FIXPROPERTYHANDLE_USEMAILUI       ] >>= m_bUseMailUI  ;
	seqValues[FIXPROPERTYHANDLE_CURRENTTEMPURL	] >>= m_aCurrentTempURL	;
//    seqValues[FIXPROPERTYHANDLE_REMOVEMENUENTRYCLOSE ] >>= m_bRemoveMenuEntryClose  ;
//    seqValues[FIXPROPERTYHANDLE_REMOVEMENUENTRYBACKTOWEBTOP ] >>= m_bRemoveMenuEntryBackToWebtop  ;
//    seqValues[FIXPROPERTYHANDLE_REMOVEMENUENTRYNEWWEBTOP ] >>= m_bRemoveMenuEntryNewWebtop  ;
//    seqValues[FIXPROPERTYHANDLE_REMOVEMENUENTRYLOGOUT ] >>= m_bRemoveMenuEntryLogout  ;
/*
	// Read dynamical set "RecoveryList" then.
	// 3 subkeys for every item!
	// Attention: Start at next element after last fixed entry! We must ignore "Slot" and "SendCrashMail" ...
	tIMPL_RecoveryEntry aEntry;
	sal_uInt32 nCount		= seqValues.getLength()	;
	sal_uInt32 nPosition	= FIXPROPERTYCOUNT		;

	while( nPosition<nCount )
	{
		seqValues[nPosition] >>= aEntry.sURL		;
		++nPosition;
		seqValues[nPosition] >>= aEntry.sFilter		;
		++nPosition;
		seqValues[nPosition] >>= aEntry.sTempName	;
		++nPosition;
		m_aRecoveryList.push_front( aEntry );
	}
*/
	// We don't need any notifications here.
	// "Slot" and "SendCrashMail" are readonly(!) and our recovery list should not modified during runtime - it's used
	// by our crash guard only ... otherwise we have a big problem.
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtInternalOptions_Impl::~SvtInternalOptions_Impl()
{
	if( IsModified() == sal_True )
	{
		Commit();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtInternalOptions_Impl::Commit()
{
	// We have to write our current temp URL
	Sequence< OUString > aNames( 1 );
	OUString* pNames = aNames.getArray();
	Sequence< Any > aValues( 1 );
	Any* pValues = aValues.getArray();

	pNames[0] = FIXPROPERTYNAME_CURRENTTEMPURL;
	pValues[0] <<= m_aCurrentTempURL;

	PutProperties( aNames, aValues );

/*
	// Write set of dynamic properties then.
	ClearNodeSet( PROPERTYNAME_RECOVERYLIST );

	tIMPL_RecoveryEntry			aItem					;
	OUString					sNode					;
	Sequence< PropertyValue >	seqPropertyValues( 3 )	;	// Every node in set has 3 sub-nodes!( url, filter, tempname )

	// Copy list entries to save-list and write it to configuration.

	sal_uInt32 nCount = m_aRecoveryList.size();
	for( sal_uInt32 nItem=0; nItem<nCount; ++nItem )
	{
		aItem = m_aRecoveryList.top();
		m_aRecoveryList.pop();
		sNode = PROPERTYNAME_RECOVERYLIST + PATHDELIMITER + FIXR + OUString::valueOf( (sal_Int32)nItem ) + PATHDELIMITER;
		seqPropertyValues[OFFSET_URL		].Name  =	sNode + PROPERTYNAME_URL		;
		seqPropertyValues[OFFSET_FILTER		].Name  =	sNode + PROPERTYNAME_FILTER		;
		seqPropertyValues[OFFSET_TEMPNAME	].Name  =	sNode + PROPERTYNAME_TEMPNAME	;
		seqPropertyValues[OFFSET_URL		].Value <<=	aItem.sURL						;
		seqPropertyValues[OFFSET_FILTER		].Value <<=	aItem.sFilter					;
		seqPropertyValues[OFFSET_TEMPNAME	].Value <<=	aItem.sTempName					;

		SetSetProperties( PROPERTYNAME_RECOVERYLIST, seqPropertyValues );
	}

	tIMPL_RecoveryStack::iterator iRecovery = m_aRecoveryList.begin();
	for ( sal_uInt32 nItem=0; iRecovery != m_aRecoveryList.end(); ++nItem, ++iRecovery)
	{
		aItem = *iRecovery;
		sNode = PROPERTYNAME_RECOVERYLIST + PATHDELIMITER + FIXR +
			OUString::valueOf( (sal_Int32)nItem ) + PATHDELIMITER;
		seqPropertyValues[OFFSET_URL		].Name  =	sNode + PROPERTYNAME_URL		;
		seqPropertyValues[OFFSET_FILTER		].Name  =	sNode + PROPERTYNAME_FILTER		;
		seqPropertyValues[OFFSET_TEMPNAME	].Name  =	sNode + PROPERTYNAME_TEMPNAME	;
		seqPropertyValues[OFFSET_URL		].Value <<=	iRecovery->sURL					;
		seqPropertyValues[OFFSET_FILTER		].Value <<=	iRecovery->sFilter				;
		seqPropertyValues[OFFSET_TEMPNAME	].Value <<=	iRecovery->sTempName			;
		SetSetProperties( PROPERTYNAME_RECOVERYLIST, seqPropertyValues );
	}

    */
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtInternalOptions_Impl::SetCurrentTempURL( const OUString& aNewCurrentTempURL )
{
	m_aCurrentTempURL = aNewCurrentTempURL;
	SetModified();
    Commit();
}

#if 0
//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtInternalOptions_Impl::PushRecoveryItem(	const	OUString&	sURL		,
												const	OUString&	sFilter		,
												const	OUString&	sTempName	)
{
	tIMPL_RecoveryEntry aEntry( sURL, sFilter, sTempName );
	m_aRecoveryList.push_front( aEntry );
	SetModified();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtInternalOptions_Impl::PopRecoveryItem(	OUString&	sURL		,
												OUString&	sFilter		,
												OUString&	sTempName	)
{
	tIMPL_RecoveryEntry aEntry = m_aRecoveryList.front();
	m_aRecoveryList.pop_front();
	SetModified();	// Don't forget it - we delete an entry here!
	sURL		= aEntry.sURL		;
	sFilter		= aEntry.sFilter	;
	sTempName	= aEntry.sTempName	;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtInternalOptions_Impl::IsRecoveryListEmpty() const
{
	return ( m_aRecoveryList.empty() );
}
#endif

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtInternalOptions_Impl::impl_GetPropertyNames()
{
    /*
	// First get ALL names of current existing list items in configuration!
	Sequence< OUString > seqRecoveryItems = GetNodeNames( PROPERTYNAME_RECOVERYLIST );
	// Get information about list counts ...
	sal_Int32 nRecoveryCount = seqRecoveryItems.getLength();
	// ... and create a property list with right size! (+2...for fix properties!) (*3 ... = sub nodes for every set node!)
	Sequence< OUString > seqProperties( FIXPROPERTYCOUNT + (nRecoveryCount*3) );
    */
	Sequence< OUString > seqProperties(4);

	// Add names of fix properties to list.
    seqProperties[FIXPROPERTYHANDLE_SLOTCFG         ]   =   FIXPROPERTYNAME_SLOTCFG         ;
	seqProperties[FIXPROPERTYHANDLE_SENDCRASHMAIL	]	=	FIXPROPERTYNAME_SENDCRASHMAIL	;
    seqProperties[FIXPROPERTYHANDLE_USEMAILUI       ]   =   FIXPROPERTYNAME_USEMAILUI       ;
	seqProperties[FIXPROPERTYHANDLE_CURRENTTEMPURL	]	=	FIXPROPERTYNAME_CURRENTTEMPURL	;
//    seqProperties[FIXPROPERTYHANDLE_REMOVEMENUENTRYCLOSE        ]   =   FIXPROPERTYNAME_REMOVEMENUENTRYCLOSE;
//    seqProperties[FIXPROPERTYHANDLE_REMOVEMENUENTRYBACKTOWEBTOP ]   =   FIXPROPERTYNAME_REMOVEMENUENTRYBACKTOWEBTOP;
//    seqProperties[FIXPROPERTYHANDLE_REMOVEMENUENTRYNEWWEBTOP    ]   =   FIXPROPERTYNAME_REMOVEMENUENTRYNEWWEBTOP;
//    seqProperties[FIXPROPERTYHANDLE_REMOVEMENUENTRYLOGOUT       ]   =   FIXPROPERTYNAME_REMOVEMENUENTRYLOGOUT;
/*
	sal_uInt32 nPosition = FIXPROPERTYCOUNT;
	// Add names for recovery list to list.
	// 3 subkeys for every item!
	// nPosition is the start point of an list item, nItem an index into right list of node names!
	for( sal_Int32 nItem=0; nItem<nRecoveryCount; ++nItem )
	{
		seqProperties[nPosition] = PROPERTYNAME_RECOVERYLIST + PATHDELIMITER + seqRecoveryItems[nItem] + PATHDELIMITER + PROPERTYNAME_URL		;
		++nPosition;
		seqProperties[nPosition] = PROPERTYNAME_RECOVERYLIST + PATHDELIMITER + seqRecoveryItems[nItem] + PATHDELIMITER + PROPERTYNAME_FILTER	;
		++nPosition;
		seqProperties[nPosition] = PROPERTYNAME_RECOVERYLIST + PATHDELIMITER + seqRecoveryItems[nItem] + PATHDELIMITER + PROPERTYNAME_TEMPNAME	;
		++nPosition;
	}
*/
	// Return result.
	return seqProperties;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
SvtInternalOptions_Impl*	SvtInternalOptions::m_pDataContainer	= NULL	;
sal_Int32					SvtInternalOptions::m_nRefCount			= 0		;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtInternalOptions::SvtInternalOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it is not already!
    if( m_pDataContainer == NULL )
	{
        RTL_LOGFILE_CONTEXT(aLog, "unotools ( ??? ) ::SvtInternalOptions_Impl::ctor()");
        m_pDataContainer = new SvtInternalOptions_Impl();

        ItemHolder1::holdConfigItem(E_INTERNALOPTIONS);
 	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtInternalOptions::~SvtInternalOptions()
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
//	public method
//*****************************************************************************************************************
sal_Bool SvtInternalOptions::SlotCFGEnabled() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->SlotCFGEnabled();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtInternalOptions::CrashMailEnabled() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->CrashMailEnabled();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtInternalOptions::MailUIEnabled() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->MailUIEnabled();
}

//*****************************************************************************************************************
//  public methods
//*****************************************************************************************************************
sal_Bool SvtInternalOptions::IsRemoveMenuEntryClose() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsRemoveMenuEntryClose();
}

sal_Bool SvtInternalOptions::IsRemoveMenuEntryBackToWebtop() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsRemoveMenuEntryBackToWebtop();
}

sal_Bool SvtInternalOptions::IsRemoveMenuEntryNewWebtop() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsRemoveMenuEntryNewWebtop();
}

sal_Bool SvtInternalOptions::IsRemoveMenuEntryLogout() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsRemoveMenuEntryLogout();
}

OUString SvtInternalOptions::GetCurrentTempURL() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetCurrentTempURL();
}

void SvtInternalOptions::SetCurrentTempURL( const OUString& aNewCurrentTempURL )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetCurrentTempURL( aNewCurrentTempURL );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtInternalOptions::GetOwnStaticMutex()
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
