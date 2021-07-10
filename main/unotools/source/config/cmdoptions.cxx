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

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <unotools/cmdoptions.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <cppuhelper/weakref.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustrbuf.hxx>

#include <itemholder1.hxx>

#include <algorithm>
#include <hash_map>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::std					;
using namespace ::utl					;
using namespace ::rtl					;
using namespace ::osl					;
using namespace ::com::sun::star::uno	;
using namespace ::com::sun::star::beans	;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define ROOTNODE_CMDOPTIONS								OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Commands/Execute"	))
#define PATHDELIMITER                                   OUString(RTL_CONSTASCII_USTRINGPARAM("/"						))

#define SETNODE_DISABLED                                OUString(RTL_CONSTASCII_USTRINGPARAM("Disabled"					))

#define PROPERTYNAME_CMD                                OUString(RTL_CONSTASCII_USTRINGPARAM("Command"					))

#define PROPERTYCOUNT                                   1

#define OFFSET_CMD                                      0

//_________________________________________________________________________________________________________________
//	private declarations!
//_________________________________________________________________________________________________________________

// Method to retrieve a hash code from a string. May be we have to change it to decrease collisions in the hash map
struct OUStringHashCode
{
    size_t operator()( const ::rtl::OUString& sString ) const
	{
		return sString.hashCode();
	}
};

/*-****************************************************************************************************************
    @descr  support simple command option structures and operations on it
****************************************************************************************************************-*/
class SvtCmdOptions
{
    public:
        //---------------------------------------------------------------------------------------------------------
        // the only way to free memory!
        void Clear()
        {
			m_aCommandHashMap.clear();
		}

		sal_Bool HasEntries() const
        {
            return ( m_aCommandHashMap.size() > 0 );
        }

        void SetContainerSize( sal_Int32 nSize )
		{
			m_aCommandHashMap.rehash( nSize );
		}

		sal_Bool Lookup( const OUString& aCmd ) const
		{
			CommandHashMap::const_iterator pEntry = m_aCommandHashMap.find( aCmd );
			return ( pEntry != m_aCommandHashMap.end() );
		}

		void AddCommand( const OUString& aCmd )
		{
			m_aCommandHashMap.insert( CommandHashMap::value_type( aCmd, 0 ) );
		}

		//---------------------------------------------------------------------------------------------------------
        // convert internal list to external format
        // for using it on right menus really
        // Notice:   We build a property list with 4 entries and set it on result list then.
        //           The while-loop starts with pointer on internal member list lSetupEntries, change to
        //           lUserEntries then and stop after that with NULL!
        //           Separator entries will be packed in another way then normal entries! We define
        //           special strings "sEmpty" and "sSeperator" to perform too ...
        Sequence< OUString > GetList() const
        {
            sal_Int32				nCount = (sal_Int32)m_aCommandHashMap.size();
			sal_Int32				nIndex = 0;
            Sequence< OUString >	aList( nCount );

			CommandHashMap::const_iterator pEntry = m_aCommandHashMap.begin();
			while ( pEntry != m_aCommandHashMap.end() )
				aList[nIndex++] = pEntry->first;

            return aList;
        }

    private:
		class CommandHashMap : public ::std::hash_map< ::rtl::OUString		,
														sal_Int32			,
														OUStringHashCode	,
														::std::equal_to< ::rtl::OUString >	>
		{
			public:
				inline void free()
				{
					CommandHashMap().swap( *this );
				}
		};

        CommandHashMap m_aCommandHashMap;
};

typedef ::std::vector< ::com::sun::star::uno::WeakReference< ::com::sun::star::frame::XFrame > > SvtFrameVector;

class SvtCommandOptions_Impl : public ConfigItem
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

         SvtCommandOptions_Impl();
        ~SvtCommandOptions_Impl();

		//---------------------------------------------------------------------------------------------------------
		//	overloaded methods of baseclass
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		called for notify of configmanager
			@descr		These method is called from the ConfigManager before application ends or from the
			 			PropertyChangeListener if the sub tree broadcasts changes. You must update your
						internal values.

			@seealso	baseclass ConfigItem

            @param      "lPropertyNames" is the list of properties which should be updated.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        virtual void Notify( const Sequence< OUString >& lPropertyNames );

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
            @short      base implementation of public interface for "SvtDynamicMenuOptions"!
            @descr      These class is used as static member of "SvtDynamicMenuOptions" ...
						=> The code exist only for one time and isn't duplicated for every instance!

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        void					Clear       (	SvtCommandOptions::CmdOption	eCmdOption	);
        sal_Bool                HasEntries  (   SvtCommandOptions::CmdOption    eOption     ) const;
		sal_Bool				Lookup		(	SvtCommandOptions::CmdOption	eCmdOption,	const OUString& ) const;
        Sequence< OUString >	GetList		(	SvtCommandOptions::CmdOption	eCmdOption	) const ;
        void					AddCommand	(	SvtCommandOptions::CmdOption	eCmdOption,
												const OUString& sURL		);
        void EstablisFrameCallback(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame);

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
            @short      return list of key names of our configuration management which represent our module tree
			@descr		These methods return the current list of key names! We need it to get needed values from our
                        configuration management and support dynamical menu item lists!

			@seealso	-

            @param      "nDisabledCount"	,   returns count of menu entries for "new"
			@return		A list of configuration key names is returned.

			@onerror	-
		*//*-*****************************************************************************************************/

        Sequence< OUString > impl_GetPropertyNames();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:
        SvtCmdOptions  m_aDisabledCommands;
        SvtFrameVector m_lFrames;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtCommandOptions_Impl::SvtCommandOptions_Impl()
	// Init baseclasses first
    :   ConfigItem( ROOTNODE_CMDOPTIONS )
	// Init member then...
{
    // Get names and values of all accessible menu entries and fill internal structures.
	// See impl_GetPropertyNames() for further informations.
    Sequence< OUString >    lNames              = impl_GetPropertyNames ();
    Sequence< Any >         lValues             = GetProperties         ( lNames         );

	// Safe impossible cases.
	// We need values from ALL configuration keys.
	// Follow assignment use order of values in relation to our list of key names!
    DBG_ASSERT( !(lNames.getLength()!=lValues.getLength()), "SvtCommandOptions_Impl::SvtCommandOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	// Attention: List for names and values have an internal construction pattern!
    sal_Int32	nItem     = 0 ;
    OUString    sCmd		  ;

	// Set size of hash_map reach a used size of approx. 60%
	m_aDisabledCommands.SetContainerSize( lNames.getLength() * 10 / 6 );

	// Get names/values for disabled commands.
    for( nItem=0; nItem < lNames.getLength(); ++nItem )
	{
		// Currently only one value
        lValues[nItem] >>= sCmd;
        m_aDisabledCommands.AddCommand( sCmd );
	}

/*TODO: Not used in the moment! see Notify() ...
	// Enable notification mechanism of our baseclass.
	// We need it to get information about changes outside these class on our used configuration keys! */
    Sequence< OUString > aNotifySeq( 1 );
    aNotifySeq[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( "Disabled" ));
    EnableNotification( aNotifySeq, sal_True );
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtCommandOptions_Impl::~SvtCommandOptions_Impl()
{
	// We must save our current values .. if user forget it!
	if( IsModified() == sal_True )
	{
		Commit();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCommandOptions_Impl::Notify( const Sequence< OUString >& )
{
    MutexGuard aGuard( SvtCommandOptions::GetOwnStaticMutex() );

    Sequence< OUString >    lNames   = impl_GetPropertyNames ();
    Sequence< Any >         lValues  = GetProperties         ( lNames         );

	// Safe impossible cases.
	// We need values from ALL configuration keys.
	// Follow assignment use order of values in relation to our list of key names!
    DBG_ASSERT( !(lNames.getLength()!=lValues.getLength()), "SvtCommandOptions_Impl::SvtCommandOptions_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	// Attention: List for names and values have an internal construction pattern!
    sal_Int32	nItem     = 0 ;
    OUString    sCmd		  ;

	// Set size of hash_map reach a used size of approx. 60%
    m_aDisabledCommands.Clear();
	m_aDisabledCommands.SetContainerSize( lNames.getLength() * 10 / 6 );

	// Get names/values for disabled commands.
    for( nItem=0; nItem < lNames.getLength(); ++nItem )
	{
		// Currently only one value
        lValues[nItem] >>= sCmd;
        m_aDisabledCommands.AddCommand( sCmd );
	}

    // dont forget to update all existing frames and her might cached dispatch objects!
    // But look for already killed frames. We hold weak references instead of hard ones ...
    for (SvtFrameVector::const_iterator pIt  = m_lFrames.begin();
                                        pIt != m_lFrames.end()  ;
                                      ++pIt                     )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xFrame(pIt->get(), ::com::sun::star::uno::UNO_QUERY);
        if (xFrame.is())
            xFrame->contextChanged();
    }
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCommandOptions_Impl::Commit()
{
    DBG_ERROR( "SvtCommandOptions_Impl::Commit()\nNot implemented yet!\n" );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCommandOptions_Impl::Clear( SvtCommandOptions::CmdOption eCmdOption )
{
    switch( eCmdOption )
	{
		case SvtCommandOptions::CMDOPTION_DISABLED:
		{
			m_aDisabledCommands.Clear();
            SetModified();
		}
        break;

		default:
			DBG_ASSERT( sal_False, "SvtCommandOptions_Impl::Clear()\nUnknown option type given!\n" );
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtCommandOptions_Impl::HasEntries( SvtCommandOptions::CmdOption eOption ) const
{
    if ( eOption == SvtCommandOptions::CMDOPTION_DISABLED )
        return ( m_aDisabledCommands.HasEntries() > 0 );
    else
        return sal_False;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Sequence< OUString > SvtCommandOptions_Impl::GetList( SvtCommandOptions::CmdOption eCmdOption ) const
{
    Sequence< OUString > lReturn;

	switch( eCmdOption )
	{
        case SvtCommandOptions::CMDOPTION_DISABLED:
		{
			lReturn = m_aDisabledCommands.GetList();
		}
        break;

		default:
			DBG_ASSERT( sal_False, "SvtCommandOptions_Impl::GetList()\nUnknown option type given!\n" );
	}

	return lReturn;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtCommandOptions_Impl::Lookup( SvtCommandOptions::CmdOption eCmdOption, const OUString& aCommand ) const
{
    switch( eCmdOption )
	{
        case SvtCommandOptions::CMDOPTION_DISABLED:
		{
			return m_aDisabledCommands.Lookup( aCommand );
		}
		default:
			DBG_ASSERT( sal_False, "SvtCommandOptions_Impl::GetList()\nUnknown option type given!\n" );
	}

	return sal_False;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCommandOptions_Impl::AddCommand( SvtCommandOptions::CmdOption eCmdOption, const OUString& sCmd )
{
    switch( eCmdOption )
	{
        case SvtCommandOptions::CMDOPTION_DISABLED:
		{
			m_aDisabledCommands.AddCommand( sCmd );
			SetModified();
		}
		break;

		default:
			DBG_ASSERT( sal_False, "SvtCommandOptions_Impl::GetList()\nUnknown option type given!\n" );
	}
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtCommandOptions_Impl::EstablisFrameCallback(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame)
{
    // check if frame already exists inside list
    // ignore double registrations
    // every frame must be notified one times only!
    ::com::sun::star::uno::WeakReference< ::com::sun::star::frame::XFrame > xWeak(xFrame);
    SvtFrameVector::const_iterator pIt = ::std::find(m_lFrames.begin(), m_lFrames.end(), xWeak);
    if (pIt == m_lFrames.end())
        m_lFrames.push_back(xWeak);
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtCommandOptions_Impl::impl_GetPropertyNames()
{
	// First get ALL names of current existing list items in configuration!
    Sequence< OUString > lDisabledItems      = GetNodeNames( SETNODE_DISABLED, utl::CONFIG_NAME_LOCAL_PATH );

	OUString aSetNode( SETNODE_DISABLED );
	aSetNode += PATHDELIMITER;

	OUString aCommandKey( PATHDELIMITER );
	aCommandKey += PROPERTYNAME_CMD;

	// Expand all keys
	for (sal_Int32 i=0; i<lDisabledItems.getLength(); ++i )
	{
		OUStringBuffer aBuffer( 32 );
		aBuffer.append( aSetNode );
		aBuffer.append( lDisabledItems[i] );
		aBuffer.append( aCommandKey );
		lDisabledItems[i] = aBuffer.makeStringAndClear();
	}

	// Return result.
	return lDisabledItems;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
SvtCommandOptions_Impl*     SvtCommandOptions::m_pDataContainer = NULL  ;
sal_Int32                   SvtCommandOptions::m_nRefCount      = 0     ;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtCommandOptions::SvtCommandOptions()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already exist!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new SvtCommandOptions_Impl;
		ItemHolder1::holdConfigItem(E_CMDOPTIONS);
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtCommandOptions::~SvtCommandOptions()
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
void SvtCommandOptions::Clear( CmdOption eCmdOption )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->Clear( eCmdOption );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtCommandOptions::HasEntries( CmdOption eOption ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->HasEntries( eOption );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtCommandOptions::Lookup( CmdOption eCmdOption, const OUString& aCommandURL ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->Lookup( eCmdOption, aCommandURL );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Sequence< OUString > SvtCommandOptions::GetList( CmdOption eCmdOption ) const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetList( eCmdOption );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtCommandOptions::AddCommand( CmdOption eCmdOption, const OUString& sURL )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->AddCommand( eCmdOption, sURL );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtCommandOptions::EstablisFrameCallback(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame)
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->EstablisFrameCallback(xFrame);
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtCommandOptions::GetOwnStaticMutex()
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
